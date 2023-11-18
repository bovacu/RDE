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

typedef struct {
	size_t temo_allocator_size;
	size_t max_physics_jobs;
	size_t max_physics_barriers;
	size_t max_bodies;
	size_t max_body_mutexes;
	size_t max_body_pairs;
	size_t max_concat_constraints;
	int    max_threads;
} rde_jolt_init_config;

RDE_FUNC bool rde_jolt_init(rde_jolt_init_config _init_config);
RDE_FUNC void rde_jolt_end();

#ifdef __cplusplus
}
#endif