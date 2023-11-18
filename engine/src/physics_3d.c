#if defined(RDE_PHYSICS_3D_MODULE) || defined(RDE_PHYSICS_2D_MODULE)
void rde_inner_engine_on_fixed_update(float _fixed_dt) {
	rde_physics_3d_run(_fixed_dt);
}
#endif

#ifdef RDE_PHYSICS_3D_MODULE

//--------------------------------------------------------------------------------------------------
// BPLayerInterface
//--------------------------------------------------------------------------------------------------
//static uint32_t BPLayerInterface_GetNumBroadPhaseLayers(const void* _in_self) {
//	(void)_in_self;
//    return NUM_BP_LAYERS;
//}
//
//#ifdef _MSC_VER
//static const JPC_BroadPhaseLayer* BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_BroadPhaseLayer* _out_layer, JPC_ObjectLayer _in_layer) {
//#else
//static JPC_BroadPhaseLayer BPLayerInterface_GetBroadPhaseLayer(const void* _in_self, JPC_ObjectLayer _in_layer) {
//#endif
//    assert(_in_layer < NUM_BP_LAYERS);
//    const BPLayerInterfaceImpl* _self = (BPLayerInterfaceImpl*)_in_self;
//#ifdef _MSC_VER
//    *_out_layer = _self->object_to_broad_phase[_in_layer];
//    return _out_layer;
//#else
//    return _self->object_to_broad_phase[_in_layer];
//#endif
//}
//
//static BPLayerInterfaceImpl BPLayerInterface_Init(void) {
//    static const JPC_BroadPhaseLayerInterfaceVTable _vtable = {
//        .GetNumBroadPhaseLayers = BPLayerInterface_GetNumBroadPhaseLayers,
//        .GetBroadPhaseLayer     = BPLayerInterface_GetBroadPhaseLayer,
//    };
//    BPLayerInterfaceImpl _impl = {
//        .vtable = &_vtable,
//    };
//    _impl.object_to_broad_phase[OBJ_LAYER_NON_MOVING] = BP_LAYER_NON_MOVING;
//    _impl.object_to_broad_phase[OBJ_LAYER_MOVING]     = BP_LAYER_MOVING;
//
//    return _impl;
//}
//
//
//
//--------------------------------------------------------------------------------------------------
// MyContactListener
//--------------------------------------------------------------------------------------------------
//static JPC_ValidateResult MyContactListener_OnContactValidate(void* _in_self,
//                                    const JPC_Body* _in_body1,
//                                    const JPC_Body* _in_body2,
//                                    const JPC_Real _in_base_offset[3],
//                                    const JPC_CollideShapeResult* _in_collision_result) {
//	(void)_in_self;
//	(void)_in_body1;
//	(void)_in_body2;
//	(void)_in_base_offset;
//	(void)_in_collision_result;
//    return JPC_VALIDATE_RESULT_ACCEPT_ALL_CONTACTS;
//}
//
//static void MyContactListener_OnContactAdded(void* _in_self,
//                                 const JPC_Body* _in_body1,
//                                 const JPC_Body* _in_body2,
//                                 const JPC_ContactManifold* _in_manifold,
//                                 JPC_ContactSettings* _io_settings) {
//	(void)_in_self;
//	(void)_in_body1;
//	(void)_in_body2;
//	(void)_in_manifold;
//	(void)_io_settings;
//}
//
//static void MyContactListener_OnContactPersisted(void* _in_self,
//                                     const JPC_Body* _in_body1,
//                                     const JPC_Body* _in_body2,
//                                     const JPC_ContactManifold* _in_manifold,
//                                     JPC_ContactSettings* _io_settings) {
//	(void)_in_self;
//	(void)_in_body1;
//	(void)_in_body2;
//	(void)_in_manifold;
//	(void)_io_settings;
//}
//
//static void MyContactListener_OnContactRemoved(void* _in_self, const JPC_SubShapeIDPair* _in_sub_shape_pair) {
//	(void)_in_self;
//	(void)_in_sub_shape_pair;
//}
//
//static MyContactListener MyContactListener_Init(void) {
//    static const JPC_ContactListenerVTable _vtable = {
//        .OnContactValidate  = MyContactListener_OnContactValidate,
//        .OnContactAdded     = MyContactListener_OnContactAdded,
//        .OnContactPersisted = MyContactListener_OnContactPersisted,
//        .OnContactRemoved   = MyContactListener_OnContactRemoved,
//    };
//    MyContactListener _impl = {
//        .vtable = &_vtable,
//    };
//    return _impl;
//}
//
//--------------------------------------------------------------------------------------------------
// MyActivationListener
//--------------------------------------------------------------------------------------------------
//static void MyActivationListener_OnBodyActivated(void* _in_self, const JPC_BodyID* _in_body_id, uint64_t _in_user_data) {
//	(void)_in_self;
//	(void)_in_body_id;
//	(void)_in_user_data;
//}
//
//static void MyActivationListener_OnBodyDeactivated(void* _in_self, const JPC_BodyID* _in_body_id, uint64_t _in_user_data) {
//	(void)_in_self;
//	(void)_in_body_id;
//	(void)_in_user_data;
//}
//
//static MyActivationListener MyActivationListener_Init(void) {
//    static const JPC_BodyActivationListenerVTable _vtable = {
//        .OnBodyActivated   = MyActivationListener_OnBodyActivated,
//        .OnBodyDeactivated = MyActivationListener_OnBodyDeactivated,
//    };
//
//    MyActivationListener _impl = {
//        .vtable = &_vtable,
//    };
//    return _impl;
//}
//
//--------------------------------------------------------------------------------------------------
// MyObjectFilter
//--------------------------------------------------------------------------------------------------
//static bool MyObjectFilter_ShouldCollide(const void* _in_self, JPC_ObjectLayer _in_object1, JPC_ObjectLayer _in_object2) {
//	(void)_in_self;
//    switch (_in_object1) {
//        case OBJ_LAYER_NON_MOVING:
//            return _in_object2 == OBJ_LAYER_MOVING;
//        case OBJ_LAYER_MOVING:
//            return true;
//        default:
//            assert(false);
//            return false;
//    }
//}
//
//static MyObjectFilter MyObjectFilter_Init(void) {
//    static const JPC_ObjectLayerPairFilterVTable _vtable = {
//        .ShouldCollide = MyObjectFilter_ShouldCollide,
//    };
//
//    MyObjectFilter _impl = {
//        .vtable = &_vtable,
//    };
//
//    return _impl;
//}
//
//--------------------------------------------------------------------------------------------------
// MyBroadPhaseFilter
//--------------------------------------------------------------------------------------------------
//static bool MyBroadPhaseFilter_ShouldCollide(const void* _in_self, JPC_ObjectLayer _in_layer1, JPC_BroadPhaseLayer _in_layer2) {
//	(void)_in_self;
//    switch (_in_layer1) {
//        case OBJ_LAYER_NON_MOVING:
//            return _in_layer2 == BP_LAYER_MOVING;
//        case OBJ_LAYER_MOVING:
//            return true;
//        default:
//            assert(false);
//            return false;
//    }
//}
//
//static MyBroadPhaseFilter MyBroadPhaseFilter_Init(void) {
//    static const JPC_ObjectVsBroadPhaseLayerFilterVTable _vtable = {
//        .ShouldCollide = MyBroadPhaseFilter_ShouldCollide,
//    };
//
//    MyBroadPhaseFilter _impl = {
//        .vtable = &_vtable,
//    };
//
//    return _impl;
//}



// ==============================================================================
// =									API										=
// ==============================================================================

void rde_physics_3d_init(rde_physics_3d_config _physics_config) {
	(void)_physics_config;
	rde_jolt_init_config _init_config;
	_init_config.temo_allocator_size = 10 * 1024 * 1024;
	_init_config.max_physics_jobs = 2048;
	_init_config.max_physics_barriers = 8;
	_init_config.max_bodies = 65536;
	_init_config.max_body_mutexes = 0;
	_init_config.max_body_pairs = 65536;
	_init_config.max_concat_constraints = 1024;
	_init_config.max_threads = -1;
	rde_jolt_init(_init_config);
	rde_log_level(RDE_LOG_LEVEL_INFO, "RDE Jolt physics loaded");
}

rde_physics_3d_body* rde_physics_3d_body_load(RDE_PHYSICS_3D_SHAPE_TYPE_ _shape_type, rde_transform* _transform, const void* _settings) {
	(void)_shape_type;
	(void)_transform;
	(void)_settings;
	return NULL;
}

void rde_physics_3d_body_unload(rde_physics_3d_body* _body) {
	(void)_body;
}


void rde_physics_3d_run(float _fixed_dt) {
	(void)_fixed_dt;
}

void rde_physics_3d_body_enable(rde_physics_3d_body* _body, bool _enable_body) {
	(void)_body;
	(void)_enable_body;
}

void rde_physics_3d_body_set_transform(rde_physics_3d_body* _body, rde_transform _transform) {
	(void)_body;
	(void)_transform;
}

void rde_physics_3d_destroy() {
	rde_jolt_end();
}

#endif