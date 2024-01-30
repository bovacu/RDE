#define RDE_PHYSICS_MODULE
#include "rde.h"

#include <iostream>
#include <cassert>

//#define JPH_ENABLE_ASSERTS
#ifdef JPH_EXTERNAL_PROFILE
	#undef JPH_EXTERNAL_PROFILE
#endif
#ifdef JPH_PROFILE_ENABLED
	#undef JPH_PROFILE_ENABLED
#endif

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Math/Math.h>
#include <Jolt/Math/MathTypes.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/EPhysicsUpdateError.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

//#define JPH_DEBUG_RENDERER
//// Make sure the debug renderer symbols don't get imported or exported
//#define JPH_DEBUG_RENDERER_EXPORT
//#include <Jolt/Renderer/DebugRenderer.h>
//#undef JPH_DEBUG_RENDERER
//#undef JPH_DEBUG_RENDERER_EXPORT

namespace Layers {
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers {
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr unsigned int NUM_LAYERS(2);
};

class rde_broad_phase_layer_interface final : public JPH::BroadPhaseLayerInterface {
	private:
		JPH::BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];

	public:
		rde_broad_phase_layer_interface() {
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		}
	
		virtual unsigned int GetNumBroadPhaseLayers() const override {
			return BroadPhaseLayers::NUM_LAYERS;
		}
	
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
			JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
			return mObjectToBroadPhase[inLayer];
		}
	
	#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
			switch ((JPH::BroadPhaseLayer::Type)inLayer) {
				case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
				case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
				default:														JPH_ASSERT(false); return "INVALID";
			}
		}
	#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
};

class rde_object_vs_broad_phase_layer_filter : public JPH::ObjectVsBroadPhaseLayerFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
			switch (inLayer1) {
				case Layers::NON_MOVING:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layers::MOVING:
					return true;
				default:
					JPH_ASSERT(false);
					return false;
			}
		}
};

class rde_object_layer_pair_filter : public JPH::ObjectLayerPairFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
			switch (inObject1) {
				case Layers::NON_MOVING:
					return inObject2 == Layers::MOVING; // Non moving only collides with moving
				case Layers::MOVING:
					return true; // Moving collides with everything
				default:
					JPH_ASSERT(false);
					return false;
			}
		}
};

class rde_activation_listener : public JPH::BodyActivationListener {
	public:
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {  }
		
		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {	}
};

class rde_contact_listener : public JPH::ContactListener {
	public:
		virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override {
			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}
	
		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {  }
	
		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {  }
	
		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {  }
};

//class rde_debug_renderer final : public JPH::DebugRenderer {
//
//	public:
//		rde_debug_renderer() {
//			JPH::DebugRenderer::Initialize();
//		}
//
//		virtual ~rde_debug_renderer() override {
//			
//		}
//	
//		virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override {
//			
//		}
//
//		virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override {
//			
//		}
//		
//		virtual Batch CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) override {
//			return {};
//		}
//	
//		virtual Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount) override {
//			return {};
//		}
//		
//		virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override {
//			
//		}
//		
//		virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor, float inHeight) override {
//			
//		}
//};

struct rde_physics_shape {
	JPH::Shape* inner_shape;
	RDE_PHYSICS_SHAPE_ shape_type;
};
rde_physics_shape create_shape_struct() {
	rde_physics_shape _s;
	_s.inner_shape = nullptr;
	return _s;
}

struct rde_physics_body {
	rde_physics_shape shape;
	JPH::Body* inner_body;
	RDE_PHYSICS_BODY_MOTION_TYPE_ motion_type;
	rde_transform* transform;
	size_t layer;
	size_t index;
};
rde_physics_body create_body_struct() {
	rde_physics_body _j;
	_j.shape = create_shape_struct();
	_j.inner_body = nullptr;
	_j.motion_type = RDE_PHYSICS_BODY_MOTION_TYPE_STATIC;
	_j.transform = nullptr;
	_j.layer = 0;
	_j.index = -1;
	return _j;
}

static rde_physics_init_config init_config;
static JPH::TempAllocatorImpl* temp_allocator;
static JPH::PhysicsSystem* physics_system;
static JPH::JobSystem* job_system;
static JPH::BodyInterface* body_interface;
static rde_broad_phase_layer_interface* broad_phase_layer_interface;
static rde_object_vs_broad_phase_layer_filter* object_vs_broad_phase_layer_filter;
static rde_object_layer_pair_filter* object_layer_pair_filter;
static rde_contact_listener* contact_listener;
static rde_activation_listener* activation_listener;
static rde_physics_body* body_pool;

#define RDE_PHYSICS_PI 3.14159265358979323846f
#define DEGS_TO_RADS(_a) (_a * RDE_PHYSICS_PI) / 180.f
#define RADS_TO_DEGS(_a) (_a * 180.f) / RDE_PHYSICS_PI

static rde_physics_error_fn rde_error;
static rde_physics_log_fn rde_log;
static rde_physics_transform_get_pos rde_transform_get_pos;
static rde_physics_transform_get_rot rde_transform_get_rot_degs;
static rde_physics_transform_set_pos rde_transform_set_pos;
static rde_physics_transform_set_rot rde_transform_set_rot;

static int last_used_body_index;

bool rde_physics_init(rde_physics_init_config _init_config, rde_physics_callbacks _callbacks) {
	assert(_callbacks.error_fn 		!= nullptr && "Callback 'error_fn' MUST be not nullptr");
	assert(_callbacks.log_fn   		!= nullptr && "Callback 'log_fn' MUST be not nullptr");
	assert(_callbacks.get_pos_fn 	!= nullptr && "Callback 'get_pos_fn' MUST be not nullptr");
	assert(_callbacks.get_rot_fn 	!= nullptr && "Callback 'get_rot_fn' MUST be not nullptr");
	assert(_callbacks.set_pos_fn 	!= nullptr && "Callback 'set_pos_fn' MUST be not nullptr");
	assert(_callbacks.set_rot_fn 	!= nullptr && "Callback 'set_rot_fn' MUST be not nullptr");
	rde_error = _callbacks.error_fn;
	rde_log = _callbacks.log_fn;
	rde_transform_get_pos = _callbacks.get_pos_fn;
	rde_transform_get_rot_degs = _callbacks.get_rot_fn;
	rde_transform_set_pos = _callbacks.set_pos_fn;
	rde_transform_set_rot = _callbacks.set_rot_fn;

	body_pool = new rde_physics_body[_init_config.max_amount_of_bodies];
	for(size_t _i = 0; _i < _init_config.max_amount_of_bodies; _i++) {
		body_pool[_i] = create_body_struct();
	}

	rde_error(body_pool == nullptr, "'_body_pool' cannot be nullptr");

	init_config = _init_config;
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

    rde_error(_init_config.max_amount_of_physics_jobs == 0, "max_physics_jobs cannot be 0. \n");
    rde_error(_init_config.max_amount_of_physics_barriers == 0, "max_physics_barriers cannot be 0. \n");
    rde_error(_init_config.max_amount_of_threads == 0, "max_threads cannot be 0. \n");

	temp_allocator = new JPH::TempAllocatorImpl(_init_config.temp_allocator_size);
	job_system = new JPH::JobSystemThreadPool(_init_config.max_amount_of_physics_jobs, 
	                                          _init_config.max_amount_of_physics_barriers, 
	                                          _init_config.max_amount_of_threads);

	//JPH::DebugRenderer::sInstance = new rde_debug_renderer();

	broad_phase_layer_interface = new rde_broad_phase_layer_interface();
	object_vs_broad_phase_layer_filter = new rde_object_vs_broad_phase_layer_filter();
	object_layer_pair_filter = new rde_object_layer_pair_filter();
	activation_listener = new rde_activation_listener();
	contact_listener = new rde_contact_listener();

	physics_system = new JPH::PhysicsSystem();
	physics_system->Init(_init_config.max_amount_of_bodies, 
	                     _init_config.max_amount_of_body_mutexes, 
	                     _init_config.max_amount_of_body_pairs, 
	                     _init_config.max_amount_of_contact_constraints, 
	                     *broad_phase_layer_interface, 
	                     *object_vs_broad_phase_layer_filter, 
	                     *object_layer_pair_filter);

	physics_system->SetBodyActivationListener(activation_listener);
	physics_system->SetContactListener(contact_listener);
	body_interface = &physics_system->GetBodyInterface();

	last_used_body_index = -1;

	return true;
}

rde_physics_body* rde_physics_body_load(RDE_PHYSICS_SHAPE_ _shape_type, rde_physics_body_settings _body_settings, void* _shape_settings, rde_transform* _transform) {
	rde_physics_body* _body = nullptr;
	size_t _index = -1;

	for(size_t _i = 0; _i < init_config.max_amount_of_bodies; _i++) {
		rde_physics_body* _b = &body_pool[_i];

		if(_b->inner_body == nullptr) {
			_body = _b;

			if(last_used_body_index < (int)_i) {
				last_used_body_index = _i;
			}

			_index = _i;

			break;
		}
	}

	rde_error(_body == nullptr, "Max number of bodies '%d' reached. Crashing application \n", init_config.max_amount_of_bodies);

	JPH::ShapeSettings::ShapeResult _shape;

	switch(_shape_type) {
		case RDE_PHYSICS_SHAPE_BOX: {
			rde_physics_box_shape_settings* _rde_shape_settings = (rde_physics_box_shape_settings*)_shape_settings;
			JPH::BoxShapeSettings _jolt_shape_settings = JPH::BoxShapeSettings(JPH::Vec3(_rde_shape_settings->width,
			                                                                             _rde_shape_settings->height,
			                                                                             _rde_shape_settings->depth));
			_shape = _jolt_shape_settings.Create();
		} break;
		
		case RDE_PHYSICS_SHAPE_SPHERE: {
			rde_physics_sphere_shape_settings* _rde_shape_settings = (rde_physics_sphere_shape_settings*)_shape_settings;
			JPH::SphereShapeSettings _jolt_shape_settings = JPH::SphereShapeSettings(_rde_shape_settings->radius);
			_shape = _jolt_shape_settings.Create();
		} break;

		default: {
			rde_error(true, "RDE Jolt Physics, tried to add a non supported or implemented shape with id '%d' \n", _shape_type);
		}
	}

	_body->shape.inner_shape = &(*_shape.Get());
	_body->shape.shape_type = _shape_type;

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	rde_vec_3F _rotation = rde_transform_get_rot_degs(_transform);
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_rotation.z), 
	                                                       DEGS_TO_RADS(_rotation.y), 
	                                                       DEGS_TO_RADS(_rotation.x)));
	
	rde_vec_3F _position = rde_transform_get_pos(_transform);
	JPH::BodyCreationSettings _body_creation_settings(_body->shape.inner_shape, 
	                                                  JPH::RVec3(_position.x, 
	                                                             _position.y,
	                                                             _position.z), 
	                                                             _quat,
	                                                             (JPH::EMotionType)_body_settings.motion_type, 
	                                                             _body_settings.layer);

	// Create the actual rigid body
	_body->inner_body = body_interface->CreateBody(_body_creation_settings); // Note that if we run out of bodies this c
	_body->motion_type = _body_settings.motion_type;
	_body->transform = _transform;
	_body->layer = _body_settings.layer;
	_body->index = _index;

	return _body;
}

void rde_physics_body_unload(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	if(body_interface->IsAdded(_body->inner_body->GetID())) {
		body_interface->RemoveBody(_body->inner_body->GetID());
	}

	body_interface->DestroyBody(_body->inner_body->GetID());

	if(last_used_body_index == _body->index) {
		int _new_last_index = -1;

		for(int _i = 0; _i < last_used_body_index; _i++) {
			rde_physics_body* _body = &body_pool[_i];
			
			if(_body->inner_body != nullptr) {
				_new_last_index = _i;
			}
		}

		last_used_body_index = _new_last_index;
	}

	*_body = create_body_struct();
}

void rde_physics_body_add_to_simulation(rde_physics_body* _body, RDE_PHYSICS_BODY_ACTIVATION_ _activation) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddBody(_body->inner_body->GetID(), (JPH::EActivation)_activation);
}

void rde_physics_body_remove_from_simulation(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	if(body_interface->IsAdded(_body->inner_body->GetID())) {
		body_interface->RemoveBody(_body->inner_body->GetID());
	}
}

void rde_physics_body_set_active(rde_physics_body* _body, bool _active) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	
	if(_active) {
		body_interface->ActivateBody(_body->inner_body->GetID());
	} else {
		body_interface->DeactivateBody(_body->inner_body->GetID());
	}
}

void rde_physics_body_set_transform(rde_physics_body* _body, rde_transform* _transform) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	rde_error(_body == NULL, "%s had '_transform' NULL, crashing application", __FUNCTION__);

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	rde_vec_3F _rotation = rde_transform_get_rot_degs(_transform);
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_rotation.z), 
	                                                       DEGS_TO_RADS(_rotation.y), 
	                                                       DEGS_TO_RADS(_rotation.x)));
	
	rde_vec_3F _position = rde_transform_get_pos(_transform);
	body_interface->SetPositionAndRotation(_body->inner_body->GetID(),
										   { _position.x, _position.y, _position.z },
										   _quat, 
										   _body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void rde_physics_body_set_position(rde_physics_body* _body, rde_vec_3F _position) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->SetPosition(_body->inner_body->GetID(),
	                            { _position.x, _position.y, _position.z },
								_body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void rde_physics_body_set_rotation(rde_physics_body* _body, rde_vec_3F _rotation_degs) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_rotation_degs.z), 
	                                                       DEGS_TO_RADS(_rotation_degs.y), 
	                                                       DEGS_TO_RADS(_rotation_degs.x)));
	body_interface->SetRotation(_body->inner_body->GetID(), 
	                            _quat, 
	                            _body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

rde_quaternion rde_physics_body_get_rotation_quat(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Quat _quat = _body->inner_body->GetRotation();
	// I think this is wrong, need to check it out. 
	return { _quat.GetZ(), _quat.GetY(), _quat.GetX(), -_quat.GetW() };
}

rde_vec_3F rde_physics_body_get_rotation_euler_rads(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Vec3 _euler = _body->inner_body->GetRotation().GetEulerAngles();

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	return {
		-_euler.GetZ(),
		-_euler.GetY(),
		-_euler.GetX()
	};
}

rde_vec_3F rde_physics_body_get_rotation_euler_degs(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Vec3 _euler = _body->inner_body->GetRotation().GetEulerAngles();

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	return {
		-RADS_TO_DEGS(_euler.GetZ()),
		-RADS_TO_DEGS(_euler.GetY()),
		-RADS_TO_DEGS(_euler.GetX())
	};
}

float rde_physics_body_get_friction(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->GetFriction();
}

void rde_physics_body_set_friction(rde_physics_body* _body, float _friction) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_friction = _friction > 1.0f ? 1.0f : (_friction < 0.0f ? 0.0f : _friction);
	_body->inner_body->SetFriction(_friction);
}

float rde_physics_body_get_restitution(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->GetRestitution();
}

void rde_physics_body_set_restitution(rde_physics_body* _body, float _restitution) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_restitution = _restitution > 1.0f ? 1.0f : (_restitution < 0.0f ? 0.0f : _restitution);
	_body->inner_body->SetRestitution(_restitution);
}

float rde_physics_body_get_mass(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	if(_body->inner_body->GetMotionType() != JPH::EMotionType::Dynamic) {
		// Non Dynamic bodies have infinite mass
		return 0;
	}

	return 1.f / _body->inner_body->GetMotionPropertiesUnchecked()->GetInverseMass();
}

void rde_physics_body_set_mass(rde_physics_body* _body, float _mass) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	if(_body->inner_body->GetMotionType() != JPH::EMotionType::Dynamic) {
		rde_log(RDE_LOG_LEVEL_WARNING, "Non Dynamic bodies cannot set their mass, it is infinite \n");
		return;
	}
	JPH::EAllowedDOFs _dofs = _body->inner_body->GetMotionProperties()->GetAllowedDOFs();
	_body->inner_body->GetMotionProperties()->SetMassProperties(_dofs, {
	                                                            .mMass = _mass,
	                                                            .mInertia = _body->shape.inner_shape->GetMassProperties().mInertia
	});
}

bool rde_physics_body_is_sensor(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->IsSensor();
}

void rde_physics_body_set_sensor(rde_physics_body* _body, bool _sensor) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->SetIsSensor(_sensor);
}

RDE_PHYSICS_BODY_MOTION_TYPE_ rde_physics_body_get_motion_type(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return (RDE_PHYSICS_BODY_MOTION_TYPE_)_body->inner_body->GetMotionType();
}

void rde_physics_body_set_motion_type(rde_physics_body* _body, RDE_PHYSICS_BODY_MOTION_TYPE_ _motion_type) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->SetMotionType((JPH::EMotionType)_motion_type);
}

RDE_PHYSICS_BODY_DOF_ rde_physics_body_get_degrees_of_freedom(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return (RDE_PHYSICS_BODY_DOF_)_body->inner_body->GetMotionProperties()->GetAllowedDOFs();
}

void rde_physics_body_set_degrees_of_freedom(rde_physics_body* _body, RDE_PHYSICS_BODY_DOF_ _degrees_of_freedom) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->GetMotionProperties()->SetMassProperties((JPH::EAllowedDOFs)_degrees_of_freedom,
	                                                            _body->shape.inner_shape->GetMassProperties());
}

bool rde_physics_body_is_sleeping(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return !_body->inner_body->IsActive();
}

float rde_physics_body_get_gravity_factor(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return body_interface->GetGravityFactor(_body->inner_body->GetID());
}

void rde_physics_body_set_gravity_factor(rde_physics_body* _body, float _gravity_factor) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->SetGravityFactor(_body->inner_body->GetID(), _gravity_factor);
}

void rde_physics_body_add_impulse(rde_physics_body* _body, rde_vec_3F _impulse) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddImpulse(_body->inner_body->GetID(), { _impulse.x, _impulse.y, _impulse.z });
}

void rde_physics_body_add_force(rde_physics_body* _body, rde_vec_3F _force) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddForce(_body->inner_body->GetID(), { _force.x, _force.y, _force.z });
}

void rde_physics_body_add_linear_velocity(rde_physics_body* _body, rde_vec_3F _linear_velocity) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddLinearVelocity(_body->inner_body->GetID(), { _linear_velocity.x, _linear_velocity.y, _linear_velocity.z });
}

void rde_physics_body_add_torque(rde_physics_body* _body, rde_vec_3F _torque) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddTorque(_body->inner_body->GetID(), { _torque.x, _torque.y, _torque.z });
}

void rde_physics_body_add_angular_impulse(rde_physics_body* _body, rde_vec_3F _angular_impulse) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddAngularImpulse(_body->inner_body->GetID(), { _angular_impulse.x, _angular_impulse.y, _angular_impulse.z });
}

void rde_physics_body_add_angular_linear_velocity(rde_physics_body* _body, rde_vec_3F _angular_linear_velocity) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->SetAngularVelocity(_body->inner_body->GetID(), { _angular_linear_velocity.z, _angular_linear_velocity.y, _angular_linear_velocity.x });
}

rde_physics_shape* rde_physics_body_get_shape(rde_physics_body* _body) {
	rde_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return &_body->shape;
}

RDE_PHYSICS_SHAPE_ rde_physics_shape_get_type(rde_physics_shape* _shape) {
	rde_error(_shape == NULL, "%s had '_shape' NULL, crashing application", __FUNCTION__);
	return _shape->shape_type;
}

void rde_physics_shape_get_bounds(rde_physics_shape* _shape, RDE_PHYSICS_SHAPE_ _shape_type, void* _out_bounds) {
	rde_error(_shape == NULL, "%s had '_shape' NULL, crashing application", __FUNCTION__);

	switch(_shape_type) {
		case RDE_PHYSICS_SHAPE_BOX: {
			JPH::BoxShape* _box_shape = (JPH::BoxShape*)_shape->inner_shape;
			JPH::Vec3 _half_extent = _box_shape->GetHalfExtent();
			rde_physics_box_shape_bounds* _rde_box_shape = (rde_physics_box_shape_bounds*)_out_bounds;
			_rde_box_shape->width = _half_extent.GetX();
			_rde_box_shape->height = _half_extent.GetY();
			_rde_box_shape->depth = _half_extent.GetZ();
		} break;

		case RDE_PHYSICS_SHAPE_SPHERE: {
			JPH::SphereShape* _sphere_shape = (JPH::SphereShape*)_shape->inner_shape;
			float _radius = _sphere_shape->GetRadius();
			rde_physics_sphere_shape_bounds* _rde_box_shape = (rde_physics_sphere_shape_bounds*)_out_bounds;
			_rde_box_shape->radius = _radius;
		} break;

		default: {
			rde_error(true, "rde_physics_shape_get_bounds shape id '%d' not supported or not implemented \n", _shape_type);
		} break;
	}
}


void rde_physics_update(float _fixed_dt) {
	JPH::EPhysicsUpdateError _error = physics_system->Update(_fixed_dt, init_config.collision_steps_per_update, temp_allocator, job_system);
	rde_error(_error != JPH::EPhysicsUpdateError::None, "RDE Jolt Physics Update error code '%d', crashing before malfunctioning", _error);

	for(int _i = 0; _i <= last_used_body_index; _i++) {
		rde_physics_body* _body = &body_pool[_i];
		if(_body->inner_body == NULL) {
			continue;
		}

		JPH::RVec3 _position = _body->inner_body->GetPosition();
		JPH::Quat _quat = _body->inner_body->GetRotation();

		if(_body->motion_type != RDE_PHYSICS_BODY_MOTION_TYPE_STATIC) {
			JPH::Vec3 _euler = _quat.GetEulerAngles();
			rde_transform_set_pos(_body->transform, (rde_vec_3F) {
				_position.GetX(),
				_position.GetY(),
				_position.GetZ()
			});

			// Jolt is righ-handed with Y-Up, RDE is left-handed with Y-Up, this modifications are used to 
			// transform form Jolt's Coordinate system to RDE's Coordinate system
			rde_transform_set_pos(_body->transform, (rde_vec_3F) {
				-RADS_TO_DEGS(_euler.GetZ()),
				-RADS_TO_DEGS(_euler.GetY()),
				-RADS_TO_DEGS(_euler.GetX())
			});
		}
	}
}

rde_quaternion rde_physics_euler_degs_to_quaternion(rde_vec_3F _euler) {
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_euler.x), DEGS_TO_RADS(_euler.y), DEGS_TO_RADS(_euler.z)));
	return { _quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW() };
}

rde_quaternion rde_physics_euler_rads_to_quaternion(rde_vec_3F _euler) {
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(_euler.x, _euler.y, _euler.z));
	return { _quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW() };
}

rde_vec_3F rde_physics_quaternion_to_euler_degs(rde_quaternion _quat) {
	JPH::Quat _q(_quat.x, _quat.y, _quat.z, _quat.w);
	JPH::Vec3 _euler = _q.GetEulerAngles();
	return { RADS_TO_DEGS(_euler.GetX()), RADS_TO_DEGS(_euler.GetY()), RADS_TO_DEGS(_euler.GetZ()) };
}

rde_vec_3F rde_physics_quaternion_to_euler_rads(rde_quaternion _quat) {
	JPH::Quat _q(_quat.x, _quat.y, _quat.z, _quat.w);
	JPH::Vec3 _euler = _q.GetEulerAngles();
	return { _euler.GetX(), _euler.GetY(), _euler.GetZ() };
}

void rde_physics_iterate_over_bodies(rde_physics_body_iter_callback_fn _iterate_body_callback) {
	if(_iterate_body_callback == nullptr) {
		return;
	}

	for (int _i = 0; _i <= last_used_body_index; _i++) {
		rde_physics_body* _body = &body_pool[_i];
		if (_body->inner_body == NULL) {
			continue;
		}

		_iterate_body_callback(_body, &_body->shape, _body->transform);
	}
}

void rde_physics_end() {

	for(int _i = 0; _i <= last_used_body_index; _i++) {
		rde_physics_body* _body = &body_pool[_i];
		
		if(_body->inner_body != nullptr) {
			rde_physics_body_unload(_body);
		}
	}

	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	if(physics_system == nullptr) {
		return;
	}

//	delete JPH::DebugRenderer::sInstance;
//	JPH::DebugRenderer::sInstance = nullptr;

	delete physics_system;
	physics_system = nullptr;

	delete job_system;
	job_system = nullptr;
	
	delete temp_allocator;
	temp_allocator = NULL;

	delete broad_phase_layer_interface;
	broad_phase_layer_interface = nullptr;

	delete object_vs_broad_phase_layer_filter;
	object_vs_broad_phase_layer_filter = nullptr;

	delete object_layer_pair_filter;
	object_layer_pair_filter = nullptr;

	delete activation_listener;
	activation_listener = nullptr;

	delete contact_listener;
	contact_listener = nullptr;

	body_interface = nullptr;

	delete [] body_pool;
}
