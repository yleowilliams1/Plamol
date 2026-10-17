#define RAYGUI_IMPLEMENTATION
#include "../raygui/raygui.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>


#include "util/util.h"
#include "settings.h"
#include "editor.h"
#include "camera.h"
static const float BASE_PANEL_HEIGHT = 75;
static const float BASE_SIDE_WIDTH = 128;
static const float BASE_CHECK_BOX = 20;
static const float BASE_CHECK_PAD = 10;
static const float BASE_TEXT_SIZE = 12;

static void top_fnc(struct Editor *e, int indx);
static void update_recs(struct Editor *e);
static void draw_panel(struct Editor *e);
static void draw_assets(struct Editor *e);

struct Editor *init_editor(){
	struct Editor *e = XCALLOC(1, sizeof(struct Editor));
	e->available_sprites = LoadDirectoryFiles(STR(SPRITE_PATH));
	e->available_entities = LoadDirectoryFiles(STR(ENTITY_PATH));
	e->available_items = LoadDirectoryFiles(STR(ITEM_PATH));	
	update_recs(e);	
	return e;
}
void update_editor(struct Editor *editor){
	if(!editor){LOG(IS_NULL, "Editor is NULL"); return;}
	if(IsWindowResized()){update_recs(editor);}
	Vector2 screen = GetMousePosition();
	Vector2 world = GetScreenToWorld2D(GetMousePosition(), editor->cam);
	update_cam(&editor->cam);
	update_window();
	if(CheckCollisionPointRec(screen, editor->map_frame)){
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			vf2 wray = (vf2){world.x, world.y};
			v2 tile = world_to_tile(wray);

			bool valid = false;
			if(editor->m){valid = in_bounds(editor->m, tile.x, tile.y);}
			if(valid){
								
			}
		}
	}
}
void draw_editor(struct Editor *e){
	if(!e){LOG(IS_NULL, "Editor is NULL"); return;}
	BeginDrawing();
	ClearBackground(BLACK);
	GuiPanel(e->top, NULL);
	for(int i = 0; i < TOP_BUTTON_COUNT; i++){
		Rectangle r = {i * e->top_delta, e->top.y, e->top_delta, e->panel_h};
		if(GuiButton(r, top_labels[i])){top_fnc(e, i);}
	}
	GuiPanel(e->bottom, NULL);
	for(int i = 0; i < ACTIVE_DATA_COUNT; i++){
		Rectangle cell = {i * e->bottom_delta, e->bottom.y, e->bottom_delta, e->panel_h};
		Rectangle cb = {cell.x + e->pad, cell.y + (cell.height - e->box) / 2, e->box, e->box};
		bool on = (e->active_data == i);
		GuiCheckBox(cb, bottom_labels[i], &on);
		if(on && e->active_data != i){e->active_data = i; e->active_ctrl = 0;}
	}
	GuiPanel(e->left, NULL);
	draw_panel(e);
	GuiPanel(e->right, NULL);
	draw_assets(e);
	EndDrawing();
}
void free_editor(struct Editor *editor){
	if(!editor){LOG(IS_NULL, "Editor is NULL"); return;}
	UnloadDirectoryFiles(editor->available_sprites);
	UnloadDirectoryFiles(editor->available_entities);
	UnloadDirectoryFiles(editor->available_items);
	free(editor);
}
static const char *flag_labels[] = {
	#define X(name) #name,
	TILE_FLAGS
	#undef X
};
#define TYPE_OPTS "GROUND;WALL;PROP;CEILING"
#define DIR_OPTS  "NW;SW;SE;NE;N;W;S;E"

typedef struct {
	struct Editor *e;
	Rectangle area;
	float y, row, gap, pad;
	int id;                       // per-frame control id counter (ids start at 1)

	// the open dropdown is drawn last so its list sits on top of other controls
	bool has_deferred;
	Rectangle def_rect;
	const char *def_opts;
	uint8_t *def_value;
} PanelLayout;

static PanelLayout panel_begin(struct Editor *e){
	PanelLayout l = {0};
	l.e    = e;
	l.area = e->left;
	l.pad  = e->pad;
	l.row  = wincvrt(24, BY_Y);
	l.gap  = wincvrt(6,  BY_Y);
	l.y    = e->left.y + l.gap;

	// while a dropdown is open, everything else ignores input
	if(e->active_ctrl && e->active_dropdown){GuiLock();}
	return l;
}
static void panel_end(PanelLayout *l){
	GuiUnlock();
	if(!l->has_deferred){return;}

	int tmp = *l->def_value;
	if(GuiDropdownBox(l->def_rect, l->def_opts, &tmp, true)){
		l->e->active_ctrl = 0;    // picked an item or clicked the header: close
	}
	*l->def_value = (uint8_t)tmp;
}
static Rectangle panel_row(PanelLayout *l){
	Rectangle r = {l->area.x + l->pad, l->y, l->area.width - l->pad * 2, l->row};
	l->y += l->row + l->gap;
	return r;
}
static void panel_label(PanelLayout *l, const char *text){
	GuiLabel(panel_row(l), text);
}
// typed numeric input
static void panel_value(PanelLayout *l, const char *label, uint32_t *v){
	panel_label(l, label);
	Rectangle r = panel_row(l);
	int id = ++l->id;
	bool editing = (l->e->active_ctrl == id);

	int tmp = (*v > (uint32_t)INT_MAX) ? INT_MAX : (int)*v;
	if(GuiValueBox(r, NULL, &tmp, 0, INT_MAX, editing)){
		if(editing){
			if(l->e->active_ctrl == id){l->e->active_ctrl = 0;}
		}else{
			l->e->active_ctrl = id;
			l->e->active_dropdown = false;
		}
	}
	*v = (uint32_t)tmp;
}
static void panel_dropdown(PanelLayout *l, const char *label, const char *options, uint8_t *v){
	panel_label(l, label);
	Rectangle r = panel_row(l);
	int id = ++l->id;

	if(l->e->active_ctrl == id){      // open: draw later, on top
		l->has_deferred = true;
		l->def_rect  = r;
		l->def_opts  = options;
		l->def_value = v;
		return;
	}
	int tmp = *v;
	if(GuiDropdownBox(r, options, &tmp, false)){
		l->e->active_ctrl = id;
		l->e->active_dropdown = true;
	}
	*v = (uint8_t)tmp;
}
static void panel_flags(PanelLayout *l, uint32_t *flags){
	float box = l->e->box;
	for(int i = 0; i < TILE_FLAG_COUNT; i++){
		Rectangle r = panel_row(l);
		Rectangle cb = {r.x, r.y + (r.height - box) / 2, box, box};
		bool on = (*flags & FLAG_BIT(i)) != 0;
		GuiCheckBox(cb, flag_labels[i], &on);
		if(on){*flags |= FLAG_BIT(i);}
		else  {*flags &= ~FLAG_BIT(i);}
	}
}

// ---- the panel -----------------------------------------------------------

static void draw_panel(struct Editor *e){
	PanelLayout l = panel_begin(e);

	switch(e->active_data){
		case SPRITE_LAYER:
			panel_label(&l, "SPRITE BRUSH");
			panel_dropdown(&l, "Type", TYPE_OPTS, &e->sprite_brush.type);
			panel_dropdown(&l, "Origin dir", DIR_OPTS, &e->sprite_brush.dir);
			panel_value(&l, "Sprite index", &e->sprite_brush.sprite_gindex);
			break;

		case ENTITY_LAYER:
			panel_label(&l, "ENTITY BRUSH");
			panel_dropdown(&l, "Facing", DIR_OPTS, &e->entity_brush.dir);
			panel_value(&l, "Prototype", &e->entity_brush.prototype_gindex);
			panel_value(&l, "Instance", &e->entity_brush.instance_gindex);
			panel_label(&l, "Flags");
			panel_flags(&l, &e->entity_brush.flags);
			break;

		case EXIT_LAYER:
			panel_label(&l, "EXIT BRUSH");
			panel_value(&l, "Target map", &e->exit_brush.map_gindx);
			panel_value(&l, "Target tile X", &e->exit_brush.tx);
			panel_value(&l, "Target tile Y", &e->exit_brush.ty);
			break;

		case FLAG_LAYER:
			panel_label(&l, "TILE FLAGS");
			panel_flags(&l, &e->flag_brush);
			break;

		case MAP_PROPERTIES:
			panel_label(&l, "MAP");
			if(!e->m){
				panel_label(&l, "No map loaded");
				break;
			}
			panel_value(&l, "Map index", &e->m->map_gindx);
			panel_label(&l, TextFormat("Size: %ux%u", e->m->width, e->m->height));
			panel_label(&l, TextFormat("Sprites: %u", e->m->sprite_count));
			panel_label(&l, TextFormat("Entities: %u", e->m->entity_count));
			panel_label(&l, TextFormat("Exits: %u", e->m->exit_count));
			break;

		case ACTIVE_DATA_COUNT: break;
	}
	panel_end(&l);
}
static void draw_assets(struct Editor *e){
}
static void top_fnc(struct Editor *e, int indx){
	switch(indx){
		case NEW_MAP: break;
		case SAVE_MAP: break;
		case LOAD_MAP: break;
		case HIDE_ENTITY: break;
		case HIDE_SPRITES: break;
		case HIDE_EXITS: break; 
	}
}
static void update_recs(struct Editor *e){
	if(!e){LOG(IS_NULL, "Editor is NULL"); return;}
	float sw = (float)GetScreenWidth();
	float sh = (float)GetScreenHeight();

	e->panel_h = wincvrt(BASE_PANEL_HEIGHT, BY_Y);
	e->side_w  = wincvrt(BASE_SIDE_WIDTH,   BY_X);
	e->box     = wincvrt(BASE_CHECK_BOX,    BY_LARGEST);
	e->pad     = wincvrt(BASE_CHECK_PAD,    BY_X);

	e->top    = (Rectangle){0, 0, sw, e->panel_h};
	e->bottom = (Rectangle){0, sh - e->panel_h, sw, e->panel_h};

	e->left  = (Rectangle){0, e->panel_h, e->side_w, sh - (e->panel_h * 2)};
	e->right = (Rectangle){sw - e->side_w, e->panel_h, e->side_w, sh - (e->panel_h * 2)};

	e->top_delta    = sw / TOP_BUTTON_COUNT;
	e->bottom_delta = sw / ACTIVE_DATA_COUNT;

	e->map_frame = (Rectangle){e->side_w, e->panel_h, sw - (e->side_w * 2), sh - (e->panel_h * 2)};
	
	GuiSetStyle(DEFAULT, TEXT_SIZE, (int)wincvrt(BASE_TEXT_SIZE, BY_Y));
}
