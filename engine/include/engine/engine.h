#ifndef RDE_ENGINE_H
#define RDE_ENGINE_H

#define DEFAULT_IMPLEMENTATION

#include "core.h"
#include "enums/generic_enums.h"

RDE_FUNC_ENGINE rde_engine*			rde_engine_create_engine(int _argc, char** _argv);

RDE_FUNC 		RDE_PLATFORM_TYPE_ 	rde_engine_get_platform(rde_engine* _engine);

RDE_FUNC_ND 	float 				rde_engine_get_fixed_delta(rde_engine* _engine);
RDE_FUNC 		void 				rde_engine_set_fixed_delta(rde_engine* _engine, float _fixed_dt);

RDE_FUNC		void 				rde_engine_set_current_scene(rde_engine* _engine, rde_scene* _scene);
RDE_FUNC		void				rde_engine_on_run(rde_engine* _engine);

RDE_FUNC		void				rde_engine_init_imgui_layer(rde_engine* _engine);
RDE_FUNC		void				rde_engine_end_imgui_layer(rde_engine* _engine);

RDE_FUNC_ND		rde_window*			rde_engine_get_window(rde_engine* _engine, int _index);

RDE_FUNC_ND		bool				rde_engine_is_running(rde_engine* _engine);
RDE_FUNC		void				rde_engine_set_running(rde_engine* _engine, bool _running);

RDE_FUNC_ND		rde_vec2I			rde_engine_get_display_size(rde_engine* _engine);

RDE_FUNC		void				rde_engine_destroy_engine(rde_engine* _engine);

#endif