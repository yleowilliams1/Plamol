#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../imgui_backend/imgui_impl_raylib.h"
#include "../imgui_backend/rlcimgui.h"
#include "editor.h"
#include "util/util.h"
#include "draw.h"
#include "entity.h"
#include "map.h"
#include "sprite.h"
#include "camera.h"
#include "settings.h"
static void update_ui(struct Editor *editor);
void draw_editor_ui(struct Editor *editor);

static const char *type_items[] = {"GROUND", "WALL", "PROP", "CEILING"};
static const char *dir_items[]  = {"NW", "SW", "SE", "NE", "N", "W", "S", "E"};
static const char *flag_items[] = {"COLLIDE", "DIRT_SOUND", "GRASS_SOUND", "STONE_SOUND"};
static const char *entity_flag_names[] = {"PLAYER", "HOSTILE", "DEAD", "PASSTHROUGH", "WORLD", "DOOR"};
static void draw_tile(v2 tile, Color clr) {
	float tileWidth = TILE_W;
	float tileHeight = TILE_H;
	float halfW = tileWidth / 2.0f;
	float halfH = tileHeight / 2.0f;

	vf2 position = tile_to_world(tile);
	Vector2 top    = { position.x,         position.y - halfH };
	Vector2 right  = { position.x + halfW, position.y };
	Vector2 bottom = { position.x,         position.y + halfH };
	Vector2 left   = { position.x - halfW, position.y };

	// 1. Draw semi-transparent filled background
	Color fillColor = (Color){ 255, 255, 255, 50 }; // Light gray/white with alpha
	DrawTriangle(top, left, right, fillColor);
	DrawTriangle(left, bottom, right, fillColor);

	// 2. Draw border lines
	Color borderColor = clr;
	DrawLineV(top, right, borderColor);
	DrawLineV(right, bottom, borderColor);
	DrawLineV(bottom, left, borderColor);
	DrawLineV(left, top, borderColor);
}
static void load_entity(struct Editor *editor, int index){
	if(!editor){return;}
	struct EntityImmutable **e = &editor->entity->immutable_entity[index];
	if(*e){return;}
	*e = load_entity_immutable_from_disk(index);
}

static void load_sprite(struct Editor *editor, int index){
	if(!editor){return;}
	struct SpriteData **s = &editor->sprite->sprite[index];
	if(*s){return;}
	*s = load_sprite_from_disk(index);
}
struct Editor *init_editor(){
	igCreateContext(NULL);
	ImGuiIO *io = igGetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#ifdef IMGUI_HAS_DOCK
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

	igStyleColorsDark(NULL);

	ImGui_ImplRaylib_Init();
	ImFontAtlas_AddFontDefault(io->Fonts, NULL);
	ImGui_ImplRaylib_BuildFontAtlas();   

	struct Editor *editor = XCALLOC(1, sizeof(struct Editor));
	editor->cam.target = (Vector2){0, 0};
	editor->cam.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
	editor->cam.rotation = 0.0f;
	editor->cam.zoom = 1.0f;
	
	editor->sprite = create_sprite_manager();
	editor->entity = create_entity_manager();
	
	// Default values
	editor->current_layer = LAYER_NONE;
	editor->paint_flag = COLLIDE;
	editor->paint_sprite = (struct Sprite){.sprite_gindex = 0, .tx = 0, .ty = 0, .type = GROUND, .dir = N};
	editor->paint_entity = (struct Entity){.dir = N, .prototype_gindex = 0, .instance_gindex = 0, .tx = 0, .ty = 0, .flags = 0};
	editor->paint_exit = (struct ExitTile){.map_gindx = 0, .tx = 0, .ty = 0};
	return editor;
}
void update_editor(struct Editor *editor){
	if(!editor){LOG(IS_NULL, "Editor is NULL");return;}
	update_ui(editor);
	update_cam(&editor->cam);
	editor->mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), editor->cam);

	v2 tile_pos = world_to_tile((vf2){editor->mouse_world_pos.x, editor->mouse_world_pos.y});

	if(editor->map){
		// Out of bounds - keep last frame's tile pos, don't update this frame.
		if(tile_pos.x >= 0 && tile_pos.x < editor->map->width && tile_pos.y >= 0 && tile_pos.y < editor->map->height){
			editor->mouse_tile_pos = tile_pos;
			editor->mouse_tile_world_pos = tile_to_world(tile_pos);
		}
	}else{
		editor->mouse_tile_pos = tile_pos;
		editor->mouse_tile_world_pos = tile_to_world(tile_pos);
	}

	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
		if(!editor->map){return;}
		if(igGetIO()->WantCaptureMouse){return;} // click landed on an ImGui window/popup, not the map
		struct Map *map = editor->map;
		bool on_grid = editor->mouse_tile_pos.x >= 0 && editor->mouse_tile_pos.x < map->width
		            && editor->mouse_tile_pos.y >= 0 && editor->mouse_tile_pos.y < map->height;
		if(!on_grid){return;}
		switch(editor->current_layer){	
			case LAYER_NONE:
				break;
			case LAYER_SPRITE: {
				
				editor->paint_sprite.tx = (tilsize)editor->mouse_tile_pos.x;
				editor->paint_sprite.ty = (tilsize)editor->mouse_tile_pos.y;

				bool found = false;
				for(int i = 0; i < map->sprite_count; i++){
					if(map->sprite[i].tx == editor->paint_sprite.tx
					&& map->sprite[i].ty == editor->paint_sprite.ty
					&& map->sprite[i].sprite_gindex == editor->paint_sprite.sprite_gindex){
						// Same sprite already painted on this exact tile - override it.
						// Different gindex on the same tile is allowed (stacked sprites), so it isn't a match.
						map->sprite[i] = editor->paint_sprite;
						found = true;
						break;
					}
				}
				if(!found){
					// Nothing here yet - grow the array by one and append.
					struct Sprite *grown = XCALLOC(map->sprite_count + 1, sizeof(struct Sprite));
					if(map->sprite_count > 0){
						memcpy(grown, map->sprite, (size_t)map->sprite_count * sizeof(struct Sprite));
					}
					grown[map->sprite_count] = editor->paint_sprite;
					if(map->sprite){free(map->sprite);}
					map->sprite = grown;
					map->sprite_count++;
					load_sprite(editor, editor->paint_sprite.sprite_gindex);	
				}
				break;
			}
			case LAYER_ENTITY: {
				editor->paint_entity.tx = (tilsize)editor->mouse_tile_pos.x;
				editor->paint_entity.ty = (tilsize)editor->mouse_tile_pos.y;

				bool found = false;
				for(int i = 0; i < map->entity_count; i++){
					if(map->entity[i].tx == editor->paint_entity.tx
					&& map->entity[i].ty == editor->paint_entity.ty
					&& map->entity[i].instance_gindex == editor->paint_entity.instance_gindex){
						map->entity[i] = editor->paint_entity;
						found = true;
						break;
					}
				}
				if(!found){
					struct Entity *grown = XCALLOC(map->entity_count + 1, sizeof(struct Entity));
					if(map->entity_count > 0){
						memcpy(grown, map->entity, (size_t)map->entity_count * sizeof(struct Entity));
					}
					grown[map->entity_count] = editor->paint_entity;
					if(map->entity){free(map->entity);}
					map->entity = grown;
					map->entity_count++;
				}
				break;
			}
			case LAYER_EXIT: {
				editor->paint_exit.tx = (tilsize)editor->mouse_tile_pos.x;
				editor->paint_exit.ty = (tilsize)editor->mouse_tile_pos.y;

				bool found = false;
				for(int i = 0; i < map->exit_count; i++){
					if(map->exit[i].tx == editor->paint_exit.tx && map->exit[i].ty == editor->paint_exit.ty){
						map->exit[i] = editor->paint_exit;
						found = true;
						break;
					}
				}
				if(!found){
					struct ExitTile *grown = XCALLOC(map->exit_count + 1, sizeof(struct ExitTile));
					if(map->exit_count > 0){
						memcpy(grown, map->exit, (size_t)map->exit_count * sizeof(struct ExitTile));
					}
					grown[map->exit_count] = editor->paint_exit;
					if(map->exit){free(map->exit);}
					map->exit = grown;
					map->exit_count++;
				}
				break;
			}
			case LAYER_FLAG: {
				// Not a growable list - tile_flag is one bitmask per tile,
				// indexed directly by position, so just set the bit.
				size_t idx = (size_t)editor->mouse_tile_pos.y * map->width + (size_t)editor->mouse_tile_pos.x;
				map->tile_flag[idx] |= (tile_flag_size)(1u << editor->paint_flag);
				break;
			}
		}

	}

	if(IsKeyPressed(KEY_R)){
		if(!editor->map){return;}
		if(igGetIO()->WantCaptureMouse){return;}
		struct Map *map = editor->map;
		bool on_grid = editor->mouse_tile_pos.x >= 0 && editor->mouse_tile_pos.x < map->width
		            && editor->mouse_tile_pos.y >= 0 && editor->mouse_tile_pos.y < map->height;
		if(!on_grid){return;}
		tilsize tx = (tilsize)editor->mouse_tile_pos.x;
		tilsize ty = (tilsize)editor->mouse_tile_pos.y;

		switch(editor->current_layer){
			case LAYER_NONE:
				break;
			case LAYER_SPRITE: {
				// A tile can hold several stacked sprites (different gindex) -
				// clear all of them, not just the first match.
				for(int i = 0; i < map->sprite_count; i++){
					if(map->sprite[i].tx == tx && map->sprite[i].ty == ty){
						memmove(&map->sprite[i], &map->sprite[i+1], (size_t)(map->sprite_count - i - 1) * sizeof(struct Sprite));
						map->sprite_count--;
						i--; // recheck this slot - another stacked sprite may have shifted into it
					}
				}
				break;
			}
			case LAYER_ENTITY: {
				for(int i = 0; i < map->entity_count; i++){
					if(map->entity[i].tx == tx && map->entity[i].ty == ty){
						memmove(&map->entity[i], &map->entity[i+1], (size_t)(map->entity_count - i - 1) * sizeof(struct Entity));
						map->entity_count--;
						i--;
					}
				}
				break;
			}
			case LAYER_EXIT: {
				for(int i = 0; i < map->exit_count; i++){
					if(map->exit[i].tx == tx && map->exit[i].ty == ty){
						memmove(&map->exit[i], &map->exit[i+1], (size_t)(map->exit_count - i - 1) * sizeof(struct ExitTile));
						map->exit_count--;
						i--;
					}
				}
				break;
			}
			case LAYER_FLAG: {
				size_t idx = (size_t)ty * map->width + (size_t)tx;
				map->tile_flag[idx] = 0;
				break;
			}
		}
	}
}
void draw_editor(struct Editor *editor){
	if(!editor){return;}
	BeginDrawing();
	ClearBackground(RAYWHITE);
	BeginMode2D(editor->cam);
	if(editor->map){
		for(int y = 0; y < editor->map->height; y++){
			for(int x = 0; x < editor->map->width; x++){
				draw_tile((v2){x, y}, GREEN);
			}
		}		
		draw_map(editor->entity, editor->map, editor->sprite);
		DrawCircle(editor->mouse_tile_world_pos.x, editor->mouse_tile_world_pos.y, 5.0f, BLUE);
	}
	EndMode2D();
	ImGui_ImplRaylib_RenderDrawData(igGetDrawData());  
	EndDrawing();
}
void free_editor(struct Editor *editor){
	if(!editor){return;}

	if(editor->map){free_map(editor->map);}
	LOG(LOAD, "Is mapped");
	if(editor->entity){free_entity_manager(&editor->entity);}
	LOG(LOAD, "Is entitied");
	if(editor->sprite){free_sprite_manager(&editor->sprite);}
	LOG(LOAD, "Is sprited");
	free(editor);
	LOG(LOAD, "Is editored freed");
	// This crashes btw. Idk why but who gives a fuck. This ends last so the gpu stuff is prolly fine. It's fine leavem alone 
	ImGui_ImplRaylib_Shutdown();
	LOG(LOAD, "Is imgui");
	igDestroyContext(NULL);
}
static void update_ui(struct Editor *editor){
	ImGui_ImplRaylib_ProcessEvents();
	ImGui_ImplRaylib_NewFrame();
	igNewFrame();
	
	static bool new = false;
	static bool open = false;
	static bool write = false;
	
	static bool lsprite = false;
	static bool lentity = false;
	static bool lexit = false;
	static bool lflag = false;
	
	static int map_num = 0;
	if(igBeginMainMenuBar()){
		if(igBeginMenu("File", true)){
			if(igMenuItem_Bool("New", NULL, false, true)){new = true;}
			if(igMenuItem_Bool("Read", NULL, false, true)){open = true;}
			if(igMenuItem_Bool("Write", NULL, false, true)){write = true;}
			igSeparator();
			igEndMenu();
		}
		if(igBeginMenu("Layer", true)){
			if(igMenuItem_Bool("Sprite Layer", NULL, false, true)){editor->current_layer = LAYER_SPRITE; lsprite = true;}
			if(igMenuItem_Bool("Entity Layer", NULL, false ,true)){editor->current_layer = LAYER_ENTITY; lentity = true;}
			if(igMenuItem_Bool("Exit Layer", NULL, false , true)){editor->current_layer = LAYER_EXIT; lexit = true;}
			if(igMenuItem_Bool("Flag Layer", NULL, false, true)){editor->current_layer = LAYER_FLAG; lflag = true;}
			igSeparator();
			igEndMenu();
		}
		igEndMainMenuBar();
	}

	if(!igGetIO()->WantCaptureMouse && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
		switch(editor->current_layer){
			case LAYER_SPRITE: lsprite = true; break;
			case LAYER_ENTITY: lentity = true; break;
			case LAYER_EXIT:   lexit   = true; break;
			case LAYER_FLAG:   lflag   = true; break;
			default: break;
		}
	}

	if(new){igOpenPopup_Str("New", 0); new = false;}
	if(open){igOpenPopup_Str("Open", 0); open = false;}
	if(write){igOpenPopup_Str("Write", 0); write = false;}

	if(lsprite){igOpenPopup_Str("Sprite", 0); lsprite = false;}
	if(lentity){igOpenPopup_Str("Entity", 0); lentity = false;}
	if(lexit){igOpenPopup_Str("Exit", 0); lexit = false;}
	if(lflag){igOpenPopup_Str("Flag", 0); lflag = false;}
	
	if(igBeginPopupModal("Sprite", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		int gindex = editor->paint_sprite.sprite_gindex;
		int type_idx = editor->paint_sprite.type;
		int dir_idx = editor->paint_sprite.dir;

		igInputInt("Sprite Gindex", &gindex, 1, 10, 0);
		igCombo_Str_arr("Type", &type_idx, type_items, 4, -1);
		igCombo_Str_arr("Dir", &dir_idx, dir_items, DIR_COUNT, -1);

		if(gindex < 0){gindex = 0;}

		editor->paint_sprite.sprite_gindex = (indsize)gindex;
		editor->paint_sprite.type = (uint8_t)type_idx;
		editor->paint_sprite.dir = (uint8_t)dir_idx;

		igSpacing(); igSeparator(); igSpacing();
		if(igButton("Done", (ImVec2){120, 0})){igCloseCurrentPopup();}
		igEndPopup();
	}

	if(igBeginPopupModal("Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		int proto = editor->paint_entity.prototype_gindex;
		int inst = editor->paint_entity.instance_gindex;
		int dir_idx = editor->paint_entity.dir;

		igInputInt("Prototype Gindex", &proto, 1, 10, 0);
		igInputInt("Instance Gindex", &inst, 1, 10, 0);
		igCombo_Str_arr("Dir", &dir_idx, dir_items, DIR_COUNT, -1);

		igSpacing();
		igText("Flags");
		for(int i=0; i<ENTITY_FLAG_COUNT; i++){
			bool set = (editor->paint_entity.flags >> i) & 1u;
			igCheckbox(entity_flag_names[i], &set);
			if(set){editor->paint_entity.flags |= (1u << i);}
			else{editor->paint_entity.flags &= ~(1u << i);}
		}

		if(proto < 0){proto = 0;}
		if(inst < 0){inst = 0;}

		editor->paint_entity.prototype_gindex = (indsize)proto;
		editor->paint_entity.instance_gindex = (indsize)inst;
		editor->paint_entity.dir = (uint8_t)dir_idx;

		igSpacing(); igSeparator(); igSpacing();
		if(igButton("Done", (ImVec2){120, 0})){igCloseCurrentPopup();}
		igEndPopup();
	}

	if(igBeginPopupModal("Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		int map_gindx = editor->paint_exit.map_gindx;
		int tx = editor->paint_exit.tx;
		int ty = editor->paint_exit.ty;

		igInputInt("Target Map Gindex", &map_gindx, 1, 10, 0);

		if(map_gindx < 0){map_gindx = 0;}

		editor->paint_exit.map_gindx = (indsize)map_gindx;

		igSpacing(); igSeparator(); igSpacing();
		if(igButton("Done", (ImVec2){120, 0})){igCloseCurrentPopup();}
		igEndPopup();
	}

	if(igBeginPopupModal("Flag", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		int flag_idx = editor->paint_flag;
		igCombo_Str_arr("Tile Flag", &flag_idx, flag_items, 4, -1);
		editor->paint_flag = (enum TileFlags)flag_idx;

		igSpacing(); igSeparator(); igSpacing();
		if(igButton("Done", (ImVec2){120, 0})){igCloseCurrentPopup();}
		igEndPopup();
	}	
	if(igBeginPopupModal("New", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		static int width = 64;
		static int height = 64;
		igInputInt("Map Index", &map_num, 1, 100, 0);		
		igInputInt("Width", &width, 1, 100, 0);
		igInputInt("Height", &height, 1, 100, 0);
		if(width < 1){width = 1;}
		if(height < 1){height = 1;}
		if(map_num < 0){map_num = 0;}
		igSpacing();
		igSeparator();
		igSpacing();
		if(igButton("Create", (ImVec2){120, 0})){
			if(editor->map){
				unload_map_sprites(editor->sprite, editor->map);
				free_map(editor->map);
				unload_map_entities(editor->entity, editor->map);
			}
			editor->map = NULL;
			editor->map = create_map(width, height, map_num); 
			igCloseCurrentPopup();
		}	
		igSetItemDefaultFocus();
		igSameLine(0.0f, -1.0f);
		if (igButton("Cancel", (ImVec2){120, 0})) {
			igCloseCurrentPopup();
		}
		igEndPopup();
	}
	if(igBeginPopupModal("Open", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		igInputInt("Map Index", &map_num, 1, 100, 0);	
		if(map_num < 0){map_num = 0;}
		igSpacing();
		igSeparator();
		igSpacing();
		if(igButton("Open", (ImVec2){120, 0})){
			if(editor->map){
				unload_map_sprites(editor->sprite, editor->map);
				free_map(editor->map);
				unload_map_entities(editor->entity, editor->map);
			}
			char *path = format_path(STR(MAP_PATH), "MAP", map_num);
			editor->map = read_map(path);		
			free(path);
			load_map_sprites(editor->sprite, editor->map);			
			igCloseCurrentPopup();
		}
		igSetItemDefaultFocus();
		igSameLine(0.0f, -1.0f);
		if (igButton("Cancel", (ImVec2){120, 0})) {
			igCloseCurrentPopup();
		}
		igEndPopup();

	}
	if(igBeginPopupModal("Write", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		igInputInt("Map Index", &map_num, 1, 100, 0);	
		if(map_num < 0){map_num = 0;}
		igSpacing();
		igSeparator();
		igSpacing();
		if(igButton("Write", (ImVec2){120, 0})){
			if(editor->map){
				char *path = format_path(STR(MAP_PATH), "MAP", map_num);
				write_map(editor->map, path);		
				free(path);				
			} else{LOG(IS_NULL, "Can't write map is NULL");}
			igCloseCurrentPopup();
		}
		igSetItemDefaultFocus();
		igSameLine(0.0f, -1.0f);
		if (igButton("Cancel", (ImVec2){120, 0})) {
			igCloseCurrentPopup();
		}
		igEndPopup();

	}
	igRender();
}
