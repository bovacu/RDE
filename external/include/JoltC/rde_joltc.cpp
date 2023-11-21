#include "rde_joltc.h"

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

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine) {
	return true;
};

struct rde_jolt_shape {
	JPH::Shape* inner_shape;
};
rde_jolt_shape create_shape_struct() {
	rde_jolt_shape _s;
	_s.inner_shape = nullptr;
	return _s;
}

struct rde_jolt_body {
	rde_jolt_shape shape;
	JPH::Body* inner_body;
	RDE_JOLT_BODY_MOTION_TYPE_ motion_type;
	rde_transform* transform;
	size_t layer;
	size_t index;
};
rde_jolt_body create_body_struct() {
	rde_jolt_body _j;
	_j.shape = create_shape_struct();
	_j.inner_body = nullptr;
	_j.motion_type = RDE_JOLT_BODY_MOTION_TYPE_STATIC;
	_j.transform = nullptr;
	_j.layer = 0;
	_j.index = -1;
	return _j;
}

static rde_jolt_init_config init_config;
static JPH::TempAllocatorImpl* temp_allocator;
static JPH::PhysicsSystem* physics_system;
static JPH::JobSystem* job_system;
static JPH::BodyInterface* body_interface;
static rde_broad_phase_layer_interface* broad_phase_layer_interface;
static rde_object_vs_broad_phase_layer_filter* object_vs_broad_phase_layer_filter;
static rde_object_layer_pair_filter* object_layer_pair_filter;
static rde_contact_listener* contact_listener;
static rde_activation_listener* activation_listener;
static rde_jolt_body* body_pool;

#define RDE_JOLT_PI 3.14159265358979323846f
#define DEGS_TO_RADS(_a) (_a * RDE_JOLT_PI) / 180.f
#define RADS_TO_DEGS(_a) (_a * 180.f) / RDE_JOLT_PI

static critical_error rde_critical_error;
static logger rde_log_level;

static int last_used_body_index;

bool rde_jolt_init(rde_jolt_init_config _init_config, critical_error _rde_critical_error_callback, logger _log_callback) {
	assert(_rde_critical_error_callback != nullptr && "Callback '_rde_critical_error_callback' MUST be not nullptr");
	rde_critical_error = _rde_critical_error_callback;

	rde_critical_error(_log_callback == nullptr, "Callback '_post_update_callback' MUST be not nullptr");
	rde_log_level = _log_callback;

	body_pool = new rde_jolt_body[_init_config.max_bodies];
	for(size_t _i = 0; _i < _init_config.max_bodies; _i++) {
		body_pool[_i] = create_body_struct();
	}

	rde_critical_error(body_pool == nullptr, "'_body_pool' cannot be nullptr");

	init_config = _init_config;
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();
	
	temp_allocator = new JPH::TempAllocatorImpl(_init_config.temo_allocator_size);
	job_system = new JPH::JobSystemThreadPool(_init_config.max_physics_jobs, 
	                                          _init_config.max_physics_barriers, 
	                                          _init_config.max_threads);

	broad_phase_layer_interface = new rde_broad_phase_layer_interface();
	object_vs_broad_phase_layer_filter = new rde_object_vs_broad_phase_layer_filter();
	object_layer_pair_filter = new rde_object_layer_pair_filter();
	activation_listener = new rde_activation_listener();
	contact_listener = new rde_contact_listener();

	physics_system = new JPH::PhysicsSystem();
	physics_system->Init(_init_config.max_bodies, 
	                     _init_config.max_body_mutexes, 
	                     _init_config.max_body_pairs, 
	                     _init_config.max_concat_constraints, 
	                     *broad_phase_layer_interface, 
	                     *object_vs_broad_phase_layer_filter, 
	                     *object_layer_pair_filter);

	activation_listener = new rde_activation_listener();
	physics_system->SetBodyActivationListener(activation_listener);

	contact_listener = new rde_contact_listener();
	physics_system->SetContactListener(contact_listener);

	body_interface = &physics_system->GetBodyInterface();

	last_used_body_index = -1;

	return true;
}

rde_jolt_body* rde_jolt_body_load(RDE_JOLT_SHAPE_ _shape_type, rde_jolt_body_settings _body_settings, void* _shape_settings, rde_transform* _transform) {
	rde_jolt_body* _body = nullptr;
	size_t _index = -1;

	for(size_t _i = 0; _i < init_config.max_bodies; _i++) {
		rde_jolt_body* _b = &body_pool[_i];

		if(_b->inner_body == nullptr) {
			_body = _b;

			if(last_used_body_index < (int)_i) {
				last_used_body_index = _i;
			}

			_index = _i;

			break;
		}
	}

	rde_critical_error(_body == nullptr, "Max number of bodies '%d' reached. Crashing application \n", init_config.max_bodies);

	JPH::ShapeSettings::ShapeResult _shape;

	switch(_shape_type) {
		case RDE_JOLT_SHAPE_BOX: {
			rde_jolt_box_shape_settings* _rde_shape_settings = (rde_jolt_box_shape_settings*)_shape_settings;
			JPH::BoxShapeSettings _jolt_shape_settings = JPH::BoxShapeSettings(JPH::Vec3(_rde_shape_settings->width,
			                                                                             _rde_shape_settings->height,
			                                                                             _rde_shape_settings->depth));
			_shape = _jolt_shape_settings.Create();
		} break;
		
		case RDE_JOLT_SHAPE_SPHERE: {
			rde_jolt_sphere_shape_settings* _rde_shape_settings = (rde_jolt_sphere_shape_settings*)_shape_settings;
			JPH::SphereShapeSettings _jolt_shape_settings = JPH::SphereShapeSettings(_rde_shape_settings->radius);
			_shape = _jolt_shape_settings.Create();
		} break;

		default: {
			rde_critical_error(true, "RDE Jolt Physics, tried to add a non supported or implemented shape with id '%d' \n", _shape_type);
		}
	}

	
	_body->shape.inner_shape = &(*_shape.Get());

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_transform->rotation.z), 
	                                                       DEGS_TO_RADS(_transform->rotation.y), 
	                                                       DEGS_TO_RADS(_transform->rotation.x)));
	JPH::BodyCreationSettings _body_creation_settings(_body->shape.inner_shape, 
	                                                  JPH::RVec3(_transform->position.x, 
	                                                             _transform->position.y,
	                                                             _transform->position.z), 
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

void rde_jolt_body_unload(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	if(body_interface->IsAdded(_body->inner_body->GetID())) {
		body_interface->RemoveBody(_body->inner_body->GetID());
	}

	body_interface->DestroyBody(_body->inner_body->GetID());

	if(last_used_body_index == _body->index) {
		int _new_last_index = -1;

		for(int _i = 0; _i < last_used_body_index; _i++) {
			rde_jolt_body* _body = &body_pool[_i];
			
			if(_body->inner_body != nullptr) {
				_new_last_index = _i;
			}
		}

		last_used_body_index = _new_last_index;
	}

	*_body = create_body_struct();
}

void rde_jolt_body_add_to_simulation(rde_jolt_body* _body, RDE_JOLT_BODY_ACTIVATION_ _activation) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddBody(_body->inner_body->GetID(), (JPH::EActivation)_activation);
}

void rde_jolt_body_remove_from_simulation(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	if(body_interface->IsAdded(_body->inner_body->GetID())) {
		body_interface->RemoveBody(_body->inner_body->GetID());
	}
}

void rde_jolt_body_set_active(rde_jolt_body* _body, bool _active) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	
	if(_active) {
		body_interface->ActivateBody(_body->inner_body->GetID());
	} else {
		body_interface->DeactivateBody(_body->inner_body->GetID());
	}
}

void rde_jolt_body_set_transform(rde_jolt_body* _body, rde_transform* _transform) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	rde_critical_error(_body == NULL, "%s had '_transform' NULL, crashing application", __FUNCTION__);

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_transform->rotation.z), 
	                                                       DEGS_TO_RADS(_transform->rotation.y), 
	                                                       DEGS_TO_RADS(_transform->rotation.x)));
	body_interface->SetPositionAndRotation(_body->inner_body->GetID(),
										   { _transform->position.x, _transform->position.y, _transform->position.z },
										   _quat, 
										   _body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void rde_jolt_body_set_position(rde_jolt_body* _body, rde_vec_3F _position) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->SetPosition(_body->inner_body->GetID(),
	                            { _position.x, _position.y, _position.z },
								_body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

void rde_jolt_body_set_rotation(rde_jolt_body* _body, rde_vec_3F _rotation_degs) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_rotation_degs.z), 
	                                                       DEGS_TO_RADS(_rotation_degs.y), 
	                                                       DEGS_TO_RADS(_rotation_degs.x)));
	body_interface->SetRotation(_body->inner_body->GetID(), 
	                            _quat, 
	                            _body->inner_body->IsActive() ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
}

rde_quaternion rde_jolt_body_get_rotation_quat(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Quat _quat = _body->inner_body->GetRotation();
	// I think this is wrong, need to check it out. 
	return { _quat.GetZ(), _quat.GetY(), _quat.GetX(), -_quat.GetW() };
}

rde_vec_3F rde_jolt_body_get_rotation_euler_rads(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Vec3 _euler = _body->inner_body->GetRotation().GetEulerAngles();

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	return {
		-_euler.GetZ(),
		-_euler.GetY(),
		-_euler.GetX()
	};
}

rde_vec_3F rde_jolt_body_get_rotation_euler_degs(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	JPH::Vec3 _euler = _body->inner_body->GetRotation().GetEulerAngles();

	// RDE is left-handed with Y-Up, Jolt is righ-handed with Y-Up, this modifications are used to 
	// transform form RDE's Coordinate system to Jolts's Coordinate system
	return {
		-RADS_TO_DEGS(_euler.GetZ()),
		-RADS_TO_DEGS(_euler.GetY()),
		-RADS_TO_DEGS(_euler.GetX())
	};
}

float rde_jolt_body_get_friction(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->GetFriction();
}

void rde_jolt_body_set_friction(rde_jolt_body* _body, float _friction) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_friction = _friction > 1.0f ? 1.0f : (_friction < 0.0f ? 0.0f : _friction);
	_body->inner_body->SetFriction(_friction);
}

float rde_jolt_body_get_restitution(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->GetRestitution();
}

void rde_jolt_body_set_restitution(rde_jolt_body* _body, float _restitution) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_restitution = _restitution > 1.0f ? 1.0f : (_restitution < 0.0f ? 0.0f : _restitution);
	_body->inner_body->SetRestitution(_restitution);
}

float rde_jolt_body_get_mass(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);

	if(_body->inner_body->GetMotionType() != JPH::EMotionType::Dynamic) {
		// Non Dynamic bodies have infinite mass
		return 0;
	}

	return 1.f / _body->inner_body->GetMotionPropertiesUnchecked()->GetInverseMass();
}

void rde_jolt_body_set_mass(rde_jolt_body* _body, float _mass) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	if(_body->inner_body->GetMotionType() != JPH::EMotionType::Dynamic) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "Non Dynamic bodies cannot set their mass, it is infinite \n");
		return;
	}
	JPH::EAllowedDOFs _dofs = _body->inner_body->GetMotionProperties()->GetAllowedDOFs();
	_body->inner_body->GetMotionProperties()->SetMassProperties(_dofs, {
	                                                            .mMass = _mass,
	                                                            .mInertia = _body->shape.inner_shape->GetMassProperties().mInertia
	});
}

bool rde_jolt_body_is_sensor(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return _body->inner_body->IsSensor();
}

void rde_jolt_body_set_sensor(rde_jolt_body* _body, bool _sensor) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->SetIsSensor(_sensor);
}

RDE_JOLT_BODY_MOTION_TYPE_ rde_jolt_body_get_motion_type(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return (RDE_JOLT_BODY_MOTION_TYPE_)_body->inner_body->GetMotionType();
}

void rde_jolt_body_set_motion_type(rde_jolt_body* _body, RDE_JOLT_BODY_MOTION_TYPE_ _motion_type) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->SetMotionType((JPH::EMotionType)_motion_type);
}

RDE_JOLT_BODY_DOF_ rde_jolt_body_get_degrees_of_freedom(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return (RDE_JOLT_BODY_DOF_)_body->inner_body->GetMotionProperties()->GetAllowedDOFs();
}

void rde_jolt_body_set_degrees_of_freedom(rde_jolt_body* _body, RDE_JOLT_BODY_DOF_ _degrees_of_freedom) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	_body->inner_body->GetMotionProperties()->SetMassProperties((JPH::EAllowedDOFs)_degrees_of_freedom,
	                                                            _body->shape.inner_shape->GetMassProperties());
}

bool rde_jolt_body_is_sleeping(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	rde_log_level(RDE_LOG_LEVEL_WARNING, "%s is not implemented yet \n", __FUNCTION__);
	return false;
}

float rde_jolt_body_get_gravity_factor(rde_jolt_body* _body) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	return body_interface->GetGravityFactor(_body->inner_body->GetID());
}

void rde_jolt_body_set_gravity_factor(rde_jolt_body* _body, float _gravity_factor) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->SetGravityFactor(_body->inner_body->GetID(), _gravity_factor);
}

void rde_jolt_body_add_impulse(rde_jolt_body* _body, rde_vec_3F _impulse) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddImpulse(_body->inner_body->GetID(), { _impulse.x, _impulse.y, _impulse.z });
}

void rde_jolt_body_add_force(rde_jolt_body* _body, rde_vec_3F _force) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddForce(_body->inner_body->GetID(), { _force.x, _force.y, _force.z });
}

void rde_jolt_body_add_linear_velocity(rde_jolt_body* _body, rde_vec_3F _linear_velocity) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddLinearVelocity(_body->inner_body->GetID(), { _linear_velocity.x, _linear_velocity.y, _linear_velocity.z });
}

void rde_jolt_body_add_torque(rde_jolt_body* _body, rde_vec_3F _torque) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddTorque(_body->inner_body->GetID(), { _torque.x, _torque.y, _torque.z });
}

void rde_jolt_body_add_angular_impulse(rde_jolt_body* _body, rde_vec_3F _angular_impulse) {
	rde_critical_error(_body == NULL, "%s had '_body' NULL, crashing application", __FUNCTION__);
	body_interface->AddAngularImpulse(_body->inner_body->GetID(), { _angular_impulse.x, _angular_impulse.y, _angular_impulse.z });
}

void rde_jolt_update(float _fixed_dt) {
	JPH::EPhysicsUpdateError _error = physics_system->Update(_fixed_dt, init_config.collision_steps_per_update, temp_allocator, job_system);
	rde_critical_error(_error != JPH::EPhysicsUpdateError::None, "RDE Jolt Physics Update error code '%d', crashing before malfunctioning", _error);

	for(int _i = 0; _i <= last_used_body_index; _i++) {
		rde_jolt_body* _body = &body_pool[_i];
		if(_body->inner_body == NULL) {
			continue;
		}

		JPH::RVec3 _position = _body->inner_body->GetPosition();
		JPH::Quat _quat = _body->inner_body->GetRotation();
		rde_jolt_body_post_update_info _body_post_update_info = {
			.position = { _position.GetX(), _position.GetY(), _position.GetZ() },
			.rotation = { _quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW() },
			.motion_type = _body->motion_type,
			.transform = _body->transform,
			.id = _body->inner_body->GetID().GetIndexAndSequenceNumber(),
			.layer = _body->layer
		};

		if(_body->motion_type != RDE_JOLT_BODY_MOTION_TYPE_STATIC) {
			JPH::Vec3 _euler = _quat.GetEulerAngles();
			_body->transform->position.x = _position.GetX();
			_body->transform->position.y = _position.GetY();
			_body->transform->position.z = _position.GetZ();

			// Jolt is righ-handed with Y-Up, RDE is left-handed with Y-Up, this modifications are used to 
			// transform form Jolt's Coordinate system to RDE's Coordinate system
			_body->transform->rotation.x = -RADS_TO_DEGS(_euler.GetZ());
			_body->transform->rotation.y = -RADS_TO_DEGS(_euler.GetY());
			_body->transform->rotation.z = -RADS_TO_DEGS(_euler.GetX());
		}
	}
}

rde_quaternion rde_jolt_euler_degs_to_quaternion(rde_vec_3F _euler) {
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(DEGS_TO_RADS(_euler.x), DEGS_TO_RADS(_euler.y), DEGS_TO_RADS(_euler.z)));
	return { _quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW() };
}

rde_quaternion rde_jolt_euler_rads_to_quaternion(rde_vec_3F _euler) {
	JPH::Quat _quat = JPH::Quat::sEulerAngles(JPH::Vec3Arg(_euler.x, _euler.y, _euler.z));
	return { _quat.GetX(), _quat.GetY(), _quat.GetZ(), _quat.GetW() };
}

rde_vec_3F rde_jolt_quaternion_to_euler_degs(rde_quaternion _quat) {
	JPH::Quat _q(_quat.x, _quat.y, _quat.z, _quat.w);
	JPH::Vec3 _euler = _q.GetEulerAngles();
	return { RADS_TO_DEGS(_euler.GetX()), RADS_TO_DEGS(_euler.GetY()), RADS_TO_DEGS(_euler.GetZ()) };
}

rde_vec_3F rde_jolt_quaternion_to_euler_rads(rde_quaternion _quat) {
	JPH::Quat _q(_quat.x, _quat.y, _quat.z, _quat.w);
	JPH::Vec3 _euler = _q.GetEulerAngles();
	return { _euler.GetX(), _euler.GetY(), _euler.GetZ() };
}


void rde_jolt_end() {

	for(int _i = 0; _i <= last_used_body_index; _i++) {
		rde_jolt_body* _body = &body_pool[_i];
		
		if(_body->inner_body != nullptr) {
			rde_jolt_body_unload(_body);
		}
	}

	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	if(physics_system == nullptr) {
		return;
	}

	delete physics_system;
	physics_system = nullptr;

	delete job_system;
	job_system = nullptr;

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