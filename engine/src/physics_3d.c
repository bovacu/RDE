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
    rde_log_level(RDE_LOG_LEVEL_INFO, "BPLayerInterface_GetNumBroadPhaseLayers()\n");
    return NUM_BP_LAYERS;
}

#ifdef _MSC_VER
static const JPC_BroadPhaseLayer* BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_BroadPhaseLayer* _out_layer, JPC_ObjectLayer _in_layer) {
#else
static JPC_BroadPhaseLayer BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_ObjectLayer _in_layer) {
#endif
    rde_log_level(RDE_LOG_LEVEL_INFO, "BPLayerInterface_GetBroadPhaseLayer()\n");
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
    const JPC_BodyID _body1_id = JPC_Body_GetID(_in_body1);
    const JPC_BodyID _body2_id = JPC_Body_GetID(_in_body2);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): First BodyID is (%d, %d), second BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(_body1_id), JPC_BodyID_GetIndex(_body1_id), JPC_BodyID_GetSequenceNumber(_body2_id), JPC_BodyID_GetIndex(_body2_id));
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): in_base_offset (%f, %f, %f)\n", _in_base_offset[0], _in_base_offset[1], _in_base_offset[2]);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): penetration_depth (%f)\n", _in_collision_result->penetration_depth);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): shape1_sub_shape_id (%d)\n", _in_collision_result->shape1_sub_shape_id);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): shape2_sub_shape_id (%d)\n", _in_collision_result->shape2_sub_shape_id);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactValidate(): body2_id (%d)\n", _in_collision_result->body2_id);
    return JPC_VALIDATE_RESULT_ACCEPT_ALL_CONTACTS;
}

static void MyContactListener_OnContactAdded(void* _in_self,
                                 const JPC_Body* _in_body1,
                                 const JPC_Body* _in_body2,
                                 const JPC_ContactManifold* _in_manifold,
                                 JPC_ContactSettings* _io_settings) {
	(void)_in_self;
	(void)_in_manifold;
	(void)_io_settings;
    const JPC_BodyID _body1_id = JPC_Body_GetID(_in_body1);
    const JPC_BodyID _body2_id = JPC_Body_GetID(_in_body2);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactAdded(): First BodyID is (%d, %d), second BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(_body1_id), JPC_BodyID_GetIndex(_body1_id), JPC_BodyID_GetSequenceNumber(_body2_id), JPC_BodyID_GetIndex(_body2_id));
}

static void MyContactListener_OnContactPersisted(void* _in_self,
                                     const JPC_Body* _in_body1,
                                     const JPC_Body* _in_body2,
                                     const JPC_ContactManifold* _in_manifold,
                                     JPC_ContactSettings* _io_settings) {
	(void)_in_self;
	(void)_in_manifold;
	(void)_io_settings;
    const JPC_BodyID _body1_id = JPC_Body_GetID(_in_body1);
    const JPC_BodyID _body2_id = JPC_Body_GetID(_in_body2);
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactPersisted(): First BodyID is (%d, %d), second BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(_body1_id), JPC_BodyID_GetIndex(_body1_id), JPC_BodyID_GetSequenceNumber(_body2_id), JPC_BodyID_GetIndex(_body2_id));
}

static void MyContactListener_OnContactRemoved(void* _in_self, const JPC_SubShapeIDPair* _in_sub_shape_pair) {
	(void)_in_self;
    const JPC_BodyID _body1_id = _in_sub_shape_pair->first.body_id;
    const JPC_BodyID _body2_id = _in_sub_shape_pair->second.body_id;
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnContactRemoved(): First BodyID is (%d, %d), second BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(_body1_id), JPC_BodyID_GetIndex(_body1_id), JPC_BodyID_GetSequenceNumber(_body2_id), JPC_BodyID_GetIndex(_body2_id));
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
	(void)_in_user_data;
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnBodyActivated(): BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(*_in_body_id), JPC_BodyID_GetIndex(*_in_body_id));
}

static void MyActivationListener_OnBodyDeactivated(void* _in_self, const JPC_BodyID* _in_body_id, uint64_t _in_user_data) {
	(void)_in_self;
	(void)_in_user_data;
    rde_log_level(RDE_LOG_LEVEL_INFO, "\tOnBodyDeactivated(): BodyID is (%d, %d)\n", JPC_BodyID_GetSequenceNumber(*_in_body_id), JPC_BodyID_GetIndex(*_in_body_id));
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

	JPC_PhysicsSystem_OptimizeBroadPhase(ENGINE.physics_3d.physics_system);
}

rde_physics_3d_body* rde_physics_3d_body_load(RDE_PHYSICS_3D_SHAPE_TYPE_ _shape_type, rde_transform* _transform, const void* _settings) {
	
	rde_physics_3d_body* _rde_body = NULL;

	for(size_t _i = 0; _i < ENGINE.init_info.physics_3d_config.max_amount_of_bodies; _i++) {
		if(ENGINE.physics_3d_bodies[_i].body == NULL) {
			_rde_body = &ENGINE.physics_3d_bodies[_i];
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
	_rde_body->transform = *_transform;

	rde_critical_error(_rde_body->shape_info.shape == NULL, "Create Physics 3D, error creating shape. Wrong '_settings' set (wrong type for shape '%d'). \n", _shape_type);

	// TODO: in the future substitute for a propper end of layer checking
	rde_critical_error(_rde_common_settings.layer < 0 || _rde_common_settings.layer >= NUM_BP_LAYERS, "Create Physics 3D Body error, layer not valid, should be between [0, %d]. \n", NUM_BP_LAYERS - 1);
	JPC_BodyCreationSettings_Set(
		&_rde_body->body_settings,
		_rde_body->shape_info.shape,
		(JPC_Real[]){ _rde_body->transform.position.x, 
					  _rde_body->transform.position.y, 
					  _rde_body->transform.position.z 
		},
		(float[]){ rde_math_degrees_to_radians(_rde_body->transform.rotation.x), 
				   rde_math_degrees_to_radians(_rde_body->transform.rotation.y), 
				   rde_math_degrees_to_radians(_rde_body->transform.rotation.z), 
				   1.0f },
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
	*_body = rde_struct_create_physics_3d_body();
}


void rde_physics_3d_run(float _fixed_dt) {
	JPC_PhysicsUpdateError _error = JPC_PhysicsSystem_Update(ENGINE.physics_3d.physics_system, _fixed_dt, 1, 1, 
	                         ENGINE.physics_3d.temp_allocator,
	                         ENGINE.physics_3d.job_system);
	rde_critical_error(_error != JPC_PHYSICS_UPDATE_NO_ERROR, "Physics simulation error, code: %d", _error);
}

void rde_physics_3d_destroy() {

	for(size_t _i = 0; _i < ENGINE.init_info.physics_3d_config.max_amount_of_bodies; _i++) {
		rde_physics_3d_body* _body = &ENGINE.physics_3d_bodies[_i];
		if(_body->body != NULL) {
			rde_physics_3d_body_unload(_body);
		}
	}
	
	free(ENGINE.physics_3d_bodies);
	JPC_DestroyFactory();
	JPC_TempAllocator_Destroy(ENGINE.physics_3d.temp_allocator);
	JPC_JobSystem_Destroy(ENGINE.physics_3d.job_system);
}

#endif