#ifdef RDE_PHYSICS_3D_MODULE

void rde_physics_3d_draw_debug_box(rde_transform* _transform, rde_jolt_box_shape_bounds* _box_bounds) {

	// Front and back faces
	for(int _i = 0; _i < 2; _i++) {
		float _depth = _i == 0 ? 1.0f : -1.0f;
		rde_rendering_3d_draw_line((rde_vec_3F) {
									_transform->position.x - _box_bounds->width,
									_transform->position.y - _box_bounds->height,
									_transform->position.z + _box_bounds->depth * _depth
								},
								(rde_vec_3F) {
									_transform->position.x + _box_bounds->width,
									_transform->position.y - _box_bounds->height,
									_transform->position.z + _box_bounds->depth * _depth
								},
								RDE_COLOR_YELLOW,
								1,
								NULL);

		rde_rendering_3d_draw_line((rde_vec_3F) {
								_transform->position.x - _box_bounds->width,
								_transform->position.y + _box_bounds->height,
								_transform->position.z + _box_bounds->depth * _depth
								},
								(rde_vec_3F) {
									_transform->position.x + _box_bounds->width,
									_transform->position.y + _box_bounds->height,
									_transform->position.z + _box_bounds->depth * _depth
								},
								RDE_COLOR_YELLOW,
								1,
								NULL);
	
		rde_rendering_3d_draw_line((rde_vec_3F) {
								_transform->position.x - _box_bounds->width,
								_transform->position.y - _box_bounds->height,
								_transform->position.z + _box_bounds->depth * _depth
								},
								(rde_vec_3F) {
									_transform->position.x - _box_bounds->width,
									_transform->position.y + _box_bounds->height,
									_transform->position.z + _box_bounds->depth * _depth
								},
								RDE_COLOR_YELLOW,
								1,
								NULL);
	
		rde_rendering_3d_draw_line((rde_vec_3F) {
								_transform->position.x + _box_bounds->width,
								_transform->position.y - _box_bounds->height,
								_transform->position.z + _box_bounds->depth * _depth
								},
								(rde_vec_3F) {
									_transform->position.x + _box_bounds->width,
									_transform->position.y + _box_bounds->height,
									_transform->position.z + _box_bounds->depth * _depth
								},
								RDE_COLOR_YELLOW,
								1,
								NULL);
	}
	


	// Top and bottom faces
	for(int _i = 0; _i < 2; _i++) {
		float _height = _i == 0 ? 1.0f : -1.0f;
		rde_rendering_3d_draw_line((rde_vec_3F) {
										_transform->position.x - _box_bounds->width,
										_transform->position.y + _box_bounds->height * _height,
										_transform->position.z - _box_bounds->depth
									},
									(rde_vec_3F) {
										_transform->position.x - _box_bounds->width,
										_transform->position.y + _box_bounds->height * _height,
										_transform->position.z + _box_bounds->depth
									},
									RDE_COLOR_YELLOW,
									1,
									NULL);
	
		rde_rendering_3d_draw_line((rde_vec_3F) {
								_transform->position.x + _box_bounds->width,
								_transform->position.y + _box_bounds->height * _height,
								_transform->position.z - _box_bounds->depth
								},
								(rde_vec_3F) {
									_transform->position.x + _box_bounds->width,
									_transform->position.y + _box_bounds->height * _height,
									_transform->position.z + _box_bounds->depth
								},
								RDE_COLOR_YELLOW,
								1,
								NULL);
	}
}

void rde_physics_3d_draw_debug_shapes_specific(rde_jolt_body* _body, rde_jolt_shape* _shape, rde_transform* _transform) {
	UNUSED(_body)

	RDE_JOLT_SHAPE_ _shape_type = rde_jolt_shape_get_type(_shape);
	switch(_shape_type) {
		case RDE_JOLT_SHAPE_BOX: {
			rde_jolt_box_shape_bounds _box_bounds;
			rde_jolt_shape_get_bounds(_shape, RDE_JOLT_SHAPE_BOX, (void*)&_box_bounds);
			rde_physics_3d_draw_debug_box(_transform, &_box_bounds);
		} break;

		case RDE_JOLT_SHAPE_SPHERE: {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "Rendering Debug Sphere is not implemented yet");
		} break;
	}
}

void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera) {
	rde_render_3d(_window, _camera, false, {
		rde_jolt_iterate_over_bodies(rde_physics_3d_draw_debug_shapes_specific);
	})
}

#endif