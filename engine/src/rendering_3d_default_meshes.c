#define CUBE_MESH_DATA(_size) 									\
	const size_t _vertex_count = 36;							\
	float* _positions = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_positions, (float []){								\
        -1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		 1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		-1.0f * _size, -1.0f * _size, -1.0f * _size,			\
        -1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		   														\
        -1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		 1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		 1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		 1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		-1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		-1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		   														\
		-1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		-1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		-1.0f * _size, -1.0f * _size, -1.0f * _size,			\
        -1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		-1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		-1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		   														\
		 1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		 1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		 1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		 1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		   														\
        -1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		 1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		 1.0f * _size, -1.0f * _size,  1.0f * _size,			\
        -1.0f * _size, -1.0f * _size,  1.0f * _size,			\
		-1.0f * _size, -1.0f * _size, -1.0f * _size,			\
		   														\
		-1.0f * _size,  1.0f * _size, -1.0f * _size,			\
         1.0f * _size,  1.0f * _size,  1.0f * _size,			\
		 1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		 1.0f * _size,  1.0f * _size,  1.0f * _size,			\
        -1.0f * _size,  1.0f * _size, -1.0f * _size,			\
		-1.0f * _size,  1.0f * _size,  1.0f * _size,			\
	}, _vertex_count * 3 * sizeof(float)); 						\
																\
	float* _normals = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_normals, (float []){								\
		 0.0f,  0.0f, -1.0f,									\
		 0.0f,  0.0f, -1.0f,									\
		 0.0f,  0.0f, -1.0f,									\
		 0.0f,  0.0f, -1.0f,									\
		 0.0f,  0.0f, -1.0f,									\
		 0.0f,  0.0f, -1.0f,									\
                                                                \
		 0.0f,  0.0f,  1.0f,									\
		 0.0f,  0.0f,  1.0f,									\
		 0.0f,  0.0f,  1.0f,									\
		 0.0f,  0.0f,  1.0f,									\
		 0.0f,  0.0f,  1.0f,									\
		 0.0f,  0.0f,  1.0f,									\
                                                                \
		-1.0f,  0.0f,  0.0f,									\
		-1.0f,  0.0f,  0.0f,									\
		-1.0f,  0.0f,  0.0f,									\
        -1.0f,  0.0f,  0.0f,									\
        -1.0f,  0.0f,  0.0f,									\
        -1.0f,  0.0f,  0.0f,									\
                                                                \
		 1.0f,  0.0f,  0.0f,									\
		 1.0f,  0.0f,  0.0f,									\
		 1.0f,  0.0f,  0.0f,									\
		 1.0f,  0.0f,  0.0f,									\
		 1.0f,  0.0f,  0.0f,									\
		 1.0f,  0.0f,  0.0f,									\
                                                                \
		 0.0f, -1.0f,  0.0f,									\
		 0.0f, -1.0f,  0.0f,									\
		 0.0f, -1.0f,  0.0f,									\
		 1.0f, -1.0f,  0.0f,									\
		 1.0f, -1.0f,  0.0f,									\
		 1.0f, -1.0f,  0.0f,									\
                                                                \
		 0.0f,  1.0f,  0.0f,									\
		 0.0f,  1.0f,  0.0f,									\
		 0.0f,  1.0f,  0.0f,									\
		 0.0f,  1.0f,  0.0f,									\
		 0.0f,  1.0f,  0.0f,									\
		 0.0f,  1.0f,  0.0f		 								\
	}, _vertex_count * 3 * sizeof(float));									


#define TRIANGULAR_PYRAMID_MESH_DATA(_base, _height) 		\
	const size_t _vertex_count = 12;						\
	float* _positions = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_positions, (float []){							\
	     0.0f * _base,  0.0f * _base,   1.0f * _base,		\
	     1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
	    -1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
	        												\
		 0.0f * _base,  0.0f * _base,   1.0f * _base,		\
		 1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
		 0.0f * _base,  1.0f * _height, 0.0f * _base,		\
		    												\
	     1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
		-1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
		 0.0f * _base,  1.0f * _height, 0.0f * _base,		\
															\
	    -1.0f * _base,  0.0f * _base,  -1.0f * _base,		\
		 0.0f * _base,  0.0f * _base,   1.0f * _base,		\
		 0.0f * _base,  1.0f * _height, 0.0f * _base,		\
	}, _vertex_count * 3 * sizeof(float)); 					\
															\
	float* _normals = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_normals, (float []){					   		\
	     0.0f,  0.0f, -1.0f,                                \
         0.0f,  0.0f, -1.0f,                                \
         0.0f,  0.0f, -1.0f,                                \
                                                            \
         0.0f,  1.0f,  1.0f,                                \
         0.0f,  1.0f,  1.0f,                                \
         0.0f,  1.0f,  1.0f,                                \
                                                            \
        -1.0f,  0.0f,  1.0f,                                \
        -1.0f,  0.0f,  1.0f,                                \
        -1.0f,  0.0f,  1.0f,                                \
                                                            \
         0.0f, -1.0f,  1.0f,                                \
         0.0f, -1.0f,  1.0f,                                \
         0.0f, -1.0f,  1.0f,                                \
	}, _vertex_count * 3 * sizeof(float));					


#define PYRAMID_MESH_DATA(_base_w, _base_d, _height) 			\
	const size_t _vertex_count = 18;							\
	float* _positions = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_positions, (float []){					 			\
		-1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,      \
		-1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,      \
		1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,       \
																\
		1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,       \
		1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,       \
		-1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,      \
																\
		1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,       \
		1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,       \
		0.0f * _base_w,  1.0f * _height,  0.0f * _base_d,       \
																\
		1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,       \
		-1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,      \
		0.0f * _base_w,  1.0f * _height,  0.0f * _base_d,       \
																\
		-1.0f * _base_w,  0.0f * _height, -1.0f * _base_d,      \
		-1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,      \
		0.0f * _base_w,  1.0f * _height,  0.0f * _base_d,       \
																\
		-1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,      \
		1.0f * _base_w,  0.0f * _height,  1.0f * _base_d,       \
		0.0f * _base_w,  1.0f * _height,  0.0f * _base_d,       \
	}, _vertex_count * 3 * sizeof(float)); 						\
																\
	float* _normals = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_normals, (float []){					   			\
              0.0f,  0.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f, -1.0f,      \
              0.0f,  0.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f, -1.0f,      \
              0.0f,  1.0f,  1.0f,    0.0f,  1.0f,  1.0f,     0.0f,  1.0f,  1.0f,      \
             -1.0f,  0.0f,  1.0f,   -1.0f,  0.0f,  1.0f,    -1.0f,  0.0f,  1.0f,      \
              0.0f, -1.0f,  1.0f,    0.0f, -1.0f,  1.0f,     0.0f, -1.0f,  1.0f,      \
              1.0f,  0.0f,  1.0f,    1.0f,  0.0f,  1.0f,     1.0f,  0.0f,  1.0f,      \
	}, _vertex_count * 3 * sizeof(float));									

#define PRISM_MESH_DATA(_width, _height, _depth)				\
	const size_t _vertex_count = 36;							\
	float* _positions = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_positions, (float []){								\
	       1.0f  * _width, -1.0f * _height,  1.0f * _depth,	\
	       -1.0f * _width, -1.0f * _height,  1.0f * _depth,	\
	       -1.0f * _width, -1.0f * _height, -1.0f * _depth,	\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,	\
	       -1.0f * _width,  1.0f * _height,  1.0f * _depth,	\
	       1.0f  * _width,  1.0f * _height,  1.0f * _depth,	\
	       														\
	       1.0f  * _width,  1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width,  1.0f * _height,  1.0f * _depth,		\
	       1.0f  * _width, -1.0f * _height,  1.0f * _depth,		\
	       1.0f  * _width,  1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width, -1.0f * _height,  1.0f * _depth,		\
	       														\
	       -1.0f * _width, -1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width, -1.0f * _height, -1.0f * _depth,		\
	       -1.0f * _width, -1.0f * _height, -1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,		\
	       														\
	       1.0f  * _width, -1.0f * _height, -1.0f * _depth,	\
	       1.0f  * _width, -1.0f * _height,  1.0f * _depth,	\
	       -1.0f * _width, -1.0f * _height, -1.0f * _depth,	\
	       1.0f  * _width,  1.0f * _height, -1.0f * _depth,	\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,	\
	       1.0f  * _width,  1.0f * _height,  1.0f * _depth,	\
	       														\
	       1.0f  * _width, -1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width,  1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width, -1.0f * _height,  1.0f * _depth,		\
	       1.0f  * _width, -1.0f * _height,  1.0f * _depth,		\
	       1.0f  * _width,  1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width, -1.0f * _height,  1.0f * _depth,		\
	       														\
	       -1.0f * _width, -1.0f * _height, -1.0f * _depth,		\
	       -1.0f * _width, -1.0f * _height,  1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width,  1.0f * _height, -1.0f * _depth,		\
	       1.0f  * _width, -1.0f * _height, -1.0f * _depth,		\
	       -1.0f * _width,  1.0f * _height, -1.0f * _depth,		\
	}, _vertex_count * 3 * sizeof(float)); 						\
																\
	float* _normals = (float*)malloc(sizeof(float) * _vertex_count * 3);\
	memcpy(_normals, (float []){								\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       0.0f,  0.0f, -1.0f,									\
	       0.0f,  0.0f, -1.0f,									\
	       0.0f,  0.0f, -1.0f,									\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f, -1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       0.0f,  1.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       1.0f,  0.0f,  0.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -0.0f,  0.0f,  1.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       -1.0f, -0.0f, -0.0f,									\
	       0.0f,  0.0f, -1.0f,									\
	       0.0f,  0.0f, -1.0f,									\
	       0.0f,  0.0f, -1.0f		 							\
	}, _vertex_count * 3 * sizeof(float));

#define SPHERE_MESH_DATA(_size) 											\
	const size_t _vertex_count = 2880;										\
	float* _positions = (float*)malloc(sizeof(float) * _vertex_count * 3);  \
		memcpy(_positions, (float []) {										\
		 0.000000 * _size, 1.000000 * _size, -0.000000 * _size,             \
        -0.000000 * _size, 0.980785 * _size, -0.195090 * _size,             \
        -0.038060 * _size, 0.980785 * _size, -0.191342 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.038060 * _size, 0.980785 * _size, -0.191342 * _size,             \
        -0.074658 * _size, 0.980785 * _size, -0.180240 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.074658 * _size, 0.980785 * _size, -0.180240 * _size,             \
        -0.108386 * _size, 0.980785 * _size, -0.162212 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.108386 * _size, 0.980785 * _size, -0.162212 * _size,             \
        -0.137950 * _size, 0.980785 * _size, -0.137950 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.137950 * _size, 0.980785 * _size, -0.137950 * _size,             \
        -0.162212 * _size, 0.980785 * _size, -0.108386 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.162212 * _size, 0.980785 * _size, -0.108386 * _size,             \
        -0.180240 * _size, 0.980785 * _size, -0.074658 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.180240 * _size, 0.980785 * _size, -0.074658 * _size,             \
        -0.191342 * _size, 0.980785 * _size, -0.038060 * _size,             \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.191342 * _size, 0.980785 * _size, -0.038060 * _size,             \
        -0.195090 * _size, 0.980785 * _size, 0.000000 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.195090 * _size, 0.980785 * _size, 0.000000 * _size,          \
        -0.191342 * _size, 0.980785 * _size, 0.038060 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.191342 * _size, 0.980785 * _size, 0.038060 * _size,          \
        -0.180240 * _size, 0.980785 * _size, 0.074658 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.180240 * _size, 0.980785 * _size, 0.074658 * _size,          \
        -0.162212 * _size, 0.980785 * _size, 0.108386 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.162212 * _size, 0.980785 * _size, 0.108386 * _size,          \
        -0.137950 * _size, 0.980785 * _size, 0.137950 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.137950 * _size, 0.980785 * _size, 0.137950 * _size,          \
        -0.108386 * _size, 0.980785 * _size, 0.162212 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.108386 * _size, 0.980785 * _size, 0.162212 * _size,          \
        -0.074658 * _size, 0.980785 * _size, 0.180240 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.074658 * _size, 0.980785 * _size, 0.180240 * _size,          \
        -0.038060 * _size, 0.980785 * _size, 0.191342 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        -0.038060 * _size, 0.980785 * _size, 0.191342 * _size,          \
        0.000000 * _size, 0.980785 * _size, 0.195090 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.000000 * _size, 0.980785 * _size, 0.195090 * _size,           \
        0.038060 * _size, 0.980785 * _size, 0.191342 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.038060 * _size, 0.980785 * _size, 0.191342 * _size,           \
        0.074658 * _size, 0.980785 * _size, 0.180240 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.074658 * _size, 0.980785 * _size, 0.180240 * _size,           \
        0.108387 * _size, 0.980785 * _size, 0.162212 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.108387 * _size, 0.980785 * _size, 0.162212 * _size,           \
        0.137950 * _size, 0.980785 * _size, 0.137950 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.137950 * _size, 0.980785 * _size, 0.137950 * _size,           \
        0.162212 * _size, 0.980785 * _size, 0.108386 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.162212 * _size, 0.980785 * _size, 0.108386 * _size,           \
        0.180240 * _size, 0.980785 * _size, 0.074658 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.180240 * _size, 0.980785 * _size, 0.074658 * _size,           \
        0.191342 * _size, 0.980785 * _size, 0.038060 * _size,           \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.191342 * _size, 0.980785 * _size, 0.038060 * _size,           \
        0.195090 * _size, 0.980785 * _size, -0.000000 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.195090 * _size, 0.980785 * _size, -0.000000 * _size,          \
        0.191342 * _size, 0.980785 * _size, -0.038061 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.191342 * _size, 0.980785 * _size, -0.038061 * _size,          \
        0.180240 * _size, 0.980785 * _size, -0.074658 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.180240 * _size, 0.980785 * _size, -0.074658 * _size,          \
        0.162211 * _size, 0.980785 * _size, -0.108387 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.162211 * _size, 0.980785 * _size, -0.108387 * _size,          \
        0.137949 * _size, 0.980785 * _size, -0.137950 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.137949 * _size, 0.980785 * _size, -0.137950 * _size,          \
        0.108386 * _size, 0.980785 * _size, -0.162212 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.108386 * _size, 0.980785 * _size, -0.162212 * _size,          \
        0.074657 * _size, 0.980785 * _size, -0.180240 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.074657 * _size, 0.980785 * _size, -0.180240 * _size,          \
        0.038060 * _size, 0.980785 * _size, -0.191342 * _size,          \
        0.000000 * _size, 1.000000 * _size, -0.000000 * _size,          \
        0.038060 * _size, 0.980785 * _size, -0.191342 * _size,          \
        -0.000000 * _size, 0.980785 * _size, -0.195090 * _size,             \
        -0.000000 * _size, 0.980785 * _size, -0.195090 * _size,             \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.000000 * _size, 0.980785 * _size, -0.195090 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.038060 * _size, 0.980785 * _size, -0.191342 * _size,             \
        -0.038060 * _size, 0.980785 * _size, -0.191342 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.038060 * _size, 0.980785 * _size, -0.191342 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.074658 * _size, 0.980785 * _size, -0.180240 * _size,             \
        -0.074658 * _size, 0.980785 * _size, -0.180240 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.074658 * _size, 0.980785 * _size, -0.180240 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.108386 * _size, 0.980785 * _size, -0.162212 * _size,             \
        -0.108386 * _size, 0.980785 * _size, -0.162212 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.108386 * _size, 0.980785 * _size, -0.162212 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.137950 * _size, 0.980785 * _size, -0.137950 * _size,             \
        -0.137950 * _size, 0.980785 * _size, -0.137950 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.137950 * _size, 0.980785 * _size, -0.137950 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.162212 * _size, 0.980785 * _size, -0.108386 * _size,             \
        -0.162212 * _size, 0.980785 * _size, -0.108386 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.162212 * _size, 0.980785 * _size, -0.108386 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.180240 * _size, 0.980785 * _size, -0.074658 * _size,             \
        -0.180240 * _size, 0.980785 * _size, -0.074658 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.180240 * _size, 0.980785 * _size, -0.074658 * _size,             \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.191342 * _size, 0.980785 * _size, -0.038060 * _size,             \
        -0.191342 * _size, 0.980785 * _size, -0.038060 * _size,             \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.191342 * _size, 0.980785 * _size, -0.038060 * _size,             \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.195090 * _size, 0.980785 * _size, 0.000000 * _size,          \
        -0.195090 * _size, 0.980785 * _size, 0.000000 * _size,          \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.195090 * _size, 0.980785 * _size, 0.000000 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.191342 * _size, 0.980785 * _size, 0.038060 * _size,          \
        -0.191342 * _size, 0.980785 * _size, 0.038060 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.191342 * _size, 0.980785 * _size, 0.038060 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.180240 * _size, 0.980785 * _size, 0.074658 * _size,          \
        -0.180240 * _size, 0.980785 * _size, 0.074658 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.180240 * _size, 0.980785 * _size, 0.074658 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.162212 * _size, 0.980785 * _size, 0.108386 * _size,          \
        -0.162212 * _size, 0.980785 * _size, 0.108386 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.162212 * _size, 0.980785 * _size, 0.108386 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.137950 * _size, 0.980785 * _size, 0.137950 * _size,          \
        -0.137950 * _size, 0.980785 * _size, 0.137950 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.137950 * _size, 0.980785 * _size, 0.137950 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.108386 * _size, 0.980785 * _size, 0.162212 * _size,          \
        -0.108386 * _size, 0.980785 * _size, 0.162212 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.108386 * _size, 0.980785 * _size, 0.162212 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.074658 * _size, 0.980785 * _size, 0.180240 * _size,          \
        -0.074658 * _size, 0.980785 * _size, 0.180240 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        -0.074658 * _size, 0.980785 * _size, 0.180240 * _size,          \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        -0.038060 * _size, 0.980785 * _size, 0.191342 * _size,          \
        -0.038060 * _size, 0.980785 * _size, 0.191342 * _size,          \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        -0.038060 * _size, 0.980785 * _size, 0.191342 * _size,          \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        0.000000 * _size, 0.980785 * _size, 0.195090 * _size,           \
        0.000000 * _size, 0.980785 * _size, 0.195090 * _size,           \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.000000 * _size, 0.980785 * _size, 0.195090 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.038060 * _size, 0.980785 * _size, 0.191342 * _size,           \
        0.038060 * _size, 0.980785 * _size, 0.191342 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.038060 * _size, 0.980785 * _size, 0.191342 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.074658 * _size, 0.980785 * _size, 0.180240 * _size,           \
        0.074658 * _size, 0.980785 * _size, 0.180240 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.074658 * _size, 0.980785 * _size, 0.180240 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.108387 * _size, 0.980785 * _size, 0.162212 * _size,           \
        0.108387 * _size, 0.980785 * _size, 0.162212 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.108387 * _size, 0.980785 * _size, 0.162212 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.137950 * _size, 0.980785 * _size, 0.137950 * _size,           \
        0.137950 * _size, 0.980785 * _size, 0.137950 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.137950 * _size, 0.980785 * _size, 0.137950 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.162212 * _size, 0.980785 * _size, 0.108386 * _size,           \
        0.162212 * _size, 0.980785 * _size, 0.108386 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.162212 * _size, 0.980785 * _size, 0.108386 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.180240 * _size, 0.980785 * _size, 0.074658 * _size,           \
        0.180240 * _size, 0.980785 * _size, 0.074658 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.180240 * _size, 0.980785 * _size, 0.074658 * _size,           \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.191342 * _size, 0.980785 * _size, 0.038060 * _size,           \
        0.191342 * _size, 0.980785 * _size, 0.038060 * _size,           \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.191342 * _size, 0.980785 * _size, 0.038060 * _size,           \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.195090 * _size, 0.980785 * _size, -0.000000 * _size,          \
        0.195090 * _size, 0.980785 * _size, -0.000000 * _size,          \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.195090 * _size, 0.980785 * _size, -0.000000 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.191342 * _size, 0.980785 * _size, -0.038061 * _size,          \
        0.191342 * _size, 0.980785 * _size, -0.038061 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.191342 * _size, 0.980785 * _size, -0.038061 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.180240 * _size, 0.980785 * _size, -0.074658 * _size,          \
        0.180240 * _size, 0.980785 * _size, -0.074658 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.180240 * _size, 0.980785 * _size, -0.074658 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.162211 * _size, 0.980785 * _size, -0.108387 * _size,          \
        0.162211 * _size, 0.980785 * _size, -0.108387 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.162211 * _size, 0.980785 * _size, -0.108387 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.137949 * _size, 0.980785 * _size, -0.137950 * _size,          \
        0.137949 * _size, 0.980785 * _size, -0.137950 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.137949 * _size, 0.980785 * _size, -0.137950 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.108386 * _size, 0.980785 * _size, -0.162212 * _size,          \
        0.108386 * _size, 0.980785 * _size, -0.162212 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.108386 * _size, 0.980785 * _size, -0.162212 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.074657 * _size, 0.980785 * _size, -0.180240 * _size,          \
        0.074657 * _size, 0.980785 * _size, -0.180240 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        0.074657 * _size, 0.980785 * _size, -0.180240 * _size,          \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        0.038060 * _size, 0.980785 * _size, -0.191342 * _size,          \
        0.038060 * _size, 0.980785 * _size, -0.191342 * _size,          \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        0.038060 * _size, 0.980785 * _size, -0.191342 * _size,          \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        -0.000000 * _size, 0.980785 * _size, -0.195090 * _size,             \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.074658 * _size, 0.923880 * _size, -0.375330 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.146447 * _size, 0.923880 * _size, -0.353553 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.212607 * _size, 0.923880 * _size, -0.318190 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.270598 * _size, 0.923880 * _size, -0.270598 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.318190 * _size, 0.923880 * _size, -0.212607 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.353553 * _size, 0.923880 * _size, -0.146447 * _size,             \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.375330 * _size, 0.923880 * _size, -0.074658 * _size,             \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.382683 * _size, 0.923880 * _size, 0.000000 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.375330 * _size, 0.923880 * _size, 0.074658 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.353553 * _size, 0.923880 * _size, 0.146447 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.318190 * _size, 0.923880 * _size, 0.212608 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.270598 * _size, 0.923880 * _size, 0.270598 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.212607 * _size, 0.923880 * _size, 0.318190 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        -0.146446 * _size, 0.923880 * _size, 0.353554 * _size,          \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        -0.074657 * _size, 0.923880 * _size, 0.375330 * _size,          \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.000000 * _size, 0.923880 * _size, 0.382683 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.074658 * _size, 0.923880 * _size, 0.375330 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.146447 * _size, 0.923880 * _size, 0.353553 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.212608 * _size, 0.923880 * _size, 0.318189 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.270598 * _size, 0.923880 * _size, 0.270598 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.318190 * _size, 0.923880 * _size, 0.212607 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.353554 * _size, 0.923880 * _size, 0.146446 * _size,           \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.375330 * _size, 0.923880 * _size, 0.074657 * _size,           \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.382683 * _size, 0.923880 * _size, -0.000001 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.375330 * _size, 0.923880 * _size, -0.074658 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.353553 * _size, 0.923880 * _size, -0.146447 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.318189 * _size, 0.923880 * _size, -0.212608 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.270598 * _size, 0.923880 * _size, -0.270599 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.212607 * _size, 0.923880 * _size, -0.318190 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        0.146446 * _size, 0.923880 * _size, -0.353554 * _size,          \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        0.074657 * _size, 0.923880 * _size, -0.375330 * _size,          \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        -0.000000 * _size, 0.923880 * _size, -0.382683 * _size,             \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.108386 * _size, 0.831470 * _size, -0.544895 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.212607 * _size, 0.831470 * _size, -0.513280 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.308658 * _size, 0.831470 * _size, -0.461940 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.392847 * _size, 0.831470 * _size, -0.392847 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.461940 * _size, 0.831470 * _size, -0.308658 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.513280 * _size, 0.831470 * _size, -0.212607 * _size,             \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.544895 * _size, 0.831470 * _size, -0.108386 * _size,             \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.555570 * _size, 0.831470 * _size, 0.000000 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.544895 * _size, 0.831470 * _size, 0.108387 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.513280 * _size, 0.831470 * _size, 0.212608 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.461940 * _size, 0.831470 * _size, 0.308659 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.392847 * _size, 0.831470 * _size, 0.392848 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.308658 * _size, 0.831470 * _size, 0.461940 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        -0.212607 * _size, 0.831470 * _size, 0.513280 * _size,          \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        -0.108386 * _size, 0.831470 * _size, 0.544895 * _size,          \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.000001 * _size, 0.831470 * _size, 0.555570 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.108387 * _size, 0.831470 * _size, 0.544895 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.212608 * _size, 0.831470 * _size, 0.513280 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.308659 * _size, 0.831470 * _size, 0.461939 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.392848 * _size, 0.831470 * _size, 0.392847 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.461940 * _size, 0.831470 * _size, 0.308658 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.513280 * _size, 0.831470 * _size, 0.212607 * _size,           \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.544895 * _size, 0.831470 * _size, 0.108386 * _size,           \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.555570 * _size, 0.831470 * _size, -0.000001 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.544895 * _size, 0.831470 * _size, -0.108387 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.513280 * _size, 0.831470 * _size, -0.212608 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.461939 * _size, 0.831470 * _size, -0.308659 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.392847 * _size, 0.831470 * _size, -0.392848 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.308657 * _size, 0.831470 * _size, -0.461940 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        0.212606 * _size, 0.831470 * _size, -0.513280 * _size,          \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        0.108385 * _size, 0.831470 * _size, -0.544895 * _size,          \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        -0.000000 * _size, 0.831470 * _size, -0.555570 * _size,             \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.137950 * _size, 0.707107 * _size, -0.693520 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.270598 * _size, 0.707107 * _size, -0.653282 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.392847 * _size, 0.707107 * _size, -0.587938 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.500000 * _size, 0.707107 * _size, -0.500000 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.587938 * _size, 0.707107 * _size, -0.392847 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.653282 * _size, 0.707107 * _size, -0.270598 * _size,             \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.693520 * _size, 0.707107 * _size, -0.137950 * _size,             \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.707107 * _size, 0.707107 * _size, 0.000000 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.693520 * _size, 0.707107 * _size, 0.137950 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.653281 * _size, 0.707107 * _size, 0.270598 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.587938 * _size, 0.707107 * _size, 0.392848 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.500000 * _size, 0.707107 * _size, 0.500000 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.392847 * _size, 0.707107 * _size, 0.587938 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        -0.270598 * _size, 0.707107 * _size, 0.653282 * _size,          \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        -0.137949 * _size, 0.707107 * _size, 0.693520 * _size,          \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.000001 * _size, 0.707107 * _size, 0.707107 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.137950 * _size, 0.707107 * _size, 0.693520 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.270599 * _size, 0.707107 * _size, 0.653281 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.392848 * _size, 0.707107 * _size, 0.587937 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.500001 * _size, 0.707107 * _size, 0.499999 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.587938 * _size, 0.707107 * _size, 0.392847 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.653282 * _size, 0.707107 * _size, 0.270597 * _size,           \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.693520 * _size, 0.707107 * _size, 0.137949 * _size,           \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.707107 * _size, 0.707107 * _size, -0.000001 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.693520 * _size, 0.707107 * _size, -0.137951 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.653281 * _size, 0.707107 * _size, -0.270599 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.587937 * _size, 0.707107 * _size, -0.392849 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.499999 * _size, 0.707107 * _size, -0.500001 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.392846 * _size, 0.707107 * _size, -0.587939 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        0.270597 * _size, 0.707107 * _size, -0.653282 * _size,          \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        0.137948 * _size, 0.707107 * _size, -0.693520 * _size,          \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        -0.000000 * _size, 0.707107 * _size, -0.707107 * _size,             \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.162212 * _size, 0.555570 * _size, -0.815493 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.318190 * _size, 0.555570 * _size, -0.768178 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.461940 * _size, 0.555570 * _size, -0.691342 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.587938 * _size, 0.555570 * _size, -0.587938 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.691342 * _size, 0.555570 * _size, -0.461940 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.768178 * _size, 0.555570 * _size, -0.318190 * _size,             \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.815493 * _size, 0.555570 * _size, -0.162211 * _size,             \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.831470 * _size, 0.555570 * _size, 0.000000 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.815493 * _size, 0.555570 * _size, 0.162212 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.768178 * _size, 0.555570 * _size, 0.318190 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.691341 * _size, 0.555570 * _size, 0.461940 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.587937 * _size, 0.555570 * _size, 0.587938 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.461939 * _size, 0.555570 * _size, 0.691342 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        -0.318189 * _size, 0.555570 * _size, 0.768178 * _size,          \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        -0.162211 * _size, 0.555570 * _size, 0.815493 * _size,          \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.000001 * _size, 0.555570 * _size, 0.831470 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.162213 * _size, 0.555570 * _size, 0.815493 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.318190 * _size, 0.555570 * _size, 0.768177 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.461941 * _size, 0.555570 * _size, 0.691341 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.587939 * _size, 0.555570 * _size, 0.587937 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.691342 * _size, 0.555570 * _size, 0.461939 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.768178 * _size, 0.555570 * _size, 0.318189 * _size,           \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.815493 * _size, 0.555570 * _size, 0.162210 * _size,           \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.831470 * _size, 0.555570 * _size, -0.000001 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.815493 * _size, 0.555570 * _size, -0.162213 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.768177 * _size, 0.555570 * _size, -0.318191 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.691341 * _size, 0.555570 * _size, -0.461941 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.587937 * _size, 0.555570 * _size, -0.587939 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.461938 * _size, 0.555570 * _size, -0.691343 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        0.318188 * _size, 0.555570 * _size, -0.768178 * _size,          \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        0.162210 * _size, 0.555570 * _size, -0.815494 * _size,          \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        -0.000000 * _size, 0.555570 * _size, -0.831470 * _size,             \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.180240 * _size, 0.382683 * _size, -0.906127 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.353553 * _size, 0.382683 * _size, -0.853553 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.513280 * _size, 0.382683 * _size, -0.768178 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.653281 * _size, 0.382683 * _size, -0.653281 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.768178 * _size, 0.382683 * _size, -0.513280 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -0.853553 * _size, 0.382683 * _size, -0.353553 * _size,             \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -0.906127 * _size, 0.382683 * _size, -0.180240 * _size,             \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.923880 * _size, 0.382683 * _size, 0.000000 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.906127 * _size, 0.382683 * _size, 0.180240 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.853553 * _size, 0.382683 * _size, 0.353554 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.768178 * _size, 0.382683 * _size, 0.513280 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.653281 * _size, 0.382683 * _size, 0.653282 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.513279 * _size, 0.382683 * _size, 0.768178 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        -0.353553 * _size, 0.382683 * _size, 0.853554 * _size,          \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        -0.180239 * _size, 0.382683 * _size, 0.906128 * _size,          \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.000001 * _size, 0.382683 * _size, 0.923880 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.180241 * _size, 0.382683 * _size, 0.906127 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.353554 * _size, 0.382683 * _size, 0.853553 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.513281 * _size, 0.382683 * _size, 0.768177 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.653282 * _size, 0.382683 * _size, 0.653281 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.768178 * _size, 0.382683 * _size, 0.513279 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        0.853554 * _size, 0.382683 * _size, 0.353552 * _size,           \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        0.906128 * _size, 0.382683 * _size, 0.180239 * _size,           \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.923880 * _size, 0.382683 * _size, -0.000001 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.906127 * _size, 0.382683 * _size, -0.180242 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.853553 * _size, 0.382683 * _size, -0.353555 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.768177 * _size, 0.382683 * _size, -0.513281 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.653280 * _size, 0.382683 * _size, -0.653283 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.513278 * _size, 0.382683 * _size, -0.768179 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        0.353552 * _size, 0.382683 * _size, -0.853554 * _size,          \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        0.180238 * _size, 0.382683 * _size, -0.906128 * _size,          \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        -0.000000 * _size, 0.382683 * _size, -0.923880 * _size,             \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.191342 * _size, 0.195090 * _size, -0.961940 * _size,             \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.375330 * _size, 0.195090 * _size, -0.906127 * _size,             \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.544895 * _size, 0.195090 * _size, -0.815493 * _size,             \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.693520 * _size, 0.195090 * _size, -0.693520 * _size,             \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.815493 * _size, 0.195090 * _size, -0.544895 * _size,             \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -0.906127 * _size, 0.195090 * _size, -0.375330 * _size,             \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -0.961940 * _size, 0.195090 * _size, -0.191341 * _size,             \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.980785 * _size, 0.195090 * _size, 0.000000 * _size,          \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.961940 * _size, 0.195090 * _size, 0.191342 * _size,          \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.906127 * _size, 0.195090 * _size, 0.375331 * _size,          \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.815493 * _size, 0.195090 * _size, 0.544896 * _size,          \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.693519 * _size, 0.195090 * _size, 0.693520 * _size,          \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.544895 * _size, 0.195090 * _size, 0.815494 * _size,          \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        -0.375330 * _size, 0.195090 * _size, 0.906128 * _size,          \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        -0.191341 * _size, 0.195090 * _size, 0.961940 * _size,          \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.000001 * _size, 0.195090 * _size, 0.980785 * _size,           \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.191343 * _size, 0.195090 * _size, 0.961940 * _size,           \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.375331 * _size, 0.195090 * _size, 0.906127 * _size,           \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.544896 * _size, 0.195090 * _size, 0.815492 * _size,           \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.693521 * _size, 0.195090 * _size, 0.693519 * _size,           \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.815494 * _size, 0.195090 * _size, 0.544894 * _size,           \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        0.906128 * _size, 0.195090 * _size, 0.375329 * _size,           \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        0.961940 * _size, 0.195090 * _size, 0.191340 * _size,           \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.980785 * _size, 0.195090 * _size, -0.000002 * _size,          \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.961939 * _size, 0.195090 * _size, -0.191343 * _size,          \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.906127 * _size, 0.195090 * _size, -0.375332 * _size,          \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.815492 * _size, 0.195090 * _size, -0.544897 * _size,          \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.693519 * _size, 0.195090 * _size, -0.693521 * _size,          \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.544893 * _size, 0.195090 * _size, -0.815494 * _size,          \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        0.375328 * _size, 0.195090 * _size, -0.906128 * _size,          \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        0.191340 * _size, 0.195090 * _size, -0.961940 * _size,          \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        -0.000000 * _size, 0.195090 * _size, -0.980785 * _size,             \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.195090 * _size, 0.000000 * _size, -0.980785 * _size,             \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.382683 * _size, 0.000000 * _size, -0.923880 * _size,             \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.555570 * _size, 0.000000 * _size, -0.831470 * _size,             \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.707107 * _size, 0.000000 * _size, -0.707107 * _size,             \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.831470 * _size, 0.000000 * _size, -0.555570 * _size,             \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.923880 * _size, 0.000000 * _size, -0.382683 * _size,             \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -0.980785 * _size, 0.000000 * _size, -0.195090 * _size,             \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -1.000000 * _size, 0.000000 * _size, 0.000000 * _size,          \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.980785 * _size, 0.000000 * _size, 0.195091 * _size,          \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.923879 * _size, 0.000000 * _size, 0.382684 * _size,          \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.831469 * _size, 0.000000 * _size, 0.555571 * _size,          \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.707106 * _size, 0.000000 * _size, 0.707107 * _size,          \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.555570 * _size, 0.000000 * _size, 0.831470 * _size,          \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        -0.382683 * _size, 0.000000 * _size, 0.923880 * _size,          \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        -0.195089 * _size, 0.000000 * _size, 0.980785 * _size,          \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.000001 * _size, 0.000000 * _size, 1.000000 * _size,           \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.195091 * _size, 0.000000 * _size, 0.980785 * _size,           \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.382684 * _size, 0.000000 * _size, 0.923879 * _size,           \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.555571 * _size, 0.000000 * _size, 0.831469 * _size,           \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.707108 * _size, 0.000000 * _size, 0.707106 * _size,           \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.831470 * _size, 0.000000 * _size, 0.555569 * _size,           \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.923880 * _size, 0.000000 * _size, 0.382682 * _size,           \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        0.980786 * _size, 0.000000 * _size, 0.195089 * _size,           \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        1.000000 * _size, 0.000000 * _size, -0.000002 * _size,          \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.980785 * _size, 0.000000 * _size, -0.195092 * _size,          \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.923879 * _size, 0.000000 * _size, -0.382685 * _size,          \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.831469 * _size, 0.000000 * _size, -0.555572 * _size,          \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.707105 * _size, 0.000000 * _size, -0.707108 * _size,          \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.555569 * _size, 0.000000 * _size, -0.831471 * _size,          \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        0.382681 * _size, 0.000000 * _size, -0.923880 * _size,          \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        0.195088 * _size, 0.000000 * _size, -0.980786 * _size,          \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        -0.000000 * _size, 0.000000 * _size, -1.000000 * _size,             \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.191342 * _size, -0.195090 * _size, -0.961940 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.375330 * _size, -0.195090 * _size, -0.906127 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.544895 * _size, -0.195090 * _size, -0.815493 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.693520 * _size, -0.195090 * _size, -0.693520 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.815493 * _size, -0.195090 * _size, -0.544895 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.906128 * _size, -0.195090 * _size, -0.375330 * _size,            \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.961940 * _size, -0.195090 * _size, -0.191341 * _size,            \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.980785 * _size, -0.195090 * _size, 0.000000 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.961940 * _size, -0.195090 * _size, 0.191342 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.906127 * _size, -0.195090 * _size, 0.375331 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.815493 * _size, -0.195090 * _size, 0.544896 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.693519 * _size, -0.195090 * _size, 0.693520 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.544895 * _size, -0.195090 * _size, 0.815494 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        -0.375330 * _size, -0.195090 * _size, 0.906128 * _size,             \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        -0.191341 * _size, -0.195090 * _size, 0.961940 * _size,             \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.000001 * _size, -0.195090 * _size, 0.980785 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.191343 * _size, -0.195090 * _size, 0.961940 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.375331 * _size, -0.195090 * _size, 0.906127 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.544896 * _size, -0.195090 * _size, 0.815492 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.693521 * _size, -0.195090 * _size, 0.693519 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.815494 * _size, -0.195090 * _size, 0.544894 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.906128 * _size, -0.195090 * _size, 0.375329 * _size,          \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.961940 * _size, -0.195090 * _size, 0.191340 * _size,          \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.980785 * _size, -0.195090 * _size, -0.000002 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.961939 * _size, -0.195090 * _size, -0.191343 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.906127 * _size, -0.195090 * _size, -0.375332 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.815492 * _size, -0.195090 * _size, -0.544897 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.693519 * _size, -0.195090 * _size, -0.693521 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.544894 * _size, -0.195090 * _size, -0.815494 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        0.375328 * _size, -0.195090 * _size, -0.906128 * _size,             \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        0.191340 * _size, -0.195090 * _size, -0.961940 * _size,             \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        -0.000000 * _size, -0.195090 * _size, -0.980785 * _size,            \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.180240 * _size, -0.382683 * _size, -0.906127 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.353553 * _size, -0.382683 * _size, -0.853553 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.513280 * _size, -0.382683 * _size, -0.768178 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.653282 * _size, -0.382683 * _size, -0.653282 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.768178 * _size, -0.382683 * _size, -0.513280 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.853553 * _size, -0.382683 * _size, -0.353553 * _size,            \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.906128 * _size, -0.382683 * _size, -0.180240 * _size,            \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.923880 * _size, -0.382683 * _size, 0.000000 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.906127 * _size, -0.382683 * _size, 0.180240 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.853553 * _size, -0.382683 * _size, 0.353554 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.768178 * _size, -0.382683 * _size, 0.513280 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.653281 * _size, -0.382683 * _size, 0.653282 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.513279 * _size, -0.382683 * _size, 0.768178 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        -0.353553 * _size, -0.382683 * _size, 0.853554 * _size,             \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        -0.180239 * _size, -0.382683 * _size, 0.906128 * _size,             \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.000001 * _size, -0.382683 * _size, 0.923880 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.180241 * _size, -0.382683 * _size, 0.906127 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.353554 * _size, -0.382683 * _size, 0.853553 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.513281 * _size, -0.382683 * _size, 0.768177 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.653282 * _size, -0.382683 * _size, 0.653281 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.768179 * _size, -0.382683 * _size, 0.513279 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.853554 * _size, -0.382683 * _size, 0.353552 * _size,          \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.906128 * _size, -0.382683 * _size, 0.180239 * _size,          \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.923880 * _size, -0.382683 * _size, -0.000001 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.906127 * _size, -0.382683 * _size, -0.180242 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.853553 * _size, -0.382683 * _size, -0.353555 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.768177 * _size, -0.382683 * _size, -0.513281 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.653280 * _size, -0.382683 * _size, -0.653283 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.513278 * _size, -0.382683 * _size, -0.768179 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        0.353552 * _size, -0.382683 * _size, -0.853554 * _size,             \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        0.180238 * _size, -0.382683 * _size, -0.906128 * _size,             \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        -0.000000 * _size, -0.382683 * _size, -0.923880 * _size,            \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.162212 * _size, -0.555570 * _size, -0.815493 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.318190 * _size, -0.555570 * _size, -0.768178 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.461940 * _size, -0.555570 * _size, -0.691342 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.587938 * _size, -0.555570 * _size, -0.587938 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.691342 * _size, -0.555570 * _size, -0.461940 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.768178 * _size, -0.555570 * _size, -0.318190 * _size,            \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.815493 * _size, -0.555570 * _size, -0.162211 * _size,            \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.831470 * _size, -0.555570 * _size, 0.000000 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.815493 * _size, -0.555570 * _size, 0.162212 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.768178 * _size, -0.555570 * _size, 0.318190 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.691342 * _size, -0.555570 * _size, 0.461940 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.587937 * _size, -0.555570 * _size, 0.587938 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.461939 * _size, -0.555570 * _size, 0.691342 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        -0.318189 * _size, -0.555570 * _size, 0.768178 * _size,             \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        -0.162211 * _size, -0.555570 * _size, 0.815493 * _size,             \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.000001 * _size, -0.555570 * _size, 0.831470 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.162213 * _size, -0.555570 * _size, 0.815493 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.318191 * _size, -0.555570 * _size, 0.768177 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.461941 * _size, -0.555570 * _size, 0.691341 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.587939 * _size, -0.555570 * _size, 0.587937 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.691342 * _size, -0.555570 * _size, 0.461939 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.768178 * _size, -0.555570 * _size, 0.318189 * _size,          \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.815493 * _size, -0.555570 * _size, 0.162210 * _size,          \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.831470 * _size, -0.555570 * _size, -0.000001 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.815493 * _size, -0.555570 * _size, -0.162213 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.768177 * _size, -0.555570 * _size, -0.318191 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.691341 * _size, -0.555570 * _size, -0.461941 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.587937 * _size, -0.555570 * _size, -0.587939 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.461938 * _size, -0.555570 * _size, -0.691343 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        0.318188 * _size, -0.555570 * _size, -0.768178 * _size,             \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        0.162210 * _size, -0.555570 * _size, -0.815494 * _size,             \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        -0.000000 * _size, -0.555570 * _size, -0.831470 * _size,            \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.137950 * _size, -0.707107 * _size, -0.693520 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.270598 * _size, -0.707107 * _size, -0.653281 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.392847 * _size, -0.707107 * _size, -0.587938 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.500000 * _size, -0.707107 * _size, -0.500000 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.587938 * _size, -0.707107 * _size, -0.392847 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.653282 * _size, -0.707107 * _size, -0.270598 * _size,            \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.693520 * _size, -0.707107 * _size, -0.137950 * _size,            \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.707107 * _size, -0.707107 * _size, 0.000000 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.693520 * _size, -0.707107 * _size, 0.137950 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.653281 * _size, -0.707107 * _size, 0.270598 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.587938 * _size, -0.707107 * _size, 0.392848 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.500000 * _size, -0.707107 * _size, 0.500000 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.392847 * _size, -0.707107 * _size, 0.587938 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        -0.270598 * _size, -0.707107 * _size, 0.653282 * _size,             \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        -0.137949 * _size, -0.707107 * _size, 0.693520 * _size,             \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.000001 * _size, -0.707107 * _size, 0.707107 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.137950 * _size, -0.707107 * _size, 0.693520 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.270599 * _size, -0.707107 * _size, 0.653281 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.392848 * _size, -0.707107 * _size, 0.587937 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.500001 * _size, -0.707107 * _size, 0.499999 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.587938 * _size, -0.707107 * _size, 0.392847 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.653282 * _size, -0.707107 * _size, 0.270597 * _size,          \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.693520 * _size, -0.707107 * _size, 0.137949 * _size,          \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.707107 * _size, -0.707107 * _size, -0.000001 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.693520 * _size, -0.707107 * _size, -0.137951 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.653281 * _size, -0.707107 * _size, -0.270599 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.587937 * _size, -0.707107 * _size, -0.392849 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.499999 * _size, -0.707107 * _size, -0.500001 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.392846 * _size, -0.707107 * _size, -0.587939 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        0.270597 * _size, -0.707107 * _size, -0.653282 * _size,             \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        0.137948 * _size, -0.707107 * _size, -0.693520 * _size,             \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        -0.000000 * _size, -0.707107 * _size, -0.707107 * _size,            \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.108386 * _size, -0.831470 * _size, -0.544895 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.212607 * _size, -0.831470 * _size, -0.513280 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.308658 * _size, -0.831470 * _size, -0.461940 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.392847 * _size, -0.831470 * _size, -0.392847 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.461940 * _size, -0.831470 * _size, -0.308658 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.513280 * _size, -0.831470 * _size, -0.212607 * _size,            \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.544895 * _size, -0.831470 * _size, -0.108386 * _size,            \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.555570 * _size, -0.831470 * _size, 0.000000 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.544895 * _size, -0.831470 * _size, 0.108387 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.513280 * _size, -0.831470 * _size, 0.212608 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.461940 * _size, -0.831470 * _size, 0.308659 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.392847 * _size, -0.831470 * _size, 0.392848 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.308658 * _size, -0.831470 * _size, 0.461940 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        -0.212607 * _size, -0.831470 * _size, 0.513280 * _size,             \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        -0.108386 * _size, -0.831470 * _size, 0.544895 * _size,             \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.000001 * _size, -0.831470 * _size, 0.555570 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.108387 * _size, -0.831470 * _size, 0.544895 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.212608 * _size, -0.831470 * _size, 0.513280 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.308659 * _size, -0.831470 * _size, 0.461939 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.392848 * _size, -0.831470 * _size, 0.392847 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.461940 * _size, -0.831470 * _size, 0.308658 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.513280 * _size, -0.831470 * _size, 0.212607 * _size,          \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.544895 * _size, -0.831470 * _size, 0.108386 * _size,          \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.555570 * _size, -0.831470 * _size, -0.000001 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.544895 * _size, -0.831470 * _size, -0.108387 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.513280 * _size, -0.831470 * _size, -0.212608 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.461939 * _size, -0.831470 * _size, -0.308659 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.392847 * _size, -0.831470 * _size, -0.392848 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.308657 * _size, -0.831470 * _size, -0.461940 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        0.212606 * _size, -0.831470 * _size, -0.513280 * _size,             \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        0.108385 * _size, -0.831470 * _size, -0.544895 * _size,             \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        -0.000000 * _size, -0.831470 * _size, -0.555570 * _size,            \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        -0.000000 * _size, -0.980785 * _size, -0.195090 * _size,            \
        -0.038060 * _size, -0.980785 * _size, -0.191341 * _size,            \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        -0.038060 * _size, -0.980785 * _size, -0.191341 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.038060 * _size, -0.980785 * _size, -0.191341 * _size,            \
        -0.074658 * _size, -0.980785 * _size, -0.180240 * _size,            \
        -0.074658 * _size, -0.923880 * _size, -0.375330 * _size,            \
        -0.074658 * _size, -0.980785 * _size, -0.180240 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.074658 * _size, -0.980785 * _size, -0.180240 * _size,            \
        -0.108386 * _size, -0.980785 * _size, -0.162211 * _size,            \
        -0.146447 * _size, -0.923880 * _size, -0.353553 * _size,            \
        -0.108386 * _size, -0.980785 * _size, -0.162211 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.108386 * _size, -0.980785 * _size, -0.162211 * _size,            \
        -0.137950 * _size, -0.980785 * _size, -0.137950 * _size,            \
        -0.212607 * _size, -0.923880 * _size, -0.318190 * _size,            \
        -0.137950 * _size, -0.980785 * _size, -0.137950 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.137950 * _size, -0.980785 * _size, -0.137950 * _size,            \
        -0.162211 * _size, -0.980785 * _size, -0.108386 * _size,            \
        -0.270598 * _size, -0.923880 * _size, -0.270598 * _size,            \
        -0.162211 * _size, -0.980785 * _size, -0.108386 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.162211 * _size, -0.980785 * _size, -0.108386 * _size,            \
        -0.180240 * _size, -0.980785 * _size, -0.074658 * _size,            \
        -0.318190 * _size, -0.923880 * _size, -0.212607 * _size,            \
        -0.180240 * _size, -0.980785 * _size, -0.074658 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.180240 * _size, -0.980785 * _size, -0.074658 * _size,            \
        -0.191341 * _size, -0.980785 * _size, -0.038060 * _size,            \
        -0.353553 * _size, -0.923880 * _size, -0.146446 * _size,            \
        -0.191341 * _size, -0.980785 * _size, -0.038060 * _size,            \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.191341 * _size, -0.980785 * _size, -0.038060 * _size,            \
        -0.195090 * _size, -0.980785 * _size, 0.000000 * _size,             \
        -0.375330 * _size, -0.923880 * _size, -0.074658 * _size,            \
        -0.195090 * _size, -0.980785 * _size, 0.000000 * _size,             \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.195090 * _size, -0.980785 * _size, 0.000000 * _size,             \
        -0.191341 * _size, -0.980785 * _size, 0.038060 * _size,             \
        -0.382683 * _size, -0.923880 * _size, 0.000000 * _size,             \
        -0.191341 * _size, -0.980785 * _size, 0.038060 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.191341 * _size, -0.980785 * _size, 0.038060 * _size,             \
        -0.180240 * _size, -0.980785 * _size, 0.074658 * _size,             \
        -0.375330 * _size, -0.923880 * _size, 0.074658 * _size,             \
        -0.180240 * _size, -0.980785 * _size, 0.074658 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.180240 * _size, -0.980785 * _size, 0.074658 * _size,             \
        -0.162211 * _size, -0.980785 * _size, 0.108386 * _size,             \
        -0.353553 * _size, -0.923880 * _size, 0.146447 * _size,             \
        -0.162211 * _size, -0.980785 * _size, 0.108386 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.162211 * _size, -0.980785 * _size, 0.108386 * _size,             \
        -0.137949 * _size, -0.980785 * _size, 0.137950 * _size,             \
        -0.318189 * _size, -0.923880 * _size, 0.212608 * _size,             \
        -0.137949 * _size, -0.980785 * _size, 0.137950 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.137949 * _size, -0.980785 * _size, 0.137950 * _size,             \
        -0.108386 * _size, -0.980785 * _size, 0.162212 * _size,             \
        -0.270598 * _size, -0.923880 * _size, 0.270598 * _size,             \
        -0.108386 * _size, -0.980785 * _size, 0.162212 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.108386 * _size, -0.980785 * _size, 0.162212 * _size,             \
        -0.074658 * _size, -0.980785 * _size, 0.180240 * _size,             \
        -0.212607 * _size, -0.923880 * _size, 0.318190 * _size,             \
        -0.074658 * _size, -0.980785 * _size, 0.180240 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.074658 * _size, -0.980785 * _size, 0.180240 * _size,             \
        -0.038060 * _size, -0.980785 * _size, 0.191342 * _size,             \
        -0.146446 * _size, -0.923880 * _size, 0.353553 * _size,             \
        -0.038060 * _size, -0.980785 * _size, 0.191342 * _size,             \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        -0.038060 * _size, -0.980785 * _size, 0.191342 * _size,             \
        0.000000 * _size, -0.980785 * _size, 0.195090 * _size,          \
        -0.074657 * _size, -0.923880 * _size, 0.375330 * _size,             \
        0.000000 * _size, -0.980785 * _size, 0.195090 * _size,          \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        0.000000 * _size, -0.980785 * _size, 0.195090 * _size,          \
        0.038060 * _size, -0.980785 * _size, 0.191341 * _size,          \
        0.000000 * _size, -0.923880 * _size, 0.382683 * _size,          \
        0.038060 * _size, -0.980785 * _size, 0.191341 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.038060 * _size, -0.980785 * _size, 0.191341 * _size,          \
        0.074658 * _size, -0.980785 * _size, 0.180240 * _size,          \
        0.074658 * _size, -0.923880 * _size, 0.375330 * _size,          \
        0.074658 * _size, -0.980785 * _size, 0.180240 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.074658 * _size, -0.980785 * _size, 0.180240 * _size,          \
        0.108386 * _size, -0.980785 * _size, 0.162211 * _size,          \
        0.146447 * _size, -0.923880 * _size, 0.353553 * _size,          \
        0.108386 * _size, -0.980785 * _size, 0.162211 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.108386 * _size, -0.980785 * _size, 0.162211 * _size,          \
        0.137950 * _size, -0.980785 * _size, 0.137949 * _size,          \
        0.212608 * _size, -0.923880 * _size, 0.318189 * _size,          \
        0.137950 * _size, -0.980785 * _size, 0.137949 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.137950 * _size, -0.980785 * _size, 0.137949 * _size,          \
        0.162212 * _size, -0.980785 * _size, 0.108386 * _size,          \
        0.270598 * _size, -0.923880 * _size, 0.270598 * _size,          \
        0.162212 * _size, -0.980785 * _size, 0.108386 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.162212 * _size, -0.980785 * _size, 0.108386 * _size,          \
        0.180240 * _size, -0.980785 * _size, 0.074657 * _size,          \
        0.318190 * _size, -0.923880 * _size, 0.212607 * _size,          \
        0.180240 * _size, -0.980785 * _size, 0.074657 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.180240 * _size, -0.980785 * _size, 0.074657 * _size,          \
        0.191342 * _size, -0.980785 * _size, 0.038060 * _size,          \
        0.353553 * _size, -0.923880 * _size, 0.146446 * _size,          \
        0.191342 * _size, -0.980785 * _size, 0.038060 * _size,          \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.191342 * _size, -0.980785 * _size, 0.038060 * _size,          \
        0.195090 * _size, -0.980785 * _size, -0.000000 * _size,             \
        0.375330 * _size, -0.923880 * _size, 0.074657 * _size,          \
        0.195090 * _size, -0.980785 * _size, -0.000000 * _size,             \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.195090 * _size, -0.980785 * _size, -0.000000 * _size,             \
        0.191341 * _size, -0.980785 * _size, -0.038060 * _size,             \
        0.382683 * _size, -0.923880 * _size, -0.000001 * _size,             \
        0.191341 * _size, -0.980785 * _size, -0.038060 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.191341 * _size, -0.980785 * _size, -0.038060 * _size,             \
        0.180240 * _size, -0.980785 * _size, -0.074658 * _size,             \
        0.375330 * _size, -0.923880 * _size, -0.074658 * _size,             \
        0.180240 * _size, -0.980785 * _size, -0.074658 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.180240 * _size, -0.980785 * _size, -0.074658 * _size,             \
        0.162211 * _size, -0.980785 * _size, -0.108387 * _size,             \
        0.353553 * _size, -0.923880 * _size, -0.146447 * _size,             \
        0.162211 * _size, -0.980785 * _size, -0.108387 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.162211 * _size, -0.980785 * _size, -0.108387 * _size,             \
        0.137949 * _size, -0.980785 * _size, -0.137950 * _size,             \
        0.318189 * _size, -0.923880 * _size, -0.212608 * _size,             \
        0.137949 * _size, -0.980785 * _size, -0.137950 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.137949 * _size, -0.980785 * _size, -0.137950 * _size,             \
        0.108386 * _size, -0.980785 * _size, -0.162212 * _size,             \
        0.270597 * _size, -0.923880 * _size, -0.270598 * _size,             \
        0.108386 * _size, -0.980785 * _size, -0.162212 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.108386 * _size, -0.980785 * _size, -0.162212 * _size,             \
        0.074657 * _size, -0.980785 * _size, -0.180240 * _size,             \
        0.212607 * _size, -0.923880 * _size, -0.318190 * _size,             \
        0.074657 * _size, -0.980785 * _size, -0.180240 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.074657 * _size, -0.980785 * _size, -0.180240 * _size,             \
        0.038060 * _size, -0.980785 * _size, -0.191342 * _size,             \
        0.146446 * _size, -0.923880 * _size, -0.353554 * _size,             \
        0.038060 * _size, -0.980785 * _size, -0.191342 * _size,             \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        0.038060 * _size, -0.980785 * _size, -0.191342 * _size,             \
        -0.000000 * _size, -0.980785 * _size, -0.195090 * _size,            \
        0.074657 * _size, -0.923880 * _size, -0.375330 * _size,             \
        -0.000000 * _size, -0.980785 * _size, -0.195090 * _size,            \
        -0.000000 * _size, -0.923880 * _size, -0.382683 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.038060 * _size, -0.980785 * _size, -0.191341 * _size,            \
        -0.000000 * _size, -0.980785 * _size, -0.195090 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.074658 * _size, -0.980785 * _size, -0.180240 * _size,            \
        -0.038060 * _size, -0.980785 * _size, -0.191341 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.108386 * _size, -0.980785 * _size, -0.162211 * _size,            \
        -0.074658 * _size, -0.980785 * _size, -0.180240 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.137950 * _size, -0.980785 * _size, -0.137950 * _size,            \
        -0.108386 * _size, -0.980785 * _size, -0.162211 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.162211 * _size, -0.980785 * _size, -0.108386 * _size,            \
        -0.137950 * _size, -0.980785 * _size, -0.137950 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.180240 * _size, -0.980785 * _size, -0.074658 * _size,            \
        -0.162211 * _size, -0.980785 * _size, -0.108386 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.191341 * _size, -0.980785 * _size, -0.038060 * _size,            \
        -0.180240 * _size, -0.980785 * _size, -0.074658 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.195090 * _size, -0.980785 * _size, 0.000000 * _size,             \
        -0.191341 * _size, -0.980785 * _size, -0.038060 * _size,            \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.191341 * _size, -0.980785 * _size, 0.038060 * _size,             \
        -0.195090 * _size, -0.980785 * _size, 0.000000 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.180240 * _size, -0.980785 * _size, 0.074658 * _size,             \
        -0.191341 * _size, -0.980785 * _size, 0.038060 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.162211 * _size, -0.980785 * _size, 0.108386 * _size,             \
        -0.180240 * _size, -0.980785 * _size, 0.074658 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.137949 * _size, -0.980785 * _size, 0.137950 * _size,             \
        -0.162211 * _size, -0.980785 * _size, 0.108386 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.108386 * _size, -0.980785 * _size, 0.162212 * _size,             \
        -0.137949 * _size, -0.980785 * _size, 0.137950 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.074658 * _size, -0.980785 * _size, 0.180240 * _size,             \
        -0.108386 * _size, -0.980785 * _size, 0.162212 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.038060 * _size, -0.980785 * _size, 0.191342 * _size,             \
        -0.074658 * _size, -0.980785 * _size, 0.180240 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.000000 * _size, -0.980785 * _size, 0.195090 * _size,          \
        -0.038060 * _size, -0.980785 * _size, 0.191342 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.038060 * _size, -0.980785 * _size, 0.191341 * _size,          \
        0.000000 * _size, -0.980785 * _size, 0.195090 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.074658 * _size, -0.980785 * _size, 0.180240 * _size,          \
        0.038060 * _size, -0.980785 * _size, 0.191341 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.108386 * _size, -0.980785 * _size, 0.162211 * _size,          \
        0.074658 * _size, -0.980785 * _size, 0.180240 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.137950 * _size, -0.980785 * _size, 0.137949 * _size,          \
        0.108386 * _size, -0.980785 * _size, 0.162211 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.162212 * _size, -0.980785 * _size, 0.108386 * _size,          \
        0.137950 * _size, -0.980785 * _size, 0.137949 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.180240 * _size, -0.980785 * _size, 0.074657 * _size,          \
        0.162212 * _size, -0.980785 * _size, 0.108386 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.191342 * _size, -0.980785 * _size, 0.038060 * _size,          \
        0.180240 * _size, -0.980785 * _size, 0.074657 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.195090 * _size, -0.980785 * _size, -0.000000 * _size,             \
        0.191342 * _size, -0.980785 * _size, 0.038060 * _size,          \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.191341 * _size, -0.980785 * _size, -0.038060 * _size,             \
        0.195090 * _size, -0.980785 * _size, -0.000000 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.180240 * _size, -0.980785 * _size, -0.074658 * _size,             \
        0.191341 * _size, -0.980785 * _size, -0.038060 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.162211 * _size, -0.980785 * _size, -0.108387 * _size,             \
        0.180240 * _size, -0.980785 * _size, -0.074658 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.137949 * _size, -0.980785 * _size, -0.137950 * _size,             \
        0.162211 * _size, -0.980785 * _size, -0.108387 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.108386 * _size, -0.980785 * _size, -0.162212 * _size,             \
        0.137949 * _size, -0.980785 * _size, -0.137950 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.074657 * _size, -0.980785 * _size, -0.180240 * _size,             \
        0.108386 * _size, -0.980785 * _size, -0.162212 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        0.038060 * _size, -0.980785 * _size, -0.191342 * _size,             \
        0.074657 * _size, -0.980785 * _size, -0.180240 * _size,             \
        0.000000 * _size, -1.000000 * _size, -0.000000 * _size,             \
        -0.000000 * _size, -0.980785 * _size, -0.195090 * _size,            \
        0.038060 * _size, -0.980785 * _size, -0.191342 * _size,             \
	}, _vertex_count * 3 * sizeof(float));									\
													                        \
	float* _normals = (float*)malloc(sizeof(float) * _vertex_count * 3); 	\
	memcpy(_normals, (float []){											\
		0.000000, 1.000000, -0.000000,                                      \
        -0.005744, 0.976740, -0.214348,                                     \
        -0.047451, 0.976740, -0.209109,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.047451, 0.976740, -0.209109,                                     \
        -0.087334, 0.976740, -0.195834,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.087334, 0.976740, -0.195834,                                     \
        -0.123861, 0.976740, -0.175033,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.123861, 0.976740, -0.175033,                                     \
        -0.155629, 0.976740, -0.147506,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.155629, 0.976740, -0.147506,                                     \
        -0.181415, 0.976740, -0.114310,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.181415, 0.976740, -0.114310,                                     \
        -0.200230, 0.976740, -0.076721,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.200230, 0.976740, -0.076721,                                     \
        -0.211350, 0.976740, -0.036184,                                     \
        0.000000, 1.000000, -0.000000,                                      \
        -0.211350, 0.976740, -0.036184,                                     \
        -0.214348, 0.976740, 0.005744,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.214348, 0.976740, 0.005744,                                      \
        -0.209109, 0.976740, 0.047451,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.209109, 0.976740, 0.047451,                                      \
        -0.195834, 0.976740, 0.087334,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.195834, 0.976740, 0.087334,                                      \
        -0.175033, 0.976740, 0.123861,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.175033, 0.976740, 0.123861,                                      \
        -0.147506, 0.976740, 0.155629,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.147506, 0.976740, 0.155629,                                      \
        -0.114310, 0.976740, 0.181415,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.114310, 0.976740, 0.181415,                                      \
        -0.076721, 0.976740, 0.200230,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.076721, 0.976740, 0.200230,                                      \
        -0.036184, 0.976740, 0.211350,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        -0.036184, 0.976740, 0.211350,                                      \
        0.005744, 0.976740, 0.214348,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.005744, 0.976740, 0.214348,                                       \
        0.047451, 0.976740, 0.209109,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.047451, 0.976740, 0.209109,                                       \
        0.087334, 0.976740, 0.195834,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.087334, 0.976740, 0.195834,                                       \
        0.123862, 0.976740, 0.175033,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.123862, 0.976740, 0.175033,                                       \
        0.155629, 0.976740, 0.147505,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.155629, 0.976740, 0.147505,                                       \
        0.181415, 0.976740, 0.114309,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.181415, 0.976740, 0.114309,                                       \
        0.200230, 0.976740, 0.076721,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.200230, 0.976740, 0.076721,                                       \
        0.211350, 0.976740, 0.036184,                                       \
        0.000000, 1.000000, -0.000000,                                      \
        0.211350, 0.976740, 0.036184,                                       \
        0.214348, 0.976740, -0.005744,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.214348, 0.976740, -0.005744,                                      \
        0.209109, 0.976740, -0.047451,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.209109, 0.976740, -0.047451,                                      \
        0.195834, 0.976740, -0.087334,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.195834, 0.976740, -0.087334,                                      \
        0.175033, 0.976740, -0.123862,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.175033, 0.976740, -0.123862,                                      \
        0.147505, 0.976740, -0.155629,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.147505, 0.976740, -0.155629,                                      \
        0.114309, 0.976740, -0.181415,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.114309, 0.976740, -0.181415,                                      \
        0.076721, 0.976740, -0.200230,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.076721, 0.976740, -0.200230,                                      \
        0.036184, 0.976740, -0.211350,                                      \
        0.000000, 1.000000, -0.000000,                                      \
        0.036184, 0.976740, -0.211350,                                      \
        -0.005744, 0.976740, -0.214348,                                     \
        -0.005744, 0.976740, -0.214348,                                     \
        -0.002983, 0.923888, -0.382651,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.005744, 0.976740, -0.214348,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.047451, 0.976740, -0.209109,                                     \
        -0.047451, 0.976740, -0.209109,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.047451, 0.976740, -0.209109,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.087334, 0.976740, -0.195834,                                     \
        -0.087334, 0.976740, -0.195834,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.087334, 0.976740, -0.195834,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.123861, 0.976740, -0.175033,                                     \
        -0.123861, 0.976740, -0.175033,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.123861, 0.976740, -0.175033,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.155629, 0.976740, -0.147506,                                     \
        -0.155629, 0.976740, -0.147506,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.155629, 0.976740, -0.147506,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.181415, 0.976740, -0.114310,                                     \
        -0.181415, 0.976740, -0.114310,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.181415, 0.976740, -0.114310,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.200230, 0.976740, -0.076721,                                     \
        -0.200230, 0.976740, -0.076721,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.200230, 0.976740, -0.076721,                                     \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.211350, 0.976740, -0.036184,                                     \
        -0.211350, 0.976740, -0.036184,                                     \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.211350, 0.976740, -0.036184,                                     \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.214348, 0.976740, 0.005744,                                      \
        -0.214348, 0.976740, 0.005744,                                      \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.214348, 0.976740, 0.005744,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.209109, 0.976740, 0.047451,                                      \
        -0.209109, 0.976740, 0.047451,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.209109, 0.976740, 0.047451,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.195834, 0.976740, 0.087334,                                      \
        -0.195834, 0.976740, 0.087334,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.195834, 0.976740, 0.087334,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.175033, 0.976740, 0.123861,                                      \
        -0.175033, 0.976740, 0.123861,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.175033, 0.976740, 0.123861,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.147506, 0.976740, 0.155629,                                      \
        -0.147506, 0.976740, 0.155629,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.147506, 0.976740, 0.155629,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.114310, 0.976740, 0.181415,                                      \
        -0.114310, 0.976740, 0.181415,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.114310, 0.976740, 0.181415,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.076721, 0.976740, 0.200230,                                      \
        -0.076721, 0.976740, 0.200230,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.071725, 0.923888, 0.375881,                                      \
        -0.076721, 0.976740, 0.200230,                                      \
        -0.071725, 0.923888, 0.375881,                                      \
        -0.036184, 0.976740, 0.211350,                                      \
        -0.036184, 0.976740, 0.211350,                                      \
        -0.071725, 0.923888, 0.375881,                                      \
        0.002984, 0.923888, 0.382651,                                       \
        -0.036184, 0.976740, 0.211350,                                      \
        0.002984, 0.923888, 0.382651,                                       \
        0.005744, 0.976740, 0.214348,                                       \
        0.005744, 0.976740, 0.214348,                                       \
        0.002984, 0.923888, 0.382651,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.005744, 0.976740, 0.214348,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.047451, 0.976740, 0.209109,                                       \
        0.047451, 0.976740, 0.209109,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.047451, 0.976740, 0.209109,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.087334, 0.976740, 0.195834,                                       \
        0.087334, 0.976740, 0.195834,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.087334, 0.976740, 0.195834,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.123862, 0.976740, 0.175033,                                       \
        0.123862, 0.976740, 0.175033,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.123862, 0.976740, 0.175033,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.155629, 0.976740, 0.147505,                                       \
        0.155629, 0.976740, 0.147505,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.155629, 0.976740, 0.147505,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.181415, 0.976740, 0.114309,                                       \
        0.181415, 0.976740, 0.114309,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.181415, 0.976740, 0.114309,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.200230, 0.976740, 0.076721,                                       \
        0.200230, 0.976740, 0.076721,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.375881, 0.923888, 0.071725,                                       \
        0.200230, 0.976740, 0.076721,                                       \
        0.375881, 0.923888, 0.071725,                                       \
        0.211350, 0.976740, 0.036184,                                       \
        0.211350, 0.976740, 0.036184,                                       \
        0.375881, 0.923888, 0.071725,                                       \
        0.382651, 0.923888, -0.002984,                                      \
        0.211350, 0.976740, 0.036184,                                       \
        0.382651, 0.923888, -0.002984,                                      \
        0.214348, 0.976740, -0.005744,                                      \
        0.214348, 0.976740, -0.005744,                                      \
        0.382651, 0.923888, -0.002984,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.214348, 0.976740, -0.005744,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.209109, 0.976740, -0.047451,                                      \
        0.209109, 0.976740, -0.047451,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.209109, 0.976740, -0.047451,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.195834, 0.976740, -0.087334,                                      \
        0.195834, 0.976740, -0.087334,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.195834, 0.976740, -0.087334,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.175033, 0.976740, -0.123862,                                      \
        0.175033, 0.976740, -0.123862,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.175033, 0.976740, -0.123862,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.147505, 0.976740, -0.155629,                                      \
        0.147505, 0.976740, -0.155629,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.147505, 0.976740, -0.155629,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.114309, 0.976740, -0.181415,                                      \
        0.114309, 0.976740, -0.181415,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.114309, 0.976740, -0.181415,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.076721, 0.976740, -0.200230,                                      \
        0.076721, 0.976740, -0.200230,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.071725, 0.923888, -0.375881,                                      \
        0.076721, 0.976740, -0.200230,                                      \
        0.071725, 0.923888, -0.375881,                                      \
        0.036184, 0.976740, -0.211350,                                      \
        0.036184, 0.976740, -0.211350,                                      \
        0.071725, 0.923888, -0.375881,                                      \
        -0.002983, 0.923888, -0.382651,                                     \
        0.036184, 0.976740, -0.211350,                                      \
        -0.002983, 0.923888, -0.382651,                                     \
        -0.005744, 0.976740, -0.214348,                                     \
        -0.002983, 0.923888, -0.382651,                                     \
        -0.002681, 0.831490, -0.555533,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.002983, 0.923888, -0.382651,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.077577, 0.923888, -0.374717,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.149190, 0.923888, -0.352382,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.215070, 0.923888, -0.316506,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.272685, 0.923888, -0.268466,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.319820, 0.923888, -0.210109,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.354665, 0.923888, -0.143678,                                     \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.375881, 0.923888, -0.071726,                                     \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.382651, 0.923888, 0.002983,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.374717, 0.923888, 0.077578,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.352382, 0.923888, 0.149191,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.316506, 0.923888, 0.215070,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.268466, 0.923888, 0.272685,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.210109, 0.923888, 0.319821,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.105749, 0.831490, 0.545381,                                      \
        -0.143678, 0.923888, 0.354666,                                      \
        -0.105749, 0.831490, 0.545381,                                      \
        -0.071725, 0.923888, 0.375881,                                      \
        -0.071725, 0.923888, 0.375881,                                      \
        -0.105749, 0.831490, 0.545381,                                      \
        0.002681, 0.831490, 0.555533,                                       \
        -0.071725, 0.923888, 0.375881,                                      \
        0.002681, 0.831490, 0.555533,                                       \
        0.002984, 0.923888, 0.382651,                                       \
        0.002984, 0.923888, 0.382651,                                       \
        0.002681, 0.831490, 0.555533,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.002984, 0.923888, 0.382651,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.077578, 0.923888, 0.374717,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.149191, 0.923888, 0.352382,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.215071, 0.923888, 0.316505,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.272685, 0.923888, 0.268466,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.319821, 0.923888, 0.210109,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.545381, 0.831490, 0.105749,                                       \
        0.354666, 0.923888, 0.143678,                                       \
        0.545381, 0.831490, 0.105749,                                       \
        0.375881, 0.923888, 0.071725,                                       \
        0.375881, 0.923888, 0.071725,                                       \
        0.545381, 0.831490, 0.105749,                                       \
        0.555533, 0.831490, -0.002681,                                      \
        0.375881, 0.923888, 0.071725,                                       \
        0.555533, 0.831490, -0.002681,                                      \
        0.382651, 0.923888, -0.002984,                                      \
        0.382651, 0.923888, -0.002984,                                      \
        0.555533, 0.831490, -0.002681,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.382651, 0.923888, -0.002984,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.374717, 0.923888, -0.077578,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.352382, 0.923888, -0.149191,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.316505, 0.923888, -0.215071,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.268465, 0.923888, -0.272685,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.210109, 0.923888, -0.319821,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.105749, 0.831490, -0.545381,                                      \
        0.143678, 0.923888, -0.354666,                                      \
        0.105749, 0.831490, -0.545381,                                      \
        0.071725, 0.923888, -0.375881,                                      \
        0.071725, 0.923888, -0.375881,                                      \
        0.105749, 0.831490, -0.545381,                                      \
        -0.002681, 0.831490, -0.555533,                                     \
        0.071725, 0.923888, -0.375881,                                      \
        -0.002681, 0.831490, -0.555533,                                     \
        -0.002983, 0.923888, -0.382651,                                     \
        -0.002681, 0.831490, -0.555533,                                     \
        -0.002276, 0.707138, -0.707072,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.002681, 0.831490, -0.555533,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.111008, 0.831490, -0.544335,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.215070, 0.831490, -0.512219,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.310866, 0.831490, -0.460419,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.394716, 0.831490, -0.390925,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.463398, 0.831490, -0.306408,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.514271, 0.831490, -0.210116,                                     \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.545381, 0.831490, -0.105750,                                     \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.555533, 0.831490, 0.002681,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.544335, 0.831490, 0.111008,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.512219, 0.831490, 0.215070,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.460419, 0.831490, 0.310867,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.390925, 0.831490, 0.394717,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.306408, 0.831490, 0.463398,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.135711, 0.707138, 0.693930,                                      \
        -0.210116, 0.831490, 0.514271,                                      \
        -0.135711, 0.707138, 0.693930,                                      \
        -0.105749, 0.831490, 0.545381,                                      \
        -0.105749, 0.831490, 0.545381,                                      \
        -0.135711, 0.707138, 0.693930,                                      \
        0.002276, 0.707138, 0.707072,                                       \
        -0.105749, 0.831490, 0.545381,                                      \
        0.002276, 0.707138, 0.707072,                                       \
        0.002681, 0.831490, 0.555533,                                       \
        0.002681, 0.831490, 0.555533,                                       \
        0.002276, 0.707138, 0.707072,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.002681, 0.831490, 0.555533,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.111009, 0.831490, 0.544335,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.215070, 0.831490, 0.512219,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.310867, 0.831490, 0.460419,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.394717, 0.831490, 0.390925,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.463398, 0.831490, 0.306408,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.693930, 0.707138, 0.135710,                                       \
        0.514271, 0.831490, 0.210116,                                       \
        0.693930, 0.707138, 0.135710,                                       \
        0.545381, 0.831490, 0.105749,                                       \
        0.545381, 0.831490, 0.105749,                                       \
        0.693930, 0.707138, 0.135710,                                       \
        0.707072, 0.707138, -0.002277,                                      \
        0.545381, 0.831490, 0.105749,                                       \
        0.707072, 0.707138, -0.002277,                                      \
        0.555533, 0.831490, -0.002681,                                      \
        0.555533, 0.831490, -0.002681,                                      \
        0.707072, 0.707138, -0.002277,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.555533, 0.831490, -0.002681,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.544335, 0.831490, -0.111009,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.512219, 0.831490, -0.215071,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.460419, 0.831490, -0.310867,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.390925, 0.831490, -0.394717,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.306408, 0.831490, -0.463398,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.135710, 0.707138, -0.693930,                                      \
        0.210115, 0.831490, -0.514271,                                      \
        0.135710, 0.707138, -0.693930,                                      \
        0.105749, 0.831490, -0.545381,                                      \
        0.105749, 0.831490, -0.545381,                                      \
        0.135710, 0.707138, -0.693930,                                      \
        -0.002276, 0.707138, -0.707072,                                     \
        0.105749, 0.831490, -0.545381,                                      \
        -0.002276, 0.707138, -0.707072,                                     \
        -0.002681, 0.831490, -0.555533,                                     \
        -0.002276, 0.707138, -0.707072,                                     \
        -0.001785, 0.555605, -0.831445,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.002276, 0.707138, -0.707072,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.140175, 0.707138, -0.693042,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.272687, 0.707138, -0.652379,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.394720, 0.707138, -0.586645,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.501585, 0.707138, -0.498367,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.589173, 0.707138, -0.390936,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.654120, 0.707138, -0.268482,                                     \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.693930, 0.707138, -0.135711,                                     \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.707072, 0.707138, 0.002276,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.693042, 0.707138, 0.140175,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.652379, 0.707138, 0.272687,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.586645, 0.707138, 0.394721,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.498366, 0.707138, 0.501585,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.390936, 0.707138, 0.589174,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.160456, 0.555605, 0.815817,                                      \
        -0.268482, 0.707138, 0.654121,                                      \
        -0.160456, 0.555605, 0.815817,                                      \
        -0.135711, 0.707138, 0.693930,                                      \
        -0.135711, 0.707138, 0.693930,                                      \
        -0.160456, 0.555605, 0.815817,                                      \
        0.001785, 0.555605, 0.831445,                                       \
        -0.135711, 0.707138, 0.693930,                                      \
        0.001785, 0.555605, 0.831445,                                       \
        0.002276, 0.707138, 0.707072,                                       \
        0.002276, 0.707138, 0.707072,                                       \
        0.001785, 0.555605, 0.831445,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.002276, 0.707138, 0.707072,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.140176, 0.707138, 0.693042,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.272688, 0.707138, 0.652378,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.394721, 0.707138, 0.586644,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.501585, 0.707138, 0.498366,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.589174, 0.707138, 0.390935,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.815817, 0.555605, 0.160455,                                       \
        0.654121, 0.707138, 0.268482,                                       \
        0.815817, 0.555605, 0.160455,                                       \
        0.693930, 0.707138, 0.135710,                                       \
        0.693930, 0.707138, 0.135710,                                       \
        0.815817, 0.555605, 0.160455,                                       \
        0.831445, 0.555605, -0.001786,                                      \
        0.693930, 0.707138, 0.135710,                                       \
        0.831445, 0.555605, -0.001786,                                      \
        0.707072, 0.707138, -0.002277,                                      \
        0.707072, 0.707138, -0.002277,                                      \
        0.831445, 0.555605, -0.001786,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.707072, 0.707138, -0.002277,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.693042, 0.707138, -0.140176,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.652378, 0.707138, -0.272688,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.586644, 0.707138, -0.394721,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.498366, 0.707138, -0.501585,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.390935, 0.707138, -0.589174,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.160455, 0.555605, -0.815817,                                      \
        0.268481, 0.707138, -0.654121,                                      \
        0.160455, 0.555605, -0.815817,                                      \
        0.135710, 0.707138, -0.693930,                                      \
        0.135710, 0.707138, -0.693930,                                      \
        0.160455, 0.555605, -0.815817,                                      \
        -0.001785, 0.555605, -0.831445,                                     \
        0.135710, 0.707138, -0.693930,                                      \
        -0.001785, 0.555605, -0.831445,                                     \
        -0.002276, 0.707138, -0.707072,                                     \
        -0.001785, 0.555605, -0.831445,                                     \
        -0.001228, 0.382713, -0.923866,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.001785, 0.555605, -0.831445,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.163957, 0.555605, -0.815120,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.319829, 0.555605, -0.767472,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.463410, 0.555605, -0.690329,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.589182, 0.555605, -0.586658,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.692312, 0.555605, -0.460442,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.768838, 0.555605, -0.316531,                                     \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.815817, 0.555605, -0.160456,                                     \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.831445, 0.555605, 0.001785,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.815120, 0.555605, 0.163957,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.767471, 0.555605, 0.319829,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.690329, 0.555605, 0.463410,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.586658, 0.555605, 0.589182,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.460441, 0.555605, 0.692313,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.179033, 0.382713, 0.906354,                                      \
        -0.316531, 0.555605, 0.768838,                                      \
        -0.179033, 0.382713, 0.906354,                                      \
        -0.160456, 0.555605, 0.815817,                                      \
        -0.160456, 0.555605, 0.815817,                                      \
        -0.179033, 0.382713, 0.906354,                                      \
        0.001228, 0.382713, 0.923866,                                       \
        -0.160456, 0.555605, 0.815817,                                      \
        0.001228, 0.382713, 0.923866,                                       \
        0.001785, 0.555605, 0.831445,                                       \
        0.001785, 0.555605, 0.831445,                                       \
        0.001228, 0.382713, 0.923866,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.001785, 0.555605, 0.831445,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.163958, 0.555605, 0.815120,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.319830, 0.555605, 0.767471,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.463410, 0.555605, 0.690329,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.589183, 0.555605, 0.586657,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.692313, 0.555605, 0.460441,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.906354, 0.382713, 0.179032,                                       \
        0.768838, 0.555605, 0.316530,                                       \
        0.906354, 0.382713, 0.179032,                                       \
        0.815817, 0.555605, 0.160455,                                       \
        0.815817, 0.555605, 0.160455,                                       \
        0.906354, 0.382713, 0.179032,                                       \
        0.923866, 0.382713, -0.001229,                                      \
        0.815817, 0.555605, 0.160455,                                       \
        0.923866, 0.382713, -0.001229,                                      \
        0.831445, 0.555605, -0.001786,                                      \
        0.831445, 0.555605, -0.001786,                                      \
        0.923866, 0.382713, -0.001229,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.831445, 0.555605, -0.001786,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.815120, 0.555605, -0.163958,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.767471, 0.555605, -0.319830,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.690329, 0.555605, -0.463411,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.586657, 0.555605, -0.589183,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.460441, 0.555605, -0.692313,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.179032, 0.382713, -0.906354,                                      \
        0.316530, 0.555605, -0.768838,                                      \
        0.179032, 0.382713, -0.906354,                                      \
        0.160455, 0.555605, -0.815817,                                      \
        0.160455, 0.555605, -0.815817,                                      \
        0.179032, 0.382713, -0.906354,                                      \
        -0.001228, 0.382713, -0.923866,                                     \
        0.160455, 0.555605, -0.815817,                                      \
        -0.001228, 0.382713, -0.923866,                                     \
        -0.001785, 0.555605, -0.831445,                                     \
        -0.001228, 0.382713, -0.923866,                                     \
        -0.000626, 0.195108, -0.980782,                                     \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.001228, 0.382713, -0.923866,                                     \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.181441, 0.382713, -0.905875,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.354682, 0.382713, -0.853072,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.514293, 0.382713, -0.767485,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.654140, 0.382713, -0.652404,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.768849, 0.382713, -0.512252,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.962058, 0.195108, -0.190728,                                     \
        -0.854011, 0.382713, -0.352414,                                     \
        -0.962058, 0.195108, -0.190728,                                     \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.962058, 0.195108, -0.190728,                                     \
        -0.980782, 0.195108, 0.000625,                                      \
        -0.906354, 0.382713, -0.179033,                                     \
        -0.980782, 0.195108, 0.000625,                                      \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.980782, 0.195108, 0.000625,                                      \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.923866, 0.382713, 0.001228,                                      \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.905875, 0.382713, 0.181441,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.853071, 0.382713, 0.354683,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.767485, 0.382713, 0.514294,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.652404, 0.382713, 0.654140,                                      \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.512252, 0.382713, 0.768849,                                      \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        -0.352414, 0.382713, 0.854011,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        -0.179033, 0.382713, 0.906354,                                      \
        -0.179033, 0.382713, 0.906354,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        0.000626, 0.195108, 0.980782,                                       \
        -0.179033, 0.382713, 0.906354,                                      \
        0.000626, 0.195108, 0.980782,                                       \
        0.001228, 0.382713, 0.923866,                                       \
        0.001228, 0.382713, 0.923866,                                       \
        0.000626, 0.195108, 0.980782,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.001228, 0.382713, 0.923866,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.181442, 0.382713, 0.905875,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.354683, 0.382713, 0.853071,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.514294, 0.382713, 0.767484,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.654141, 0.382713, 0.652403,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.768849, 0.382713, 0.512251,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.962058, 0.195108, 0.190727,                                       \
        0.854011, 0.382713, 0.352413,                                       \
        0.962058, 0.195108, 0.190727,                                       \
        0.906354, 0.382713, 0.179032,                                       \
        0.906354, 0.382713, 0.179032,                                       \
        0.962058, 0.195108, 0.190727,                                       \
        0.980782, 0.195108, -0.000627,                                      \
        0.906354, 0.382713, 0.179032,                                       \
        0.980782, 0.195108, -0.000627,                                      \
        0.923866, 0.382713, -0.001229,                                      \
        0.923866, 0.382713, -0.001229,                                      \
        0.980782, 0.195108, -0.000627,                                      \
        0.961814, 0.195108, -0.191956,                                      \
        0.923866, 0.382713, -0.001229,                                      \
        0.961814, 0.195108, -0.191956,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.961814, 0.195108, -0.191956,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.905875, 0.382713, -0.181443,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.853071, 0.382713, -0.354684,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.767484, 0.382713, -0.514295,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.652403, 0.382713, -0.654141,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.512251, 0.382713, -0.768850,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.190726, 0.195108, -0.962058,                                      \
        0.352413, 0.382713, -0.854012,                                      \
        0.190726, 0.195108, -0.962058,                                      \
        0.179032, 0.382713, -0.906354,                                      \
        0.179032, 0.382713, -0.906354,                                      \
        0.190726, 0.195108, -0.962058,                                      \
        -0.000626, 0.195108, -0.980782,                                     \
        0.179032, 0.382713, -0.906354,                                      \
        -0.000626, 0.195108, -0.980782,                                     \
        -0.001228, 0.382713, -0.923866,                                     \
        -0.000626, 0.195108, -0.980782,                                     \
        -0.000000, -0.000000, -1.000000,                                        \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.000626, 0.195108, -0.980782,                                     \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.191954, 0.195108, -0.961814,                                     \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.375906, 0.195108, -0.905885,                                     \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.545413, 0.195108, -0.815143,                                     \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.693959, 0.195108, -0.693075,                                     \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.815837, 0.195108, -0.544373,                                     \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.980785, 0.000000, -0.195090,                                     \
        -0.906363, 0.195108, -0.374751,                                     \
        -0.980785, 0.000000, -0.195090,                                     \
        -0.962058, 0.195108, -0.190728,                                     \
        -0.962058, 0.195108, -0.190728,                                     \
        -0.980785, 0.000000, -0.195090,                                     \
        -1.000000, 0.000000, 0.000000,                                      \
        -0.962058, 0.195108, -0.190728,                                     \
        -1.000000, 0.000000, 0.000000,                                      \
        -0.980782, 0.195108, 0.000625,                                      \
        -0.980782, 0.195108, 0.000625,                                      \
        -1.000000, 0.000000, 0.000000,                                      \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.980782, 0.195108, 0.000625,                                      \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.961814, 0.195108, 0.191954,                                      \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.905885, 0.195108, 0.375907,                                      \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.815143, 0.195108, 0.545413,                                      \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.693075, 0.195108, 0.693960,                                      \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.544373, 0.195108, 0.815838,                                      \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.195089, 0.000000, 0.980785,                                      \
        -0.374751, 0.195108, 0.906364,                                      \
        -0.195089, 0.000000, 0.980785,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        -0.195089, 0.000000, 0.980785,                                      \
        0.000001, -0.000000, 1.000000,                                      \
        -0.190727, 0.195108, 0.962058,                                      \
        0.000001, -0.000000, 1.000000,                                      \
        0.000626, 0.195108, 0.980782,                                       \
        0.000626, 0.195108, 0.980782,                                       \
        0.000001, -0.000000, 1.000000,                                      \
        0.195091, 0.000000, 0.980785,                                       \
        0.000626, 0.195108, 0.980782,                                       \
        0.195091, 0.000000, 0.980785,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.195091, 0.000000, 0.980785,                                       \
        0.382685, 0.000000, 0.923879,                                       \
        0.191955, 0.195108, 0.961814,                                       \
        0.382685, 0.000000, 0.923879,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.382685, 0.000000, 0.923879,                                       \
        0.555571, 0.000000, 0.831469,                                       \
        0.375907, 0.195108, 0.905884,                                       \
        0.555571, 0.000000, 0.831469,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.555571, 0.000000, 0.831469,                                       \
        0.707108, 0.000000, 0.707106,                                       \
        0.545414, 0.195108, 0.815142,                                       \
        0.707108, 0.000000, 0.707106,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.707108, 0.000000, 0.707106,                                       \
        0.831470, 0.000000, 0.555569,                                       \
        0.693960, 0.195108, 0.693075,                                       \
        0.831470, 0.000000, 0.555569,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.831470, 0.000000, 0.555569,                                       \
        0.923880, 0.000000, 0.382682,                                       \
        0.815838, 0.195108, 0.544372,                                       \
        0.923880, 0.000000, 0.382682,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.923880, 0.000000, 0.382682,                                       \
        0.980786, 0.000000, 0.195089,                                       \
        0.906364, 0.195108, 0.374750,                                       \
        0.980786, 0.000000, 0.195089,                                       \
        0.962058, 0.195108, 0.190727,                                       \
        0.962058, 0.195108, 0.190727,                                       \
        0.980786, 0.000000, 0.195089,                                       \
        1.000000, 0.000000, -0.000002,                                      \
        0.962058, 0.195108, 0.190727,                                       \
        1.000000, 0.000000, -0.000002,                                      \
        0.980782, 0.195108, -0.000627,                                      \
        0.980782, 0.195108, -0.000627,                                      \
        1.000000, 0.000000, -0.000002,                                      \
        0.980785, -0.000000, -0.195092,                                     \
        0.980782, 0.195108, -0.000627,                                      \
        0.980785, -0.000000, -0.195092,                                     \
        0.961814, 0.195108, -0.191956,                                      \
        0.961814, 0.195108, -0.191956,                                      \
        0.980785, -0.000000, -0.195092,                                     \
        0.923879, 0.000000, -0.382685,                                      \
        0.961814, 0.195108, -0.191956,                                      \
        0.923879, 0.000000, -0.382685,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.923879, 0.000000, -0.382685,                                      \
        0.831469, 0.000000, -0.555572,                                      \
        0.905884, 0.195108, -0.375908,                                      \
        0.831469, 0.000000, -0.555572,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.831469, 0.000000, -0.555572,                                      \
        0.707106, 0.000000, -0.707108,                                      \
        0.815142, 0.195108, -0.545414,                                      \
        0.707106, 0.000000, -0.707108,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.707106, 0.000000, -0.707108,                                      \
        0.555569, 0.000000, -0.831471,                                      \
        0.693074, 0.195108, -0.693961,                                      \
        0.555569, 0.000000, -0.831471,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.555569, 0.000000, -0.831471,                                      \
        0.382681, 0.000000, -0.923880,                                      \
        0.544372, 0.195108, -0.815839,                                      \
        0.382681, 0.000000, -0.923880,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.382681, 0.000000, -0.923880,                                      \
        0.195089, 0.000000, -0.980786,                                      \
        0.374750, 0.195108, -0.906364,                                      \
        0.195089, 0.000000, -0.980786,                                      \
        0.190726, 0.195108, -0.962058,                                      \
        0.190726, 0.195108, -0.962058,                                      \
        0.195089, 0.000000, -0.980786,                                      \
        -0.000000, -0.000000, -1.000000,                                        \
        0.190726, 0.195108, -0.962058,                                      \
        -0.000000, -0.000000, -1.000000,                                        \
        -0.000626, 0.195108, -0.980782,                                     \
        -0.000000, -0.000000, -1.000000,                                        \
        0.000624, -0.195108, -0.980782,                                     \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.000000, -0.000000, -1.000000,                                        \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.195090, -0.000000, -0.980785,                                        \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.382683, 0.000000, -0.923880,                                     \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.555570, 0.000000, -0.831470,                                     \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.707107, 0.000000, -0.707107,                                     \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.831470, 0.000000, -0.555570,                                     \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.923880, 0.000000, -0.382683,                                     \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.980785, 0.000000, -0.195090,                                     \
        -0.980785, 0.000000, -0.195090,                                     \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.980782, -0.195107, -0.000625,                                        \
        -0.980785, 0.000000, -0.195090,                                     \
        -0.980782, -0.195107, -0.000625,                                        \
        -1.000000, 0.000000, 0.000000,                                      \
        -1.000000, 0.000000, 0.000000,                                      \
        -0.980782, -0.195107, -0.000625,                                        \
        -0.962058, -0.195107, 0.190728,                                     \
        -1.000000, 0.000000, 0.000000,                                      \
        -0.962058, -0.195107, 0.190728,                                     \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.962058, -0.195107, 0.190728,                                     \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.980785, -0.000000, 0.195091,                                     \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.923879, 0.000000, 0.382684,                                      \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.831469, 0.000000, 0.555571,                                      \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.707106, 0.000000, 0.707107,                                      \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.555570, 0.000000, 0.831470,                                      \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.382683, -0.000000, 0.923880,                                     \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.195089, 0.000000, 0.980785,                                      \
        -0.195089, 0.000000, 0.980785,                                      \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.000624, -0.195107, 0.980782,                                     \
        -0.195089, 0.000000, 0.980785,                                      \
        -0.000624, -0.195107, 0.980782,                                     \
        0.000001, -0.000000, 1.000000,                                      \
        0.000001, -0.000000, 1.000000,                                      \
        -0.000624, -0.195107, 0.980782,                                     \
        0.190729, -0.195107, 0.962058,                                      \
        0.000001, -0.000000, 1.000000,                                      \
        0.190729, -0.195107, 0.962058,                                      \
        0.195091, 0.000000, 0.980785,                                       \
        0.195091, 0.000000, 0.980785,                                       \
        0.190729, -0.195107, 0.962058,                                      \
        0.374753, -0.195107, 0.906363,                                      \
        0.195091, 0.000000, 0.980785,                                       \
        0.374753, -0.195107, 0.906363,                                      \
        0.382685, 0.000000, 0.923879,                                       \
        0.382685, 0.000000, 0.923879,                                       \
        0.374753, -0.195107, 0.906363,                                      \
        0.544374, -0.195107, 0.815837,                                      \
        0.382685, 0.000000, 0.923879,                                       \
        0.544374, -0.195107, 0.815837,                                      \
        0.555571, 0.000000, 0.831469,                                       \
        0.555571, 0.000000, 0.831469,                                       \
        0.544374, -0.195107, 0.815837,                                      \
        0.693076, -0.195108, 0.693959,                                      \
        0.555571, 0.000000, 0.831469,                                       \
        0.693076, -0.195108, 0.693959,                                      \
        0.707108, 0.000000, 0.707106,                                       \
        0.707108, 0.000000, 0.707106,                                       \
        0.693076, -0.195108, 0.693959,                                      \
        0.815144, -0.195108, 0.545412,                                      \
        0.707108, 0.000000, 0.707106,                                       \
        0.815144, -0.195108, 0.545412,                                      \
        0.831470, 0.000000, 0.555569,                                       \
        0.831470, 0.000000, 0.555569,                                       \
        0.815144, -0.195108, 0.545412,                                      \
        0.905885, -0.195108, 0.375905,                                      \
        0.831470, 0.000000, 0.555569,                                       \
        0.905885, -0.195108, 0.375905,                                      \
        0.923880, 0.000000, 0.382682,                                       \
        0.923880, 0.000000, 0.382682,                                       \
        0.905885, -0.195108, 0.375905,                                      \
        0.961815, -0.195108, 0.191953,                                      \
        0.923880, 0.000000, 0.382682,                                       \
        0.961815, -0.195108, 0.191953,                                      \
        0.980786, 0.000000, 0.195089,                                       \
        0.980786, 0.000000, 0.195089,                                       \
        0.961815, -0.195108, 0.191953,                                      \
        0.980782, -0.195108, 0.000623,                                      \
        0.980786, 0.000000, 0.195089,                                       \
        0.980782, -0.195108, 0.000623,                                      \
        1.000000, 0.000000, -0.000002,                                      \
        1.000000, 0.000000, -0.000002,                                      \
        0.980782, -0.195108, 0.000623,                                      \
        0.962058, -0.195107, -0.190730,                                     \
        1.000000, 0.000000, -0.000002,                                      \
        0.962058, -0.195107, -0.190730,                                     \
        0.980785, -0.000000, -0.195092,                                     \
        0.980785, -0.000000, -0.195092,                                     \
        0.962058, -0.195107, -0.190730,                                     \
        0.906363, -0.195108, -0.374753,                                     \
        0.980785, -0.000000, -0.195092,                                     \
        0.906363, -0.195108, -0.374753,                                     \
        0.923879, 0.000000, -0.382685,                                      \
        0.923879, 0.000000, -0.382685,                                      \
        0.906363, -0.195108, -0.374753,                                     \
        0.815836, -0.195107, -0.544375,                                     \
        0.923879, 0.000000, -0.382685,                                      \
        0.815836, -0.195107, -0.544375,                                     \
        0.831469, 0.000000, -0.555572,                                      \
        0.831469, 0.000000, -0.555572,                                      \
        0.815836, -0.195107, -0.544375,                                     \
        0.693958, -0.195107, -0.693077,                                     \
        0.831469, 0.000000, -0.555572,                                      \
        0.693958, -0.195107, -0.693077,                                     \
        0.707106, 0.000000, -0.707108,                                      \
        0.707106, 0.000000, -0.707108,                                      \
        0.693958, -0.195107, -0.693077,                                     \
        0.545411, -0.195108, -0.815144,                                     \
        0.707106, 0.000000, -0.707108,                                      \
        0.545411, -0.195108, -0.815144,                                     \
        0.555569, 0.000000, -0.831471,                                      \
        0.555569, 0.000000, -0.831471,                                      \
        0.545411, -0.195108, -0.815144,                                     \
        0.375904, -0.195107, -0.905886,                                     \
        0.555569, 0.000000, -0.831471,                                      \
        0.375904, -0.195107, -0.905886,                                     \
        0.382681, 0.000000, -0.923880,                                      \
        0.382681, 0.000000, -0.923880,                                      \
        0.375904, -0.195107, -0.905886,                                     \
        0.191953, -0.195107, -0.961815,                                     \
        0.382681, 0.000000, -0.923880,                                      \
        0.191953, -0.195107, -0.961815,                                     \
        0.195089, 0.000000, -0.980786,                                      \
        0.195089, 0.000000, -0.980786,                                      \
        0.191953, -0.195107, -0.961815,                                     \
        0.000624, -0.195108, -0.980782,                                     \
        0.195089, 0.000000, -0.980786,                                      \
        0.000624, -0.195108, -0.980782,                                     \
        -0.000000, -0.000000, -1.000000,                                        \
        0.000624, -0.195108, -0.980782,                                     \
        0.001227, -0.382713, -0.923866,                                     \
        -0.179034, -0.382713, -0.906354,                                        \
        0.000624, -0.195108, -0.980782,                                     \
        -0.179034, -0.382713, -0.906354,                                        \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.179034, -0.382713, -0.906354,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.190728, -0.195108, -0.962058,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.374751, -0.195108, -0.906363,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.544373, -0.195107, -0.815837,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.693075, -0.195107, -0.693959,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.815143, -0.195108, -0.545413,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.905885, -0.195108, -0.375906,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.961814, -0.195107, -0.191954,                                        \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.980782, -0.195107, -0.000625,                                        \
        -0.980782, -0.195107, -0.000625,                                        \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.980782, -0.195107, -0.000625,                                        \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.962058, -0.195107, 0.190728,                                     \
        -0.962058, -0.195107, 0.190728,                                     \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.962058, -0.195107, 0.190728,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.906363, -0.195107, 0.374752,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.815837, -0.195108, 0.544374,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.693959, -0.195108, 0.693076,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.545412, -0.195107, 0.815143,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.375906, -0.195107, 0.905885,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.001227, -0.382713, 0.923866,                                     \
        -0.191953, -0.195108, 0.961814,                                     \
        -0.001227, -0.382713, 0.923866,                                     \
        -0.000624, -0.195107, 0.980782,                                     \
        -0.000624, -0.195107, 0.980782,                                     \
        -0.001227, -0.382713, 0.923866,                                     \
        0.179035, -0.382713, 0.906354,                                      \
        -0.000624, -0.195107, 0.980782,                                     \
        0.179035, -0.382713, 0.906354,                                      \
        0.190729, -0.195107, 0.962058,                                      \
        0.190729, -0.195107, 0.962058,                                      \
        0.179035, -0.382713, 0.906354,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.190729, -0.195107, 0.962058,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.374753, -0.195107, 0.906363,                                      \
        0.374753, -0.195107, 0.906363,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.374753, -0.195107, 0.906363,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.544374, -0.195107, 0.815837,                                      \
        0.544374, -0.195107, 0.815837,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.544374, -0.195107, 0.815837,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.693076, -0.195108, 0.693959,                                      \
        0.693076, -0.195108, 0.693959,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.693076, -0.195108, 0.693959,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.815144, -0.195108, 0.545412,                                      \
        0.815144, -0.195108, 0.545412,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.815144, -0.195108, 0.545412,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.905885, -0.195108, 0.375905,                                      \
        0.905885, -0.195108, 0.375905,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.905885, -0.195108, 0.375905,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.961815, -0.195108, 0.191953,                                      \
        0.961815, -0.195108, 0.191953,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.923866, -0.382713, 0.001226,                                      \
        0.961815, -0.195108, 0.191953,                                      \
        0.923866, -0.382713, 0.001226,                                      \
        0.980782, -0.195108, 0.000623,                                      \
        0.980782, -0.195108, 0.000623,                                      \
        0.923866, -0.382713, 0.001226,                                      \
        0.906354, -0.382713, -0.179035,                                     \
        0.980782, -0.195108, 0.000623,                                      \
        0.906354, -0.382713, -0.179035,                                     \
        0.962058, -0.195107, -0.190730,                                     \
        0.962058, -0.195107, -0.190730,                                     \
        0.906354, -0.382713, -0.179035,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.962058, -0.195107, -0.190730,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.906363, -0.195108, -0.374753,                                     \
        0.906363, -0.195108, -0.374753,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.906363, -0.195108, -0.374753,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.815836, -0.195107, -0.544375,                                     \
        0.815836, -0.195107, -0.544375,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.815836, -0.195107, -0.544375,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.693958, -0.195107, -0.693077,                                     \
        0.693958, -0.195107, -0.693077,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.693958, -0.195107, -0.693077,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.545411, -0.195108, -0.815144,                                     \
        0.545411, -0.195108, -0.815144,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.545411, -0.195108, -0.815144,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.375904, -0.195107, -0.905886,                                     \
        0.375904, -0.195107, -0.905886,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.375904, -0.195107, -0.905886,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.191953, -0.195107, -0.961815,                                     \
        0.191953, -0.195107, -0.961815,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.001227, -0.382713, -0.923866,                                     \
        0.191953, -0.195107, -0.961815,                                     \
        0.001227, -0.382713, -0.923866,                                     \
        0.000624, -0.195108, -0.980782,                                     \
        0.001227, -0.382713, -0.923866,                                     \
        0.001784, -0.555605, -0.831445,                                     \
        -0.160456, -0.555605, -0.815817,                                        \
        0.001227, -0.382713, -0.923866,                                     \
        -0.160456, -0.555605, -0.815817,                                        \
        -0.179034, -0.382713, -0.906354,                                        \
        -0.179034, -0.382713, -0.906354,                                        \
        -0.160456, -0.555605, -0.815817,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.179034, -0.382713, -0.906354,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.352414, -0.382713, -0.854011,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.512252, -0.382713, -0.768849,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.652404, -0.382713, -0.654140,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.767485, -0.382713, -0.514293,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.853072, -0.382713, -0.354682,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.905875, -0.382713, -0.181441,                                        \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.923866, -0.382713, -0.001227,                                        \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.906354, -0.382713, 0.179034,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.854011, -0.382713, 0.352415,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.768848, -0.382713, 0.512253,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.654140, -0.382713, 0.652405,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.514293, -0.382713, 0.767485,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.354682, -0.382713, 0.853072,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.001784, -0.555605, 0.831445,                                     \
        -0.181440, -0.382713, 0.905875,                                     \
        -0.001784, -0.555605, 0.831445,                                     \
        -0.001227, -0.382713, 0.923866,                                     \
        -0.001227, -0.382713, 0.923866,                                     \
        -0.001784, -0.555605, 0.831445,                                     \
        0.160457, -0.555605, 0.815817,                                      \
        -0.001227, -0.382713, 0.923866,                                     \
        0.160457, -0.555605, 0.815817,                                      \
        0.179035, -0.382713, 0.906354,                                      \
        0.179035, -0.382713, 0.906354,                                      \
        0.160457, -0.555605, 0.815817,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.179035, -0.382713, 0.906354,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.352415, -0.382713, 0.854011,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.512253, -0.382713, 0.768848,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.652405, -0.382713, 0.654139,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.767486, -0.382713, 0.514292,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.853072, -0.382713, 0.354681,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.831445, -0.555605, 0.001783,                                      \
        0.905875, -0.382713, 0.181440,                                      \
        0.831445, -0.555605, 0.001783,                                      \
        0.923866, -0.382713, 0.001226,                                      \
        0.923866, -0.382713, 0.001226,                                      \
        0.831445, -0.555605, 0.001783,                                      \
        0.815816, -0.555605, -0.160458,                                     \
        0.923866, -0.382713, 0.001226,                                      \
        0.815816, -0.555605, -0.160458,                                     \
        0.906354, -0.382713, -0.179035,                                     \
        0.906354, -0.382713, -0.179035,                                     \
        0.815816, -0.555605, -0.160458,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.906354, -0.382713, -0.179035,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.854010, -0.382713, -0.352416,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.768848, -0.382713, -0.512254,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.654139, -0.382713, -0.652406,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.514292, -0.382713, -0.767486,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.354681, -0.382713, -0.853072,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.001784, -0.555605, -0.831445,                                     \
        0.181440, -0.382713, -0.905875,                                     \
        0.001784, -0.555605, -0.831445,                                     \
        0.001227, -0.382713, -0.923866,                                     \
        0.001784, -0.555605, -0.831445,                                     \
        0.002275, -0.707138, -0.707072,                                     \
        -0.135711, -0.707138, -0.693930,                                        \
        0.001784, -0.555605, -0.831445,                                     \
        -0.135711, -0.707138, -0.693930,                                        \
        -0.160456, -0.555605, -0.815817,                                        \
        -0.160456, -0.555605, -0.815817,                                        \
        -0.135711, -0.707138, -0.693930,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.160456, -0.555605, -0.815817,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.316531, -0.555605, -0.768838,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.460442, -0.555605, -0.692312,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.586658, -0.555605, -0.589182,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.690329, -0.555605, -0.463410,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.767472, -0.555605, -0.319829,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.815121, -0.555605, -0.163957,                                        \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.831445, -0.555605, -0.001784,                                        \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.815817, -0.555605, 0.160457,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.768837, -0.555605, 0.316532,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.692312, -0.555605, 0.460442,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.589182, -0.555605, 0.586659,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.463409, -0.555605, 0.690330,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.319828, -0.555605, 0.767472,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.002275, -0.707138, 0.707072,                                     \
        -0.163956, -0.555605, 0.815121,                                     \
        -0.002275, -0.707138, 0.707072,                                     \
        -0.001784, -0.555605, 0.831445,                                     \
        -0.001784, -0.555605, 0.831445,                                     \
        -0.002275, -0.707138, 0.707072,                                     \
        0.135712, -0.707138, 0.693930,                                      \
        -0.001784, -0.555605, 0.831445,                                     \
        0.135712, -0.707138, 0.693930,                                      \
        0.160457, -0.555605, 0.815817,                                      \
        0.160457, -0.555605, 0.815817,                                      \
        0.135712, -0.707138, 0.693930,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.160457, -0.555605, 0.815817,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.316532, -0.555605, 0.768837,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.460443, -0.555605, 0.692312,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.586659, -0.555605, 0.589181,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.690330, -0.555605, 0.463409,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.767472, -0.555605, 0.319828,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.707072, -0.707138, 0.002274,                                      \
        0.815121, -0.555605, 0.163956,                                      \
        0.707072, -0.707138, 0.002274,                                      \
        0.831445, -0.555605, 0.001783,                                      \
        0.831445, -0.555605, 0.001783,                                      \
        0.707072, -0.707138, 0.002274,                                      \
        0.693930, -0.707138, -0.135712,                                     \
        0.831445, -0.555605, 0.001783,                                      \
        0.693930, -0.707138, -0.135712,                                     \
        0.815816, -0.555605, -0.160458,                                     \
        0.815816, -0.555605, -0.160458,                                     \
        0.693930, -0.707138, -0.135712,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.815816, -0.555605, -0.160458,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.768837, -0.555605, -0.316533,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.692312, -0.555605, -0.460443,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.589181, -0.555605, -0.586659,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.463408, -0.555605, -0.690330,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.319827, -0.555605, -0.767472,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.002275, -0.707138, -0.707072,                                     \
        0.163956, -0.555605, -0.815121,                                     \
        0.002275, -0.707138, -0.707072,                                     \
        0.001784, -0.555605, -0.831445,                                     \
        0.002275, -0.707138, -0.707072,                                     \
        0.002680, -0.831490, -0.555533,                                     \
        -0.105750, -0.831490, -0.545381,                                        \
        0.002275, -0.707138, -0.707072,                                     \
        -0.105750, -0.831490, -0.545381,                                        \
        -0.135711, -0.707138, -0.693930,                                        \
        -0.135711, -0.707138, -0.693930,                                        \
        -0.105750, -0.831490, -0.545381,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.135711, -0.707138, -0.693930,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.268483, -0.707138, -0.654120,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.390936, -0.707138, -0.589173,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.498367, -0.707138, -0.501585,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.586645, -0.707138, -0.394720,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.652379, -0.707138, -0.272687,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.693042, -0.707138, -0.140175,                                        \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.707072, -0.707138, -0.002275,                                        \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.693930, -0.707138, 0.135711,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.654120, -0.707138, 0.268483,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.589173, -0.707138, 0.390936,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.501584, -0.707138, 0.498367,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.394720, -0.707138, 0.586645,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.272687, -0.707138, 0.652379,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.002680, -0.831490, 0.555533,                                     \
        -0.140174, -0.707138, 0.693042,                                     \
        -0.002680, -0.831490, 0.555533,                                     \
        -0.002275, -0.707138, 0.707072,                                     \
        -0.002275, -0.707138, 0.707072,                                     \
        -0.002680, -0.831490, 0.555533,                                     \
        0.105750, -0.831490, 0.545381,                                      \
        -0.002275, -0.707138, 0.707072,                                     \
        0.105750, -0.831490, 0.545381,                                      \
        0.135712, -0.707138, 0.693930,                                      \
        0.135712, -0.707138, 0.693930,                                      \
        0.105750, -0.831490, 0.545381,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.135712, -0.707138, 0.693930,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.268483, -0.707138, 0.654120,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.390937, -0.707138, 0.589173,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.498367, -0.707138, 0.501584,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.586645, -0.707138, 0.394719,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.652379, -0.707138, 0.272686,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.555533, -0.831490, 0.002680,                                      \
        0.693042, -0.707138, 0.140174,                                      \
        0.555533, -0.831490, 0.002680,                                      \
        0.707072, -0.707138, 0.002274,                                      \
        0.707072, -0.707138, 0.002274,                                      \
        0.555533, -0.831490, 0.002680,                                      \
        0.545381, -0.831490, -0.105751,                                     \
        0.707072, -0.707138, 0.002274,                                      \
        0.545381, -0.831490, -0.105751,                                     \
        0.693930, -0.707138, -0.135712,                                     \
        0.693930, -0.707138, -0.135712,                                     \
        0.545381, -0.831490, -0.105751,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.693930, -0.707138, -0.135712,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.654120, -0.707138, -0.268484,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.589173, -0.707138, -0.390937,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.501584, -0.707138, -0.498367,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.394719, -0.707138, -0.586646,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.272686, -0.707138, -0.652379,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.002680, -0.831490, -0.555533,                                     \
        0.140174, -0.707138, -0.693042,                                     \
        0.002680, -0.831490, -0.555533,                                     \
        0.002275, -0.707138, -0.707072,                                     \
        0.002680, -0.831490, -0.555533,                                     \
        0.002983, -0.923888, -0.382651,                                     \
        -0.071726, -0.923888, -0.375880,                                        \
        0.002680, -0.831490, -0.555533,                                     \
        -0.071726, -0.923888, -0.375880,                                        \
        -0.105750, -0.831490, -0.545381,                                        \
        -0.105750, -0.831490, -0.545381,                                        \
        -0.071726, -0.923888, -0.375880,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.105750, -0.831490, -0.545381,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.210117, -0.831490, -0.514271,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.306408, -0.831490, -0.463398,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.390925, -0.831490, -0.394716,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.460419, -0.831490, -0.310866,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.512219, -0.831490, -0.215070,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.544335, -0.831490, -0.111008,                                        \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.555533, -0.831490, -0.002680,                                        \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.545381, -0.831490, 0.105750,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.514271, -0.831490, 0.210117,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.463398, -0.831490, 0.306409,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.394716, -0.831490, 0.390926,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.310866, -0.831490, 0.460419,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.215069, -0.831491, 0.512219,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.002983, -0.923888, 0.382651,                                     \
        -0.111008, -0.831490, 0.544335,                                     \
        -0.002983, -0.923888, 0.382651,                                     \
        -0.002680, -0.831490, 0.555533,                                     \
        -0.002680, -0.831490, 0.555533,                                     \
        -0.002983, -0.923888, 0.382651,                                     \
        0.071726, -0.923888, 0.375880,                                      \
        -0.002680, -0.831490, 0.555533,                                     \
        0.071726, -0.923888, 0.375880,                                      \
        0.105750, -0.831490, 0.545381,                                      \
        0.105750, -0.831490, 0.545381,                                      \
        0.071726, -0.923888, 0.375880,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.105750, -0.831490, 0.545381,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.210117, -0.831490, 0.514271,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.306409, -0.831490, 0.463397,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.390926, -0.831490, 0.394716,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.460420, -0.831490, 0.310866,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.512220, -0.831490, 0.215069,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.382651, -0.923888, 0.002983,                                      \
        0.544335, -0.831490, 0.111007,                                      \
        0.382651, -0.923888, 0.002983,                                      \
        0.555533, -0.831490, 0.002680,                                      \
        0.555533, -0.831490, 0.002680,                                      \
        0.382651, -0.923888, 0.002983,                                      \
        0.375880, -0.923888, -0.071726,                                     \
        0.555533, -0.831490, 0.002680,                                      \
        0.375880, -0.923888, -0.071726,                                     \
        0.545381, -0.831490, -0.105751,                                     \
        0.545381, -0.831490, -0.105751,                                     \
        0.375880, -0.923888, -0.071726,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.545381, -0.831490, -0.105751,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.514271, -0.831490, -0.210117,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.463397, -0.831491, -0.306409,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.394716, -0.831491, -0.390926,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.310865, -0.831490, -0.460420,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.215069, -0.831490, -0.512220,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.002983, -0.923888, -0.382651,                                     \
        0.111007, -0.831490, -0.544335,                                     \
        0.002983, -0.923888, -0.382651,                                     \
        0.002680, -0.831490, -0.555533,                                     \
        0.002983, -0.923888, -0.382651,                                     \
        0.005744, -0.976740, -0.214348,                                     \
        -0.036184, -0.976740, -0.211350,                                        \
        0.002983, -0.923888, -0.382651,                                     \
        -0.036184, -0.976740, -0.211350,                                        \
        -0.071726, -0.923888, -0.375880,                                        \
        -0.071726, -0.923888, -0.375880,                                        \
        -0.036184, -0.976740, -0.211350,                                        \
        -0.076721, -0.976740, -0.200230,                                        \
        -0.071726, -0.923888, -0.375880,                                        \
        -0.076721, -0.976740, -0.200230,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.076721, -0.976740, -0.200230,                                        \
        -0.114310, -0.976740, -0.181415,                                        \
        -0.143678, -0.923888, -0.354665,                                        \
        -0.114310, -0.976740, -0.181415,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.114310, -0.976740, -0.181415,                                        \
        -0.147505, -0.976740, -0.155628,                                        \
        -0.210109, -0.923888, -0.319820,                                        \
        -0.147505, -0.976740, -0.155628,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.147505, -0.976740, -0.155628,                                        \
        -0.175033, -0.976740, -0.123861,                                        \
        -0.268466, -0.923888, -0.272685,                                        \
        -0.175033, -0.976740, -0.123861,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.175033, -0.976740, -0.123861,                                        \
        -0.195834, -0.976740, -0.087334,                                        \
        -0.316505, -0.923888, -0.215070,                                        \
        -0.195834, -0.976740, -0.087334,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.195834, -0.976740, -0.087334,                                        \
        -0.209109, -0.976740, -0.047451,                                        \
        -0.352382, -0.923888, -0.149190,                                        \
        -0.209109, -0.976740, -0.047451,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.209109, -0.976740, -0.047451,                                        \
        -0.214348, -0.976740, -0.005744,                                        \
        -0.374717, -0.923888, -0.077577,                                        \
        -0.214348, -0.976740, -0.005744,                                        \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.214348, -0.976740, -0.005744,                                        \
        -0.211350, -0.976740, 0.036184,                                     \
        -0.382651, -0.923888, -0.002983,                                        \
        -0.211350, -0.976740, 0.036184,                                     \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.211350, -0.976740, 0.036184,                                     \
        -0.200230, -0.976740, 0.076721,                                     \
        -0.375880, -0.923888, 0.071726,                                     \
        -0.200230, -0.976740, 0.076721,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.200230, -0.976740, 0.076721,                                     \
        -0.181415, -0.976740, 0.114310,                                     \
        -0.354665, -0.923888, 0.143678,                                     \
        -0.181415, -0.976740, 0.114310,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.181415, -0.976740, 0.114310,                                     \
        -0.155628, -0.976740, 0.147506,                                     \
        -0.319820, -0.923888, 0.210109,                                     \
        -0.155628, -0.976740, 0.147506,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.155628, -0.976740, 0.147506,                                     \
        -0.123861, -0.976740, 0.175033,                                     \
        -0.272684, -0.923888, 0.268466,                                     \
        -0.123861, -0.976740, 0.175033,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.123861, -0.976740, 0.175033,                                     \
        -0.087334, -0.976740, 0.195834,                                     \
        -0.215070, -0.923888, 0.316506,                                     \
        -0.087334, -0.976740, 0.195834,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.087334, -0.976740, 0.195834,                                     \
        -0.047450, -0.976740, 0.209109,                                     \
        -0.149190, -0.923888, 0.352382,                                     \
        -0.047450, -0.976740, 0.209109,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.047450, -0.976740, 0.209109,                                     \
        -0.005744, -0.976740, 0.214348,                                     \
        -0.077577, -0.923888, 0.374717,                                     \
        -0.005744, -0.976740, 0.214348,                                     \
        -0.002983, -0.923888, 0.382651,                                     \
        -0.002983, -0.923888, 0.382651,                                     \
        -0.005744, -0.976740, 0.214348,                                     \
        0.036184, -0.976740, 0.211350,                                      \
        -0.002983, -0.923888, 0.382651,                                     \
        0.036184, -0.976740, 0.211350,                                      \
        0.071726, -0.923888, 0.375880,                                      \
        0.071726, -0.923888, 0.375880,                                      \
        0.036184, -0.976740, 0.211350,                                      \
        0.076721, -0.976740, 0.200230,                                      \
        0.071726, -0.923888, 0.375880,                                      \
        0.076721, -0.976740, 0.200230,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.076721, -0.976740, 0.200230,                                      \
        0.114310, -0.976740, 0.181415,                                      \
        0.143679, -0.923888, 0.354665,                                      \
        0.114310, -0.976740, 0.181415,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.114310, -0.976740, 0.181415,                                      \
        0.147506, -0.976740, 0.155628,                                      \
        0.210109, -0.923888, 0.319820,                                      \
        0.147506, -0.976740, 0.155628,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.147506, -0.976740, 0.155628,                                      \
        0.175033, -0.976740, 0.123861,                                      \
        0.268466, -0.923888, 0.272684,                                      \
        0.175033, -0.976740, 0.123861,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.175033, -0.976740, 0.123861,                                      \
        0.195834, -0.976740, 0.087334,                                      \
        0.316506, -0.923888, 0.215069,                                      \
        0.195834, -0.976740, 0.087334,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.195834, -0.976740, 0.087334,                                      \
        0.209109, -0.976740, 0.047450,                                      \
        0.352382, -0.923888, 0.149190,                                      \
        0.209109, -0.976740, 0.047450,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.209109, -0.976740, 0.047450,                                      \
        0.214348, -0.976740, 0.005743,                                      \
        0.374717, -0.923888, 0.077577,                                      \
        0.214348, -0.976740, 0.005743,                                      \
        0.382651, -0.923888, 0.002983,                                      \
        0.382651, -0.923888, 0.002983,                                      \
        0.214348, -0.976740, 0.005743,                                      \
        0.211350, -0.976740, -0.036184,                                     \
        0.382651, -0.923888, 0.002983,                                      \
        0.211350, -0.976740, -0.036184,                                     \
        0.375880, -0.923888, -0.071726,                                     \
        0.375880, -0.923888, -0.071726,                                     \
        0.211350, -0.976740, -0.036184,                                     \
        0.200230, -0.976740, -0.076721,                                     \
        0.375880, -0.923888, -0.071726,                                     \
        0.200230, -0.976740, -0.076721,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.200230, -0.976740, -0.076721,                                     \
        0.181415, -0.976740, -0.114310,                                     \
        0.354665, -0.923888, -0.143679,                                     \
        0.181415, -0.976740, -0.114310,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.181415, -0.976740, -0.114310,                                     \
        0.155628, -0.976740, -0.147506,                                     \
        0.319820, -0.923888, -0.210110,                                     \
        0.155628, -0.976740, -0.147506,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.155628, -0.976740, -0.147506,                                     \
        0.123861, -0.976740, -0.175033,                                     \
        0.272684, -0.923888, -0.268466,                                     \
        0.123861, -0.976740, -0.175033,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.123861, -0.976740, -0.175033,                                     \
        0.087334, -0.976740, -0.195834,                                     \
        0.215069, -0.923888, -0.316506,                                     \
        0.087334, -0.976740, -0.195834,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.087334, -0.976740, -0.195834,                                     \
        0.047450, -0.976740, -0.209109,                                     \
        0.149190, -0.923888, -0.352382,                                     \
        0.047450, -0.976740, -0.209109,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.047450, -0.976740, -0.209109,                                     \
        0.005744, -0.976740, -0.214348,                                     \
        0.077577, -0.923888, -0.374717,                                     \
        0.005744, -0.976740, -0.214348,                                     \
        0.002983, -0.923888, -0.382651,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.036184, -0.976740, -0.211350,                                        \
        0.005744, -0.976740, -0.214348,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.076721, -0.976740, -0.200230,                                        \
        -0.036184, -0.976740, -0.211350,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.114310, -0.976740, -0.181415,                                        \
        -0.076721, -0.976740, -0.200230,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.147505, -0.976740, -0.155628,                                        \
        -0.114310, -0.976740, -0.181415,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.175033, -0.976740, -0.123861,                                        \
        -0.147505, -0.976740, -0.155628,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.195834, -0.976740, -0.087334,                                        \
        -0.175033, -0.976740, -0.123861,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.209109, -0.976740, -0.047451,                                        \
        -0.195834, -0.976740, -0.087334,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.214348, -0.976740, -0.005744,                                        \
        -0.209109, -0.976740, -0.047451,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.211350, -0.976740, 0.036184,                                     \
        -0.214348, -0.976740, -0.005744,                                        \
        0.000000, -1.000000, -0.000000,                                     \
        -0.200230, -0.976740, 0.076721,                                     \
        -0.211350, -0.976740, 0.036184,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.181415, -0.976740, 0.114310,                                     \
        -0.200230, -0.976740, 0.076721,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.155628, -0.976740, 0.147506,                                     \
        -0.181415, -0.976740, 0.114310,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.123861, -0.976740, 0.175033,                                     \
        -0.155628, -0.976740, 0.147506,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.087334, -0.976740, 0.195834,                                     \
        -0.123861, -0.976740, 0.175033,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.047450, -0.976740, 0.209109,                                     \
        -0.087334, -0.976740, 0.195834,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        -0.005744, -0.976740, 0.214348,                                     \
        -0.047450, -0.976740, 0.209109,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.036184, -0.976740, 0.211350,                                      \
        -0.005744, -0.976740, 0.214348,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.076721, -0.976740, 0.200230,                                      \
        0.036184, -0.976740, 0.211350,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.114310, -0.976740, 0.181415,                                      \
        0.076721, -0.976740, 0.200230,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.147506, -0.976740, 0.155628,                                      \
        0.114310, -0.976740, 0.181415,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.175033, -0.976740, 0.123861,                                      \
        0.147506, -0.976740, 0.155628,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.195834, -0.976740, 0.087334,                                      \
        0.175033, -0.976740, 0.123861,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.209109, -0.976740, 0.047450,                                      \
        0.195834, -0.976740, 0.087334,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.214348, -0.976740, 0.005743,                                      \
        0.209109, -0.976740, 0.047450,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.211350, -0.976740, -0.036184,                                     \
        0.214348, -0.976740, 0.005743,                                      \
        0.000000, -1.000000, -0.000000,                                     \
        0.200230, -0.976740, -0.076721,                                     \
        0.211350, -0.976740, -0.036184,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.181415, -0.976740, -0.114310,                                     \
        0.200230, -0.976740, -0.076721,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.155628, -0.976740, -0.147506,                                     \
        0.181415, -0.976740, -0.114310,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.123861, -0.976740, -0.175033,                                     \
        0.155628, -0.976740, -0.147506,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.087334, -0.976740, -0.195834,                                     \
        0.123861, -0.976740, -0.175033,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.047450, -0.976740, -0.209109,                                     \
        0.087334, -0.976740, -0.195834,                                     \
        0.000000, -1.000000, -0.000000,                                     \
        0.005744, -0.976740, -0.214348,                                     \
        0.047450, -0.976740, -0.209109,                                     \
	}, _vertex_count * 3 * sizeof(float));
