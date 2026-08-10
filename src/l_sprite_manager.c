#include <stdio.h>
#include <stdbool.h>
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
static void set_frame_rec(struct SpriteData *spr){
	int frames_in_row = spr->metadata.animated ? spr->metadata.frame_x[spr->runtime.current_animation] : 1;
	if(frames_in_row <= 0){ERR_LOG(ERR_FUCKED, "Invalid frame count for animation %d", spr->runtime.current_animation); return;}

	float frame_w = (float)spr->texture.width / frames_in_row;
	float frame_h = (float)spr->texture.height / (spr->metadata.animations_y > 0 ? spr->metadata.animations_y : 1);

	spr->runtime.frame_rec = (Rectangle){
		.x = spr->runtime.current_frame * frame_w,
		.y = spr->runtime.current_animation * frame_h,
		.width = frame_w,
		.height = frame_h,
	};
}
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
	
	sprites[lindx].runtime = (struct SpriteRuntime){0};
	set_frame_rec(&sprites[lindx]);
	ERR_LOG(ERR_OK, "Loaded sprite %d", gindx);
	free(path);
	return success;
}
void l_play_sprite(int gindx, int autoload){
	struct SpriteData *spr = l_grab_sprite(gindx, autoload);
	if(!spr->metadata.animated){return;}
	spr->runtime.frame_count += GetFrameTime();
	if(spr->runtime.frame_count >= spr->metadata.time_per_frame){
		spr->runtime.frame_count = 0.0f;
		spr->runtime.current_frame += 1;	
		if(spr->runtime.current_frame >= spr->metadata.frame_x[spr->runtime.current_animation]){
			spr->runtime.current_frame = 0;
		}
	}

	set_frame_rec(spr);
}
void l_draw_sprite(int gindx, bool autoload, Vector2 position){
	struct SpriteData *spr = l_grab_sprite(gindx, autoload);

	Rectangle dest = {
		.x = position.x,
		.y = position.y,
		.width = spr->runtime.frame_rec.width,
		.height = spr->runtime.frame_rec.height,
	};
	Vector2 origin = {
		spr->runtime.frame_rec.width / 2.0f,
		spr->runtime.frame_rec.height / 2.0f,
	};

	DrawTexturePro(spr->texture, spr->runtime.frame_rec, dest, origin, spr->runtime.rotation, WHITE);
}
void l_reset_sprite(int gindx, int new_animation){
	struct SpriteData *spr = l_grab_sprite(gindx, true);
	spr->runtime.current_frame = 0;
	spr->runtime.current_animation = new_animation;
}
Vector2 l_grab_sprite_scale(int gindx, bool autoload){
	struct SpriteData *spr = l_grab_sprite(gindx, autoload);
	return (Vector2){.x = 32, .y = 64};
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
	}
}

