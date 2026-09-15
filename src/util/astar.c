#include <stdlib.h>
#include <limits.h>
#include "util.h"
#include "../map.h"
#define D_ORTHO 10
#define D_DIAG  14
#define NO_PARENT SIZE_MAX

// dx, dy, step cost for each of the 8 neighbours
static const int8_t   NB_DX[8]   = { -1,  1, -1,  1,  0, -1,  0,  1 };
static const int8_t   NB_DY[8]   = { -1, -1,  1,  1, -1,  0,  1,  0 };
static const int32_t  NB_COST[8] = { D_DIAG, D_DIAG, D_DIAG, D_DIAG, D_ORTHO, D_ORTHO, D_ORTHO, D_ORTHO };

/*
 * Binary min-heap of tile indices, ordered by f_score (ties broken by
 * g_score, so we prefer nodes already deeper into the path). heap_pos
 * tracks each node's position in heap[] (NO_PARENT if not present) so
 * we can decrease-key in place instead of pushing duplicates.
 */
struct Heap{
	size_t *data;
	size_t  count;
	size_t *pos;     // node index -> position in data[], NO_PARENT if absent
	int32_t *f_score;
	int32_t *g_score;
};

static bool heap_less(struct Heap *h, size_t a, size_t b){
	if(h->f_score[a] != h->f_score[b]){return h->f_score[a] < h->f_score[b];}
	return h->g_score[a] > h->g_score[b];
}
static void heap_swap(struct Heap *h, size_t i, size_t j){
	size_t tmp = h->data[i];
	h->data[i] = h->data[j];
	h->data[j] = tmp;
	h->pos[h->data[i]] = i;
	h->pos[h->data[j]] = j;
}
static void heap_sift_up(struct Heap *h, size_t i){
	while(i > 0){
		size_t parent = (i - 1) / 2;
		if(!heap_less(h, h->data[i], h->data[parent])){break;}
		heap_swap(h, i, parent);
		i = parent;
	}
}
static void heap_sift_down(struct Heap *h, size_t i){
	for(;;){
		size_t left = 2 * i + 1, right = 2 * i + 2, smallest = i;
		if(left < h->count && heap_less(h, h->data[left], h->data[smallest])){smallest = left;}
		if(right < h->count && heap_less(h, h->data[right], h->data[smallest])){smallest = right;}
		if(smallest == i){break;}
		heap_swap(h, i, smallest);
		i = smallest;
	}
}
static void heap_push(struct Heap *h, size_t node){
	size_t i = h->count++;
	h->data[i] = node;
	h->pos[node] = i;
	heap_sift_up(h, i);
}
static void heap_decrease(struct Heap *h, size_t node){
	// f/g_score for node were already lowered by the caller; just re-seat it
	heap_sift_up(h, h->pos[node]);
}
static size_t heap_pop(struct Heap *h){
	size_t top = h->data[0];
	h->count--;
	h->data[0] = h->data[h->count];
	h->pos[h->data[0]] = 0;
	h->pos[top] = NO_PARENT;
	if(h->count > 0){heap_sift_down(h, 0);}
	return top;
}

bool in_bounds(struct Map *m, int x, int y){
	return x >= 0 && y >= 0 && x < (int)m->width && y < (int)m->height;
}
static inline bool tile_blocked(struct Map *m, int x, int y){
	return (m->tile_flag[(size_t)y * m->width + (size_t)x] & (1 << COLLIDE)) != 0;
}
static inline int32_t heuristic(int dx, int dy){
	dx = dx < 0 ? -dx : dx;
	dy = dy < 0 ? -dy : dy;
	int32_t mn = dx < dy ? dx : dy;
	int32_t mx = dx < dy ? dy : dx;
	return D_DIAG * mn + D_ORTHO * (mx - mn);
}

bool astar_find_path(struct Map *m, v2 s, v2 e, struct PathNode **out_path, size_t *out_len){
	if(!m){LOG(IS_NULL, "Map is NULL");return false;}
	if(!out_path || !out_len){LOG(IS_NULL, "Output pointer is NULL");return false;}
	int sx = s.x;
	int sy = s.y;
	int gx = e.x;
	int gy = e.y;
	if(!in_bounds(m, sx, sy) || !in_bounds(m, gx, gy)){
		LOG(INDEX, "Start or goal is outside the map bounds");
		return false;
	}
	if(tile_blocked(m, sx, sy) || tile_blocked(m, gx, gy)){
		// Not a hard error - callers path toward doors/monsters that
		// step off blocked tiles often enough that this shouldn't log.
		return false;
	}
	if(sx == gx && sy == gy){
		*out_path = XCALLOC(1, sizeof(struct PathNode));
		(*out_path)[0].x = sx;
		(*out_path)[0].y = sy;
		*out_len = 1;
		return true;
	}

	size_t tile_count = (size_t)m->width * (size_t)m->height;
	int32_t *g_score = XCALLOC(tile_count, sizeof(int32_t));
	int32_t *f_score = XCALLOC(tile_count, sizeof(int32_t));
	size_t  *parent  = XCALLOC(tile_count, sizeof(size_t));
	uint8_t *state   = XCALLOC(tile_count, sizeof(uint8_t)); // 0 unvisited, 1 open, 2 closed

	struct Heap heap = {
		.data = XCALLOC(tile_count, sizeof(size_t)),
		.count = 0,
		.pos = XCALLOC(tile_count, sizeof(size_t)),
		.f_score = f_score,
		.g_score = g_score,
	};
	for(size_t i = 0; i < tile_count; i++){parent[i] = NO_PARENT; heap.pos[i] = NO_PARENT;}

	size_t start = (size_t)sy * m->width + sx;
	size_t goal  = (size_t)gy * m->width + gx;

	g_score[start] = 0;
	f_score[start] = heuristic((int)gx - (int)sx, (int)gy - (int)sy);
	state[start] = 1;
	heap_push(&heap, start);

	bool found = false;
	while(heap.count > 0){
		size_t current = heap_pop(&heap);
		if(current == goal){found = true;break;}
		state[current] = 2;

		int cx = (int)(current % m->width);
		int cy = (int)(current / m->width);

		for(int dir = 0; dir < 8; dir++){
			int nx = cx + NB_DX[dir];
			int ny = cy + NB_DY[dir];
			if(!in_bounds(m, nx, ny)){continue;}
			if(tile_blocked(m, nx, ny)){continue;}

			// don't let diagonal moves cut across a blocked corner
			if(NB_DX[dir] != 0 && NB_DY[dir] != 0){
				if(tile_blocked(m, cx + NB_DX[dir], cy) || tile_blocked(m, cx, cy + NB_DY[dir])){continue;}
			}

			size_t neighbor = (size_t)ny * m->width + nx;
			if(state[neighbor] == 2){continue;}

			int32_t tentative_g = g_score[current] + NB_COST[dir];
			if(state[neighbor] == 0 || tentative_g < g_score[neighbor]){
				parent[neighbor] = current;
				g_score[neighbor] = tentative_g;
				f_score[neighbor] = tentative_g + heuristic((int)gx - nx, (int)gy - ny);
				if(state[neighbor] == 1){
					heap_decrease(&heap, neighbor);
				}else{
					state[neighbor] = 1;
					heap_push(&heap, neighbor);
				}
			}
		}
	}

	if(found){
		size_t len = 1;
		for(size_t n = goal; n != start; n = parent[n]){len++;}

		struct PathNode *path = XCALLOC(len, sizeof(struct PathNode));
		size_t n = goal;
		for(size_t i = len; i-- > 0;){
			path[i].x = (tilsize)(n % m->width);
			path[i].y = (tilsize)(n / m->width);
			if(n == start){break;}
			n = parent[n];
		}

		*out_path = path;
		*out_len = len;
	}

	free(g_score);
	free(f_score);
	free(parent);
	free(state);
	free(heap.data);
	free(heap.pos);

	return found;
}
