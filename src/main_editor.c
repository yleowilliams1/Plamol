#include <sys/stat.h>
#include <raylib.h>
#include <unistd.h>
#include <string.h>
#include "util/util.h"
#include "settings.h"
#include "input.h"
#include "editor.h"
int main(int argc, char *argv[]){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	if(argc == 2){load_settings(argv[1]);}
	else if(argc == 1){load_settings("engine.ini");}
	else{LOG(ABORT, "Incorrect number of arguments. Provide only one argument as the path to an engine.ini, otherwise provide no arguments for default relative path <engine.ini>");}
	initalize_pran();
	log_init(STR(LOG_PATH));
	
	init_window(STR(WINDOW_NAME));
	update_window();
	
	struct Editor *editor = init_editor();

	while(!WindowShouldClose()){
		update_window();
		update_editor(editor);
		draw_editor(editor);
	}

	CloseWindow();
	free_settings();
	free_input();
	free_editor(editor);
	return 0;
}
