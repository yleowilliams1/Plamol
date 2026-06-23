#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <stdbool.h>
#include "r_asset_manager.h"
#include "e_engine_settings.h"

#define MAX_TEXTURES 512


Texture2D tiles[MAX_TEXTURES] = {0};
bool      is_tile_loaded[MAX_TEXTURES] = {0};

Texture2D portraits[MAX_TEXTURES]= {0};
bool	  is_portrait_loaded[MAX_TEXTURES] = {0};

char *h_get_path(int indx, char *path){
	if(!path){return NULL;}
	char *full = malloc(512);
	if(!full){return NULL;}
	snprintf(full, 512, "%s/%d.png", path, indx);
	if (!FileExists(full)) {
		fprintf(stderr, "TILE: file not found: %s\n", full);
		free(full);
		return NULL;
	}

	return full;
}

void r_cleanup_textures(){
	for(int i = 0; i < MAX_TEXTURES; i++){
		if(is_tile_loaded[i]){
			UnloadTexture(tiles[i]);
		}
		if(is_portrait_loaded[i]){
			UnloadTexture(portraits[i]);
		}	
	}
}

Texture2D *r_get_texture(enum TextureType type, int indx){
	if(indx < 0 || indx > MAX_TEXTURES){return NULL;}
	char *full;
	switch (type){
		case TILE:
			if(is_tile_loaded[indx]){return &tiles[indx];}
			full = h_get_path(indx, e_get_tile_path());	
			tiles[indx] = LoadTexture(full);
			is_tile_loaded[indx] = true;
			free(full);
			return &tiles[indx];
			break;
		case PORTRAIT:
			if(is_portrait_loaded[indx]){return &portraits[indx];}
			full = h_get_path(indx, e_get_portrait_path());
			portraits[indx] = LoadTexture(full);
			is_portrait_loaded[indx] = true;
			free(full);
			return &portraits[indx];
			break;
		default:
			if(is_tile_loaded[indx]){return &tiles[indx];}
			full = h_get_path(indx, e_get_tile_path());	
			tiles[indx] = LoadTexture(full);
			is_tile_loaded[indx] = true;
			free(full);
			return &tiles[indx];
			break;
	}	
}
