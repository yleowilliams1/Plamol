#include <raylib.h>

#include "settings.h"
#include "window.h"
#include "log.h"
#include "state.h"

int main(){	
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	load_settings();
	log_init(STR(LOG_PATH));
	init_window(STR(WINDOW_NAME));
	apply_settings();
	update_window();

	struct State *state = create_state();
	
	while(!WindowShouldClose()){
		update_window();
		update_state(state, GetFrameTime());
		draw_state(state);
	}	

	CloseWindow();
	free_state(state);
	free_settings();
	return 0;
}
