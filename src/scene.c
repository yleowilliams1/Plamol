#include <stdlib.h>
#include "entity.h"
#include "sprite.h"
#include "map.h"
#include "draw.h"
#include "save.h"
#include "settings.h"
#include "scene.h"
#include "util/util.h"
#include "interact.h"
#include "camera.h"
#include "movement.h"
struct Scene *init_scene(){
	struct Scene *s = XCALLOC(1, sizeof(struct Scene));
	return s;
}
void update_scene(struct Scene *scene){
}
void draw_scene(struct Scene *scene){
}
void free_scene(struct Scene *scene){
	if(!scene){return;}
}
