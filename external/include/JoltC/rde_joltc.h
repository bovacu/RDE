#include <stdint.h>
#include <stddef.h>

#ifndef RDE_FUNC
	#if _WIN32
		#define RDE_FUNC __declspec(dllexport)
		#define RDE_FUNC_STATIC __declspec(dllexport) static
		#define RDE_FUNC_EXTERNAL extern "C" RDE_FUNC
	#else
		#define RDE_FUNC 
		#define RDE_FUNC_STATIC static
		#define RDE_FUNC_EXTERNAL
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RDE_H
typedef struct {
	float x;
	float y;
	float z;
} rde_vec_3F;

typedef struct {
	float x; // imaginary part (i)
	float y; // imaginary part (j)
	float z; // imaginary part (k)
	float w; // real part
} rde_quaternion;

typedef struct rde_transform rde_transform;
struct rde_transform {
	rde_vec_3F position;
	rde_vec_3F rotation;
	rde_vec_3F scale;
	rde_transform* parent;
};

typedef enum { 
	RDE_LOG_LEVEL_DEBUG,
	RDE_LOG_LEVEL_INFO,
	RDE_LOG_LEVEL_WARNING,
	RDE_LOG_LEVEL_ERROR
} RDE_LOG_LEVEL_;
#endif

typedef enum {
	RDE_JOLT_BODY_MOTION_TYPE_STATIC,		///< Non movable
	RDE_JOLT_BODY_MOTION_TYPE_KINEMATIC,	///< Movable using velocities only, does not respond to forces
	RDE_JOLT_BODY_MOTION_TYPE_DYNAMIC,		///< Responds to forces as a normal physics object
} RDE_JOLT_BODY_MOTION_TYPE_;

typedef enum {
	RDE_JOLT_BODY_ACTIVATION_ACTIVATE,		///< Activate the body, making it part of the simulation
	RDE_JOLT_BODY_ACTIVATION_DONT_ACTIVATE	///< Leave activation state as it is (will not deactivate an active body)
} RDE_JOLT_BODY_ACTIVATION_;

typedef enum {
	RDE_JOLT_SHAPE_BOX,
	RDE_JOLT_SHAPE_SPHERE,
} RDE_JOLT_SHAPE_;

typedef enum {
	RDE_JOLT_BODY_DOF_NONE				= 0b000000,									///< No degrees of freedom are allowed. Note that this is not valid and will crash. Use a static body instead.
	RDE_JOLT_BODY_DOF_ALL				= 0b111111,									///< All degrees of freedom are allowed
	RDE_JOLT_BODY_DOF_TRANSLATION_X		= 0b000001,									///< Body cannot move in world space X axis
	RDE_JOLT_BODY_DOF_TRANSLATION_Y		= 0b000010,									///< Body cannot move in world space Y axis
	RDE_JOLT_BODY_DOF_TRANSLATION_Z		= 0b000100,									///< Body cannot move in world space Z axis
	RDE_JOLT_BODY_DOF_ROTATION_X		= 0b001000,									///< Body cannot rotate around local space X axis
	RDE_JOLT_BODY_DOF_ROTATION_Y		= 0b010000,									///< Body cannot rotate around local space Y axis
	RDE_JOLT_BODY_DOF_ROTATION_Z		= 0b100000,									///< Body cannot rotate around local space Z axis
	RDE_JOLT_BODY_DOF_PLANE_2D			= RDE_JOLT_BODY_DOF_TRANSLATION_X | RDE_JOLT_BODY_DOF_TRANSLATION_Y | RDE_JOLT_BODY_DOF_TRANSLATION_Z,	///< Body can only move in X and Y axis and rotate around Z axis
	RDE_JOLT_BODY_DOF_ALL_TRANSLATION	= RDE_JOLT_BODY_DOF_TRANSLATION_X | RDE_JOLT_BODY_DOF_TRANSLATION_Y | RDE_JOLT_BODY_DOF_TRANSLATION_Z,
	RDE_JOLT_BODY_DOF_ALL_ROTATION		= RDE_JOLT_BODY_DOF_ROTATION_X | RDE_JOLT_BODY_DOF_ROTATION_Y | RDE_JOLT_BODY_DOF_ROTATION_Z,
} RDE_JOLT_BODY_DOF_;

typedef unsigned int rde_jolt_body_id;

typedef struct rde_jolt_shape rde_jolt_shape;
typedef struct rde_jolt_body rde_jolt_body;
typedef void(*critical_error)(bool, const char*, ...);
typedef void(*logger)(RDE_LOG_LEVEL_, const char*, ...);
typedef void(*body_iter_callback)(rde_jolt_body*, rde_jolt_shape*, rde_transform*);

typedef struct {
	float width;
	float height;
	float depth;
} rde_jolt_box_shape_settings;

typedef struct {
	float radius;
} rde_jolt_sphere_shape_settings;

typedef struct {
	RDE_JOLT_BODY_MOTION_TYPE_ motion_type;
	size_t layer;
	float mass;
	float restitution;
	float friction;
	RDE_JOLT_BODY_DOF_ degrees_of_freedom;
	bool is_sensor;
} rde_jolt_body_settings;

typedef struct {
	size_t temp_allocator_size;
	size_t max_amount_of_physics_jobs;
	size_t max_amount_of_physics_barriers;
	size_t max_amount_of_bodies;
	size_t max_amount_of_body_mutexes;
	size_t max_amount_of_body_pairs;
	size_t max_amount_of_contact_constraints;
	int    max_amount_of_threads;
	size_t collision_steps_per_update;
} rde_jolt_init_config;

typedef struct {
	float width;
	float height;
	float depth;
} rde_jolt_box_shape_bounds;

typedef struct {
	float radius;
} rde_jolt_sphere_shape_bounds;

RDE_FUNC bool rde_jolt_init(rde_jolt_init_config _init_config, critical_error _rde_critical_error_callback, logger _log_callback);
RDE_FUNC rde_jolt_body* rde_jolt_body_load(RDE_JOLT_SHAPE_ _shape_type, rde_jolt_body_settings _body_settings, void* _shape_settings, rde_transform* _transform);
RDE_FUNC void rde_jolt_body_unload(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_add_to_simulation(rde_jolt_body* _body, RDE_JOLT_BODY_ACTIVATION_ _activation);
RDE_FUNC void rde_jolt_body_remove_from_simulation(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_active(rde_jolt_body* _body, bool _active);
RDE_FUNC void rde_jolt_body_set_transform(rde_jolt_body* _body, rde_transform* _transform);

RDE_FUNC void rde_jolt_body_set_position(rde_jolt_body* _body, rde_vec_3F _position);
RDE_FUNC rde_vec_3F rde_jolt_body_get_position(rde_jolt_body* _body);

RDE_FUNC void rde_jolt_body_set_rotation(rde_jolt_body* _body, rde_vec_3F _rotation_degs);
RDE_FUNC rde_quaternion rde_jolt_body_get_rotation_quat(rde_jolt_body* _body);
RDE_FUNC rde_vec_3F rde_jolt_body_get_rotation_euler_rads(rde_jolt_body* _body);
RDE_FUNC rde_vec_3F rde_jolt_body_get_rotation_euler_degs(rde_jolt_body* _body);

RDE_FUNC float rde_jolt_body_get_friction(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_friction(rde_jolt_body* _body, float _friction);

RDE_FUNC float rde_jolt_body_get_restitution(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_restitution(rde_jolt_body* _body, float _restitution);

RDE_FUNC float rde_jolt_body_get_mass(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_mass(rde_jolt_body* _body, float _mass);

RDE_FUNC bool rde_jolt_body_is_sensor(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_sensor(rde_jolt_body* _body, bool _sensor);

RDE_FUNC float rde_jolt_body_get_gravity_factor(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_gravity_factor(rde_jolt_body* _body, float _gravity_factor);

RDE_FUNC void rde_jolt_body_add_impulse(rde_jolt_body* _body, rde_vec_3F _impulse);
RDE_FUNC void rde_jolt_body_add_force(rde_jolt_body* _body, rde_vec_3F _force);
RDE_FUNC void rde_jolt_body_add_linear_velocity(rde_jolt_body* _body, rde_vec_3F _linear_velocity);
RDE_FUNC void rde_jolt_body_add_torque(rde_jolt_body* _body, rde_vec_3F _torque);
RDE_FUNC void rde_jolt_body_add_angular_impulse(rde_jolt_body* _body, rde_vec_3F _angular_impulse);
RDE_FUNC void rde_jolt_body_add_angular_linear_velocity(rde_jolt_body* _body, rde_vec_3F _angular_linear_velocity);

RDE_FUNC bool rde_jolt_body_is_sleeping(rde_jolt_body* _body);

RDE_FUNC RDE_JOLT_BODY_MOTION_TYPE_ rde_jolt_body_get_motion_type(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_motion_type(rde_jolt_body* _body, RDE_JOLT_BODY_MOTION_TYPE_ _motion_type);

RDE_FUNC RDE_JOLT_BODY_DOF_ rde_jolt_body_get_degrees_of_freedom(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_body_set_degrees_of_freedom(rde_jolt_body* _body, RDE_JOLT_BODY_DOF_ _degrees_of_freedom);

RDE_FUNC rde_jolt_shape* rde_jolt_body_get_shape(rde_jolt_body* _body);
RDE_FUNC void rde_jolt_shape_get_bounds(rde_jolt_shape* _shape, RDE_JOLT_SHAPE_ _shape_type, void* _out_bounds);
RDE_FUNC RDE_JOLT_SHAPE_ rde_jolt_shape_get_type(rde_jolt_shape* _shape);

RDE_FUNC void rde_jolt_update(float _fixed_dt);

RDE_FUNC rde_quaternion rde_jolt_euler_degs_to_quaternion(rde_vec_3F _euler);
RDE_FUNC rde_quaternion rde_jolt_euler_rads_to_quaternion(rde_vec_3F _euler);
RDE_FUNC rde_vec_3F rde_jolt_quaternion_to_euler_degs(rde_quaternion _quat);
RDE_FUNC rde_vec_3F rde_jolt_quaternion_to_euler_rads(rde_quaternion _quat);

RDE_FUNC void rde_jolt_iterate_over_bodies(body_iter_callback _iterate_body_callback);

RDE_FUNC void rde_jolt_end();

#ifdef __cplusplus
}
#endif