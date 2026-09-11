#include <stdlib.h>
#include "entity.h"
#include "sprite.h"
#include "map.h"
#include "draw.h"
#include "save.h"
#include "settings.h"
#include "scene.h"
#include "util/util.h"

struct Scene *init_scene(){
	struct Scene *s = XCALLOC(1, sizeof(struct Scene));
	s->state = UNINITALIZED;
	s->entity_manager= create_entity_manager();
	s->save_manager = create_save_manager();
	s->sprites_manager = create_sprite_manager();
	int found = load_all_saves(s->save_manager, STR(SAVE_PATH));
	if(found == 0){
		// New Game so load map 0
		char *path = format_path(STR(MAP_PATH), ".MAP", 0);		
		s->map = read_map(path);
		load_map_entities(s->entity_manager, s->map, s->save_manager, 0);
		free(path);
	} else{
		LOG(LOAD, "Found %d save file(s)", found);
		// Save selection here then load map and entities with selected save
	}
	return s;
}
void update_scene(struct Scene *scene){
}
void draw_scene(struct Scene *scene){
}
void draw_scene_ui(struct Scene *scene){
}
void free_scene(struct Scene *scene){
	if(!scene){return;}
	if(scene->entity_manager){free_entity_manager(&scene->entity_manager); scene->entity_manager = NULL;}
	if(scene->map){free_map(scene->map); scene->map = NULL;}	
	if(scene->save_manager){free_save_manager(&scene->save_manager); scene->save_manager = NULL;}
	if(scene->sprites_manager){free_sprite_manager(&scene->sprites_manager); scene->sprites_manager = NULL;}
	scene->state = FREED;
}
