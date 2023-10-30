#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#define RDE_RENDERING_MODULE
#define RDE_RENDERING_ERROR_MODULE
#define RDE_RENDERING_AUDIO_MODULE
#define RDE_RENDERING_OBJ_MODULE
#define RDE_FILE_SYSTEM_MODULE
#include "rde.h"
#include "test_utils.c"

#include "test_2d_draw.c"
#include "test_3d_draw.c"
#include "test_audio.c"
#include "test_engine.c"
#include "test_events.c"
#include "test_file_utils.c"
#include "test_font.c"
#include "test_math.c"
#include "test_obj.c"
#include "test_physics.c"
#include "test_string_utils.c"
#include "test_texture.c"

int main() {
	init_test_config();
	RUN_ALL_TESTS(test_string_utils,
	              test_file_utils,
	              test_math,
	              test_texture,
	              test_font,
	              test_engine,
	              test_events,
	              test_obj,
	              test_2d_draw,
	              test_3d_draw,
	              test_physics,
	              test_audio)
}