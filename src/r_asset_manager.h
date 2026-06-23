#pragma once
#include <raylib.h>

/* Okay this has raylib in it so I need to
* change this when I make my own game engine
*/

enum TextureType{
	TILE,
	PORTRAIT,
};

void r_cleanup_textures();
Texture2D *r_get_texture(enum TextureType type, int indx);
