#include <sys/stat.h>
#include <raylib.h>
#include <unistd.h>
#include <string.h>
#include "util/util.h"
#include "scene.h"
#include "settings.h"
#include "input.h"
int main(int argc, char *argv[]){		
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		
	if(argc == 2){load_settings(argv[1]);}
	else if(argc == 1){load_settings("engine.ini");}
	else{LOG(ABORT, "Incorrect number of arguments. Provide only one argument as the path to an engine.ini, otherwise provide no arguments for default relative path <engine.ini>");}
	initalize_pran();
	log_init(STR(LOG_PATH));
	
	init_window(STR(WINDOW_NAME));
	update_window();
		
	struct Scene *scene = init_scene();

	while(!WindowShouldClose()){
		update_window();
		update_scene(scene);
		draw_scene(scene);
		draw_scene_ui(scene);
	}	

	CloseWindow();
	free_scene(scene);
	free_settings();
	free_input();
	return 0;
}
