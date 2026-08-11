#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>
#include "e_engine_settings.h"
#include "t_gindex_tool.h"
#include "e_error_handler.h"
#include "l_asset_manager.h"
#include "l_sprite_manager.h"
#include "t_config_tool.h"
#include "t_strings.h"
#define SPRITE_CAP 256

static struct SpriteData *l_grab_sprite(int gindx, bool autoload);
static void sprite_parser(struct config_pack p, void *ptr);

static struct local_indx iman[SPRITE_CAP] = {0};
static struct SpriteData sprites[SPRITE_CAP] = {0};

static Rectangle compute_frame_rec(struct SpriteData *spr, int frame, int animation);

bool l_free_sprite(int gindx){
	int lindx = t_gindx_to_lindx(iman, SPRITE_CAP, gindx);
	if(!t_indxvalid(SPRITE_CAP, lindx)){ERR_LOG(ERR_INDX, "failed gindx %d conversion", gindx); return false;}	
	UnloadTexture(sprites[lindx].texture);
	sprites[lindx] = (struct SpriteData){0};
	ERR_LOG(ERR_OK, "Freed sprite %d", gindx);
	return t_lfree_lindx(iman, SPRITE_CAP, lindx);
}
bool l_load_sprite(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = SPRITE_CAP,
		.arr = sprites,
		.element_size = sizeof(struct SpriteData),
		.function = sprite_parser,
		.path = e_grab_str(SPRITE_META_PATH),
		.init = NULL,
	};

	bool success = l_load_asset(pckg);
	
	int lindx = t_gindx_to_lindx(iman, SPRITE_CAP, gindx);
	if(!t_indxvalid(SPRITE_CAP, lindx)){ERR_LOG(ERR_FUCKED, "Failed to load sprite %d", gindx);}	
	
	char *path = t_png_plus_indx(e_grab_str(SPRITE_PATH), sprites[lindx].metadata.sprite_gindx);	
	sprites[lindx].texture = LoadTexture(path);
	
	ERR_LOG(ERR_OK, "Loaded sprite %d", gindx);
	free(path);
	return success;
}

void l_draw_sprite(int gindx, bool autoload, Vector2 position, int animation, int frame){
	struct SpriteData *spr = l_grab_sprite(gindx, autoload);
	
	Rectangle source = compute_frame_rec(spr, frame, animation);
	Vector2 origin = {
		(float)spr->metadata.origin[animation][frame].x,
		(float)spr->metadata.origin[animation][frame].y,
	};
	Rectangle dest = {
		.x = position.x, 
		.y = position.y,
		.width = source.width,
		.height = source.height,
	};

	// DrawTexturePro already subtracts origin from dest, so the sprite's
	// anchor lands exactly on position. No manual offset needed.
	DrawTexturePro(spr->texture, source, dest, origin, 0.0f, WHITE);
}

static struct SpriteData *l_grab_sprite(int gindx, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, SPRITE_CAP, iman, l_load_sprite);
	if(!t_indxvalid(SPRITE_CAP ,lindx)){ERR_LOG(ERR_FUCKED, "Failed to grab and load sprite gindx %d", gindx);}
	return &sprites[lindx];	
}
static void sprite_parser(struct config_pack p, void *ptr){
	struct SpriteData *spr = (struct SpriteData *)ptr;	
	if(!spr){ERR_LOG(ERR_FUCKED, "Passed null pointer to sprite parser");}
	if(t_check(p.current_section, "metadata")){
		if(t_check(p.key, "sprite_gindx")){
			t_atoi(p.value, &spr->metadata.sprite_gindx);
		} else if(t_check(p.key, "is_animated")){
			int value;
			t_atoi(p.value, &value);
			if(value > 0){spr->metadata.animated = true;} else{
				spr->metadata.animated = false;
			}
		} else if(t_check(p.key, "time_per_frame")){
			t_atof(p.value, &spr->metadata.time_per_frame);
		} else if(t_check(p.key, "animations_y")){
			t_atoi(p.value, &spr->metadata.animations_y);
		} 
		
		int frame_count_indx;
		if(sscanf(p.key, "frame_x.%d", &frame_count_indx) == 1){
			if(frame_count_indx < 0){ERR_LOG(ERR_PARSE, "Tried to parse frame_count with negative index");return;}
			if(frame_count_indx >= MAX_ANIMATIONS){ERR_LOG(ERR_PARSE, "Frame count exceeded MAX_ANIMATIONS %d", MAX_ANIMATIONS); return;}
			t_atoi(p.value, &spr->metadata.frame_x[frame_count_indx]);
		}
		// sscanf returns the number of successful CONVERSIONS, not whether the
		// whole format matched. "origin.anim[0].frame[0].y" still yields 2 for
		// the ".x" format below, because both %d convert before the trailing
		// literal fails to match. Both branches therefore fired on every line,
		// and the .y line overwrote .x with the y value. %n records how much of
		// the key was consumed; compare it against the length to require a full
		// match. (%n does not count toward the return value.)
		int animation_indx_x;
		int frame_indx_x;
		int eaten_x = -1;
		size_t key_len = strlen(p.key);
		if(sscanf(p.key, "origin.anim[%d].frame[%d].x%n", &animation_indx_x, &frame_indx_x, &eaten_x) == 2 && eaten_x == (int)key_len){
			if(frame_indx_x >= MAX_FRAMES){ERR_LOG(ERR_PARSE, "Can't parse frame_indx %d, larger than MAX_FRAMES", frame_indx_x); return;}			
			if(frame_indx_x < 0){return;}
			if(animation_indx_x >= MAX_ANIMATIONS || animation_indx_x < 0){return;}
			t_atoi(p.value, &spr->metadata.origin[animation_indx_x][frame_indx_x].x);
		}
		int animation_indx_y;
		int frame_indx_y;
		int eaten_y = -1;
		if(sscanf(p.key, "origin.anim[%d].frame[%d].y%n", &animation_indx_y, &frame_indx_y, &eaten_y) == 2 && eaten_y == (int)key_len){
			if(frame_indx_y >= MAX_FRAMES){ERR_LOG(ERR_PARSE, "Can't parse frame_indx %d, larger than MAX_FRAMES", frame_indx_y); return;}			
			if(frame_indx_y < 0){return;}
			if(animation_indx_y >= MAX_ANIMATIONS || animation_indx_y < 0){return;}
			t_atoi(p.value, &spr->metadata.origin[animation_indx_y][frame_indx_y].y);
		}
	}
}
static Rectangle compute_frame_rec(struct SpriteData *spr, int frame, int animation){
	int rows = spr->metadata.animations_y > 0 ? spr->metadata.animations_y : 1;
	if(animation < 0 || animation >= rows){
		ERR_LOG(ERR_INDX, "Animation %d out of range (sheet has %d rows), clamping", animation, rows);
		animation = 0;
	}
	int frames_in_row = spr->metadata.animated ? spr->metadata.frame_x[animation] : 1;
	if(frames_in_row <= 0){ERR_LOG(ERR_FUCKED, "Invalid frame count for animation %d", animation); frames_in_row = 1;}
	if(frame < 0 || frame >= frames_in_row){
		ERR_LOG(ERR_INDX, "Frame %d out of range (animation %d has %d frames), clamping", frame, animation, frames_in_row);
		frame = 0;
	}

	float frame_w = (float)spr->texture.width / frames_in_row;
	float frame_h = (float)spr->texture.height / rows;

	return (Rectangle){
		.x = frame * frame_w,
		.y = animation * frame_h,
		.width = frame_w,
		.height = frame_h,
	};
}
