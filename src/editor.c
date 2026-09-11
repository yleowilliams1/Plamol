#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <raylib.h>
#include "../imgui_backend/imgui_impl_raylib.h"
#include "../imgui_backend/rlcimgui.h"
#include "editor.h"
#include "util/util.h"
#include "draw.h"
#include "entity.h"
#include "map.h"
#include "sprite.h"
#include "camera.h"

static void update_ui(struct Editor *editor);
void draw_editor_ui(struct Editor *editor);

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
	ImGui_ImplRaylib_BuildFontAtlas();   // must be called after adding fonts
}
void update_editor(struct Editor *editor){
	update_ui(editor);
}
void draw_editor(struct Editor *editor){
	BeginDrawing();
	ClearBackground(RAYWHITE);

	// ... draw your game/scene/editor viewport with raylib as usual ...

	ImGui_ImplRaylib_RenderDrawData(igGetDrawData());  // draw ImGui last, on top
	EndDrawing();
}
void free_editor(struct Editor *editor){
	ImGui_ImplRaylib_Shutdown();
	igDestroyContext(NULL);
}
static void update_ui(struct Editor *editor){
	ImGui_ImplRaylib_ProcessEvents();
	ImGui_ImplRaylib_NewFrame();
	igNewFrame();

	// --- your ImGui widgets go here ---
	igBegin("Inspector", NULL, 0);
	igText("Hello from the editor");
	igEnd();
	// -----------------------------------

	igRender();


}
