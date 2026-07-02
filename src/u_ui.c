#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "u_ui.h"
#define BUTTON_ROUNDNESS 15.0f
#define PANEL_ROUNDNESS 5.0f
struct UI_Element ui[UI_LENGTH] = {0};
void draw_text(int indx);
int grab_valid_indx(){
	for(int i = 0; i < UI_LENGTH; i++){
		if(!ui[i].valid){
			ui[i] = (struct UI_Element){0}; 
			ui[i].valid = true; 
			return i;
		}	

	}

	printf("UI: Can't find index. returning 0\n");
	ui[0] = (struct UI_Element){0};
	ui[0].valid = true;
	return 0;
}

void clear_ui_state(){
	for(int i = 0; i < UI_LENGTH; i++){
		ui[i] = (struct UI_Element){0};	
	}
}

int ui_element(enum TextType text_pos, 
Vector2 pos, Color clr, Rectangle collision, char *text, int padding, void (*button)(void)){
	int indx = grab_valid_indx();
	ui[indx].col = collision;
	ui[indx].pos = pos;
	ui[indx].colr = clr;
	ui[indx].text = text;
	ui[indx].padding = padding;
	ui[indx].text_pos = text_pos;
	ui[indx].button = button;
	return indx;	
}

void draw_ui(){
	for(int i = 0; i < UI_LENGTH; i++){
		if(!ui[i].valid)continue;
		Color colr = ui[i].colr;
		Rectangle coll = ui[i].col;
		
		if(ui[i].button){
			DrawRectangleRounded(coll, BUTTON_ROUNDNESS, 1, colr); 
			DrawRectangleRoundedLines(coll, BUTTON_ROUNDNESS, 1, BLACK);
			if(ui[i].intersect){DrawRectangleRounded(coll, BUTTON_ROUNDNESS, 1, Fade(WHITE, 0.5f));}
			continue;
		}
		
		DrawRectangleRounded(coll, PANEL_ROUNDNESS, 1, colr);
		DrawRectangleRoundedLines(coll, PANEL_ROUNDNESS, 1, BLACK);		
		
		if(ui[i].text){
			draw_text(i);			
			continue;
		}
	}	
}

void draw_text(int indx){


}

