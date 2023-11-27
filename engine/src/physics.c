#ifdef RDE_PHYSICS_MODULE

rde_vec_3F rde_inner_physics_rotate_box_point(rde_vec_3F _point, rde_vec_3F _rotation, rde_vec_3F _bounds) {
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _point.x, _point.y, _point.z });
	if(_rotation.x != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.x), (vec3){ 1.0f, 0.0f, 0.0f });
	}

	if(_rotation.y != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.y), (vec3){ 0.0f, 1.0f, 0.0f });
	}

	if(_rotation.z != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	}

	vec4 _rotated_point;
	glm_mat4_mulv(_transformation_matrix, (vec4) { _bounds.x, _bounds.y, _bounds.z, 1.0f }, _rotated_point); 

	return (rde_vec_3F) { _rotated_point[0], _rotated_point[1], _rotated_point[2] };
}

void rde_physics_draw_debug_box(rde_transform* _transform, rde_jolt_box_shape_bounds* _box_bounds) {

	// Front and back faces
	for(int _i = 0; _i < 2; _i++) {
		float _depth = _i == 0 ? 1.0f : -1.0f;

		rde_vec_3F _p_0_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_0_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_0_0, _p_0_1, RDE_COLOR_YELLOW, 1, NULL);


		rde_vec_3F _p_1_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_1_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_1_0, _p_1_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_2_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_2_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_2_0, _p_2_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_3_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_3_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_3_0, _p_3_1, RDE_COLOR_YELLOW, 1, NULL);
	}
	


	// Top and bottom faces
	for(int _i = 0; _i < 2; _i++) {
		float _height = _i == 0 ? 1.0f : -1.0f;

		rde_vec_3F _p_0_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height * _height, -_box_bounds->depth });
		rde_vec_3F _p_0_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height * _height,  _box_bounds->depth });
		rde_rendering_3d_draw_line(_p_0_0, _p_0_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_1_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height * _height, -_box_bounds->depth });
		rde_vec_3F _p_1_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height * _height, _box_bounds->depth });
		rde_rendering_3d_draw_line(_p_1_0, _p_1_1, RDE_COLOR_YELLOW, 1, NULL);
	}
}

void rde_physics_draw_debug_shapes_specific(rde_jolt_body* _body, rde_jolt_shape* _shape, rde_transform* _transform) {
	UNUSED(_body)

	RDE_JOLT_SHAPE_ _shape_type = rde_jolt_shape_get_type(_shape);
	switch(_shape_type) {
		case RDE_JOLT_SHAPE_BOX: {
			rde_jolt_box_shape_bounds _box_bounds;
			rde_jolt_shape_get_bounds(_shape, RDE_JOLT_SHAPE_BOX, (void*)&_box_bounds);
			rde_physics_draw_debug_box(_transform, &_box_bounds);
		} break;

		case RDE_JOLT_SHAPE_SPHERE: {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "Rendering Debug Sphere is not implemented yet");
		} break;
	}
}

void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera) {
	rde_render_3d(_window, _camera, false, {
		rde_jolt_iterate_over_bodies(rde_physics_draw_debug_shapes_specific);
	})
}

#endif