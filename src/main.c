#include <sys/stat.h>
#include <raylib.h>
#include <unistd.h>
#include <string.h>
#include "util/util.h"
#include "editor.h"
#include "scene.h"
#include "settings.h"
#include "input.h"
#define EDITOR 0
#define GAME 1
int main(int argc, char *argv[]){		
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	int mode = GAME;
		
	if(argc == 3){load_settings(argv[1]); if(check(argv[2], "EDITOR")){mode = EDITOR;}}
	else if (argc == 2) {load_settings(argv[1]);}
	else if(argc == 1){load_settings("engine.ini");}
	else{LOG(ABORT, "Incorrect number of arguments. Provide only one argument as the path to an engine.ini, otherwise provide no arguments for default relative path <engine.ini>");}
	initalize_pran();
	log_init(STR(LOG_PATH));
	
	init_window(STR(WINDOW_NAME));
	update_window();
	
	switch(mode){
		case EDITOR:{

			struct Editor *editor = init_editor();
			while(!WindowShouldClose()){
				update_window();
				update_editor(editor);
				draw_editor(editor);
			}

			free_editor(editor);
			break;
		}
		case GAME:{
			struct Scene *scene = init_scene();

			while(!WindowShouldClose()){
				update_window();
				update_scene(scene);
				draw_scene(scene);
			}
			free_scene(scene);

			break;	
		}
	}	



	CloseWindow();
	free_settings();
	free_input();
	return 0;
}
