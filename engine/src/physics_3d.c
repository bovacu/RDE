#if defined(RDE_PHYSICS_3D_MODULE) || defined(RDE_PHYSICS_2D_MODULE)
void rde_inner_engine_on_fixed_update(float _fixed_dt) {
	rde_physics_3d_run(_fixed_dt);
}
#endif

#ifdef RDE_PHYSICS_3D_MODULE

//--------------------------------------------------------------------------------------------------
// BPLayerInterface
//--------------------------------------------------------------------------------------------------
static uint32_t BPLayerInterface_GetNumBroadPhaseLayers(const void* _in_self) {
	(void)_in_self;
    return NUM_BP_LAYERS;
}

#ifdef _MSC_VER
static const JPC_BroadPhaseLayer* BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_BroadPhaseLayer* _out_layer, JPC_ObjectLayer _in_layer) {
#else
static JPC_BroadPhaseLayer BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_ObjectLayer _in_layer) {
#endif
    assert(_in_layer < NUM_BP_LAYERS);
    const BPLayerInterfaceImpl* _self = (BPLayerInterfaceImpl*)_in_self;
#ifdef _MSC_VER
    *_out_layer = _self->object_to_broad_phase[_in_layer];
    return _out_layer;
#else
    return _self->object_to_broad_phase[_in_layer];
#endif
}

static BPLayerInterfaceImpl BPLayerInterface_Init(void) {
    static const JPC_BroadPhaseLayerInterfaceVTable _vtable = {
        .GetNumBroadPhaseLayers = BPLayerInterface_GetNumBroadPhaseLayers,
        .GetBroadPhaseLayer     = BPLayerInterface_GetBroadPhaseLayer,
    };
    BPLayerInterfaceImpl _impl = {
        .vtable = &_vtable,
    };
    _impl.object_to_broad_phase[OBJ_LAYER_NON_MOVING] = BP_LAYER_NON_MOVING;
    _impl.object_to_broad_phase[OBJ_LAYER_MOVING]     = BP_LAYER_MOVING;

    return _impl;
}



//--------------------------------------------------------------------------------------------------
// MyContactListener
//--------------------------------------------------------------------------------------------------
static JPC_ValidateResult MyContactListener_OnContactValidate(void* _in_self,
                                    const JPC_Body* _in_body1,
                                    const JPC_Body* _in_body2,
                                    const JPC_Real _in_base_offset[3],
                                    const JPC_CollideShapeResult* _in_collision_result) {
	(void)_in_self;
	(void)_in_body1;
	(void)_in_body2;
	(void)_in_base_offset;
	(void)_in_collision_result;
    return JPC_VALIDATE_RESULT_ACCEPT_ALL_CONTACTS;
}

static void MyContactListener_OnContactAdded(void* _in_self,
                                 const JPC_Body* _in_body1,
                                 const JPC_Body* _in_body2,
                                 const JPC_ContactManifold* _in_manifold,
                                 JPC_ContactSettings* _io_settings) {
	(void)_in_self;
	(void)_in_body1;
	(void)_in_body2;
	(void)_in_manifold;
	(void)_io_settings;
}

static void MyContactListener_OnContactPersisted(void* _in_self,
                                     const JPC_Body* _in_body1,
                                     const JPC_Body* _in_body2,
                                     const JPC_ContactManifold* _in_manifold,
                                     JPC_ContactSettings* _io_settings) {
	(void)_in_self;
	(void)_in_body1;
	(void)_in_body2;
	(void)_in_manifold;
	(void)_io_settings;
}

static void MyContactListener_OnContactRemoved(void* _in_self, const JPC_SubShapeIDPair* _in_sub_shape_pair) {
	(void)_in_self;
	(void)_in_sub_shape_pair;
}

static MyContactListener MyContactListener_Init(void) {
    static const JPC_ContactListenerVTable _vtable = {
        .OnContactValidate  = MyContactListener_OnContactValidate,
        .OnContactAdded     = MyContactListener_OnContactAdded,
        .OnContactPersisted = MyContactListener_OnContactPersisted,
        .OnContactRemoved   = MyContactListener_OnContactRemoved,
    };
    MyContactListener _impl = {
        .vtable = &_vtable,
    };
    return _impl;
}

//--------------------------------------------------------------------------------------------------
// MyActivationListener
//--------------------------------------------------------------------------------------------------
static void MyActivationListener_OnBodyActivated(void* _in_self, const JPC_BodyID* _in_body_id, uint64_t _in_user_data) {
	(void)_in_self;
	(void)_in_body_id;
	(void)_in_user_data;
}

static void MyActivationListener_OnBodyDeactivated(void* _in_self, const JPC_BodyID* _in_body_id, uint64_t _in_user_data) {
	(void)_in_self;
	(void)_in_body_id;
	(void)_in_user_data;
}

static MyActivationListener MyActivationListener_Init(void) {
    static const JPC_BodyActivationListenerVTable _vtable = {
        .OnBodyActivated   = MyActivationListener_OnBodyActivated,
        .OnBodyDeactivated = MyActivationListener_OnBodyDeactivated,
    };

    MyActivationListener _impl = {
        .vtable = &_vtable,
    };
    return _impl;
}

//--------------------------------------------------------------------------------------------------
// MyObjectFilter
//--------------------------------------------------------------------------------------------------
static bool MyObjectFilter_ShouldCollide(const void* _in_self, JPC_ObjectLayer _in_object1, JPC_ObjectLayer _in_object2) {
	(void)_in_self;
    switch (_in_object1) {
        case OBJ_LAYER_NON_MOVING:
            return _in_object2 == OBJ_LAYER_MOVING;
        case OBJ_LAYER_MOVING:
            return true;
        default:
            assert(false);
            return false;
    }
}

static MyObjectFilter MyObjectFilter_Init(void) {
    static const JPC_ObjectLayerPairFilterVTable _vtable = {
        .ShouldCollide = MyObjectFilter_ShouldCollide,
    };

    MyObjectFilter _impl = {
        .vtable = &_vtable,
    };

    return _impl;
}

//--------------------------------------------------------------------------------------------------
// MyBroadPhaseFilter
//--------------------------------------------------------------------------------------------------
static bool MyBroadPhaseFilter_ShouldCollide(const void* _in_self, JPC_ObjectLayer _in_layer1, JPC_BroadPhaseLayer _in_layer2) {
	(void)_in_self;
    switch (_in_layer1) {
        case OBJ_LAYER_NON_MOVING:
            return _in_layer2 == BP_LAYER_MOVING;
        case OBJ_LAYER_MOVING:
            return true;
        default:
            assert(false);
            return false;
    }
}

static MyBroadPhaseFilter MyBroadPhaseFilter_Init(void) {
    static const JPC_ObjectVsBroadPhaseLayerFilterVTable _vtable = {
        .ShouldCollide = MyBroadPhaseFilter_ShouldCollide,
    };

    MyBroadPhaseFilter _impl = {
        .vtable = &_vtable,
    };

    return _impl;
}



// ==============================================================================
// =									API										=
// ==============================================================================

void rde_physics_3d_init(rde_physics_3d_config _physics_config) {
	JPC_RegisterDefaultAllocator();
	JPC_CreateFactory();
	JPC_RegisterTypes();
	ENGINE.physics_3d.temp_allocator = JPC_TempAllocator_Create(_physics_config.temp_allocator_bytes);
	ENGINE.physics_3d.job_system = JPC_JobSystem_Create(_physics_config.max_amout_of_allowed_jobs, 
	                                                    _physics_config.max_amount_of_physics_barriers, 
	                                                    _physics_config.max_amount_of_threads);

    ENGINE.physics_3d.broad_phase_layer = BPLayerInterface_Init();
    ENGINE.physics_3d.object_vs_broad_phase_layer = MyBroadPhaseFilter_Init();
    ENGINE.physics_3d.object_vs_object_layer = MyObjectFilter_Init();

	ENGINE.physics_3d.physics_system = JPC_PhysicsSystem_Create(_physics_config.max_amount_of_bodies,
	                                                            _physics_config.max_amount_of_mutexes,
	                                                            _physics_config.max_amount_of_bodies,
	                                                            _physics_config.max_amount_of_contact_constraints,
	                                                            &ENGINE.physics_3d.broad_phase_layer,
	                                                            &ENGINE.physics_3d.object_vs_broad_phase_layer,
	                                                            &ENGINE.physics_3d.object_vs_object_layer);

	ENGINE.physics_3d.body_activation_listener = MyActivationListener_Init();
    JPC_PhysicsSystem_SetBodyActivationListener(ENGINE.physics_3d.physics_system, &ENGINE.physics_3d.body_activation_listener);

	ENGINE.physics_3d.contact_listener = MyContactListener_Init();
	JPC_PhysicsSystem_SetContactListener(ENGINE.physics_3d.physics_system, &ENGINE.physics_3d.contact_listener);

	ENGINE.physics_3d.body_interface = JPC_PhysicsSystem_GetBodyInterface(ENGINE.physics_3d.physics_system);

	//JPC_PhysicsSystem_OptimizeBroadPhase(ENGINE.physics_3d.physics_system);

	ENGINE.physics_3d.last_body_added_on_array_index = -1;
}

rde_physics_3d_body* rde_physics_3d_body_load(RDE_PHYSICS_3D_SHAPE_TYPE_ _shape_type, rde_transform* _transform, const void* _settings) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on Physics 3D Body loading");
	rde_physics_3d_body* _rde_body = NULL;

	for(size_t _i = 0; _i < ENGINE.init_info.physics_3d_config.max_amount_of_bodies; _i++) {
		if(ENGINE.physics_3d.bodies[_i].body == NULL) {
			_rde_body = &ENGINE.physics_3d.bodies[_i];
			_rde_body->index_on_array = _i;
			if((int)_i > ENGINE.physics_3d.last_body_added_on_array_index) {
				ENGINE.physics_3d.last_body_added_on_array_index = _i;
			}
			break;
		}
	}

	rde_critical_error(_rde_body == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "Physics Bodies 3D");

	rde_physics_3d_common_shape_settings _rde_common_settings;
	switch(_shape_type) {
		case RDE_PHYSICS_3D_SHAPE_TYPE_BOX: {
			rde_physics_3d_shape_box_settings* _rde_settings = (rde_physics_3d_shape_box_settings*)_settings;
			_rde_common_settings = _rde_settings->common;

			_rde_body->shape_info.shape_settings = JPC_BoxShapeSettings_Create((float[]){ 
				_rde_settings->width,
				_rde_settings->height, 
				_rde_settings->depth
			});
		} break;
	
		case RDE_PHYSICS_3D_SHAPE_TYPE_SPHERE: {
			rde_physics_3d_shape_sphere_settings* _rde_settings = (rde_physics_3d_shape_sphere_settings*)_settings;
			_rde_common_settings = _rde_settings->common;

			_rde_body->shape_info.shape_settings = JPC_SphereShapeSettings_Create(_rde_settings->radius);
		} break;

		default: {
			rde_critical_error(true, "Shape type '%d' is unknown or not implemented yet", _shape_type);
		} break;
	}

	_rde_body->shape_info.shape = JPC_ShapeSettings_CreateShape((JPC_ShapeSettings*)_rde_body->shape_info.shape_settings);
	_rde_body->transform = _transform;
	_rde_body->active = _rde_common_settings.active;

	rde_critical_error(_rde_body->shape_info.shape == NULL, "Create Physics 3D, error creating shape. Wrong '_settings' set (wrong type for shape '%d'). \n", _shape_type);

	// TODO: in the future substitute for a propper end of layer checking
	rde_critical_error(_rde_common_settings.layer < 0 || _rde_common_settings.layer >= NUM_BP_LAYERS, "Create Physics 3D Body error, layer not valid, should be between [0, %d]. \n", NUM_BP_LAYERS - 1);
	
	JPC_Real _quat[4];
	JPC_Real _euler[3] = { rde_math_degrees_to_radians(_transform->rotation.x), rde_math_degrees_to_radians(_transform->rotation.z), rde_math_degrees_to_radians(_transform->rotation.y) };
	JPC_Euler_ToQuat(_euler, _quat);
	JPC_BodyCreationSettings_Set(
		&_rde_body->body_settings,
		_rde_body->shape_info.shape,
		(JPC_Real[]){ _rde_body->transform->position.x, 
					  _rde_body->transform->position.y, 
					  _rde_body->transform->position.z 
		},
		(JPC_Real[]) {
			_quat[0],
			_quat[1],
			_quat[2],
			_quat[3],
		},
		(JPC_MotionType)_rde_common_settings.body_type,
		_rde_common_settings.layer);

	_rde_body->body = JPC_BodyInterface_CreateBody(ENGINE.physics_3d.body_interface, &_rde_body->body_settings);
	_rde_body->body_id = JPC_Body_GetID(_rde_body->body);
	JPC_BodyInterface_AddBody(ENGINE.physics_3d.body_interface, _rde_body->body_id, _rde_common_settings.active ? JPC_ACTIVATION_ACTIVATE : JPC_ACTIVATION_DONT_ACTIVATE);
	return _rde_body;
}

void rde_physics_3d_body_unload(rde_physics_3d_body* _body) {
	JPC_BodyInterface_RemoveBody(ENGINE.physics_3d.body_interface, _body->body_id);
	JPC_BodyInterface_DestroyBody(ENGINE.physics_3d.body_interface, _body->body_id);
	JPC_ShapeSettings_Release((JPC_ShapeSettings*)_body->shape_info.shape_settings);
	JPC_Shape_Release(_body->shape_info.shape);

	if(_body->index_on_array == ENGINE.physics_3d.last_body_added_on_array_index) {
		ENGINE.physics_3d.last_body_added_on_array_index = -1;
		for(size_t _i = 0; _i < ENGINE.init_info.physics_3d_config.max_amount_of_bodies; _i++) {
			if(ENGINE.physics_3d.bodies[_i].body != NULL && &ENGINE.physics_3d.bodies[_i] != _body) {
				ENGINE.physics_3d.last_body_added_on_array_index = _i;
				break;
			}
		}
	}

	*_body = rde_struct_create_physics_3d_body();
}


void rde_physics_3d_run(float _fixed_dt) {
	JPC_PhysicsUpdateError _error = JPC_PhysicsSystem_Update(ENGINE.physics_3d.physics_system, _fixed_dt, 1, 1, 
	                         ENGINE.physics_3d.temp_allocator,
	                         ENGINE.physics_3d.job_system);
	rde_critical_error(_error != JPC_PHYSICS_UPDATE_NO_ERROR, "Physics simulation error, code: %d", _error);

	// TODO: This is not optimal at all, but there is no other way now to update associated rde_transform with inner JoltC Body positions.
	for(int _i = 0; _i < ENGINE.physics_3d.last_body_added_on_array_index; _i++) {
		rde_physics_3d_body* _body = &ENGINE.physics_3d.bodies[_i];
		if(_body->body == NULL || _body->body_settings.motion_type == JPC_MOTION_TYPE_STATIC) {
			continue;
		}

		JPC_Real _position[3];
		JPC_Body_GetPosition(_body->body, _position);

		JPC_Real _quat[4] = { _body->body->rotation[0], _body->body->rotation[1], _body->body->rotation[2], _body->body->rotation[3] };
		rde_log_level(RDE_LOG_LEVEL_INFO, "RQ: (%lf, %lf, %lf, %lf)", _quat[0], _quat[1], _quat[2], _quat[3]);
		JPC_Real _euler[3];
		JPC_Quat_ToEuler(_quat, _euler);
		rde_log_level(RDE_LOG_LEVEL_INFO, "RE Rads: (%f, %f, %f)", _euler[0], _euler[1], _euler[2]);
//
		//rde_log_level(RDE_LOG_LEVEL_INFO, "RQ: (%lf, %lf, %lf, %lf)", _quat[0], _quat[1], _quat[2], _quat[3]);
//
		_body->transform->position = (rde_vec_3F) { _position[0], _position[1], _position[2] };
		_body->transform->rotation = (rde_vec_3F) { rde_math_radians_to_degrees(_euler[2]), rde_math_radians_to_degrees(_euler[1]), rde_math_radians_to_degrees(_euler[0]) };
		//rde_log_level(RDE_LOG_LEVEL_INFO, "RE Degs: (%f, %f, %f)", _body->transform->rotation.x, _body->transform->rotation.y, _body->transform->rotation.z);
		
	}
}

void rde_physics_3d_body_enable(rde_physics_3d_body* _body, bool _enable_body) {
	rde_critical_error(_body == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Physcis 3D Body on enable body");
	_body->active = _enable_body;
	if(_enable_body) {
		JPC_BodyInterface_ActivateBody(ENGINE.physics_3d.body_interface, _body->body_id);
	} else {
		JPC_BodyInterface_DeactivateBody(ENGINE.physics_3d.body_interface, _body->body_id);
	}
}

void rde_physics_3d_body_set_transform(rde_physics_3d_body* _body, rde_transform _transform) {
	rde_critical_error(_body == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Physcis 3D Body on set transform for body");
	JPC_BodyInterface_SetPosition(ENGINE.physics_3d.body_interface, 
	                              _body->body_id, 
	                              (JPC_Real [3]) {
								  	_transform.position.x,
								  	_transform.position.y,
								  	_transform.position.z
								  },
								  _body->active ? JPC_ACTIVATION_ACTIVATE : JPC_ACTIVATION_DONT_ACTIVATE
	);

	JPC_Real _quat[4];
	JPC_Real _euler[3] = { rde_math_degrees_to_radians(_transform.rotation.x), rde_math_degrees_to_radians(_transform.rotation.z), rde_math_degrees_to_radians(_transform.rotation.y) };
	JPC_Euler_ToQuat(_euler, _quat);
	JPC_BodyInterface_SetRotation(ENGINE.physics_3d.body_interface, 
	                              _body->body_id, 
	                              (JPC_Real[4]) { _quat[0], _quat[2], _quat[1], _quat[3] },
								  _body->active ? JPC_ACTIVATION_ACTIVATE : JPC_ACTIVATION_DONT_ACTIVATE
	);
}

void rde_physics_3d_destroy() {

	for(size_t _i = 0; _i < ENGINE.init_info.physics_3d_config.max_amount_of_bodies; _i++) {
		rde_physics_3d_body* _body = &ENGINE.physics_3d.bodies[_i];
		if(_body->body != NULL) {
			rde_physics_3d_body_unload(_body);
		}
	}
	
	free(ENGINE.physics_3d.bodies);
	JPC_DestroyFactory();
	JPC_TempAllocator_Destroy(ENGINE.physics_3d.temp_allocator);
	JPC_JobSystem_Destroy(ENGINE.physics_3d.job_system);
}

#endif