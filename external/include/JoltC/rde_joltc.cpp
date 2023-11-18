#include "rde_joltc.h"

#include <iostream>

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
	virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {
		std::cout << "A body got activated" << std::endl;
	}
	
	virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {
		std::cout << "A body went to sleep" << std::endl;
	}
};

class rde_contact_listener : public JPH::ContactListener {
	public:
		virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override {
			std::cout << "Contact validate callback" << std::endl;
			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}
	
		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {
			std::cout << "A contact was added" << std::endl;
		}
	
		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {
			std::cout << "A contact was persisted" << std::endl;
		}
	
		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
			std::cout << "A contact was removed" << std::endl;
		}
};

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine) {
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};

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

bool rde_jolt_init(rde_jolt_init_config _init_config) {

	//JPH::AssertFailed = AssertFailedImpl;

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

	return true;
}

void rde_jolt_end() {
	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	if(physics_system == NULL) {
		return;
	}

	delete physics_system;
	physics_system = NULL;

	delete job_system;
	job_system = NULL;

	delete broad_phase_layer_interface;
	broad_phase_layer_interface = NULL;

	delete object_vs_broad_phase_layer_filter;
	object_vs_broad_phase_layer_filter = NULL;

	delete object_layer_pair_filter;
	object_layer_pair_filter = NULL;

	delete activation_listener;
	activation_listener = NULL;

	delete contact_listener;
	contact_listener = NULL;

	body_interface = NULL;
}