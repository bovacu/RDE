#define RDE_ERROR_NO_MEMORY "Could not allocate enought memory (%d bytes) for %s."
#define RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED "Max number of loaded %s (%d) reached."
#define RDE_ERROR_NO_NULL_ALLOWED "Tried to use a NULL %s."
#define RDE_ERROR_FILE_NOT_FOUND "File '%s' not found or could not open it."
#define RDE_ERROR_JSON "Could not load JSON from file '%s' due to error '%s'."
#define RDE_ERROR_HEAP_ALLOC_BAD_VALUE "Heap Allocs Congig -> number of %s cannot be <= 0 and %d was provided."
#define RDE_ERROR_MULTIPLE_ENGINE "Only one engine can be created."
#define RDE_ERROR_NULL_MANDATORY_CALLBACK "User-End callback %s' is not defined, before creating the engine call 'rde_setup_initial_info(...)'."
#define RDE_ERROR_UNSUPPORTED_PLATFORM "Unsupported or unimplemented platform."
#define RDE_ERROR_ATLAS_SUB_TEXTURE "Could not load sub texture %s for atlas at %s."
#define RDE_ERROR_BEGIN_RENDER "Tried to begin drawing again before ending the previous one or provided 'camera' or 'window' = NULL."
#define RDE_ERROR_BAD_MESH_DATA "Error while creating mesh, _vertex_count = %d and _index_count = %d. _vertex_count must be >= 3 and _index_count >= 3."
#define RDE_ERROR_SDL_WINDOW "SDL window creation failed: %s."
#define RDE_ERROR_SDL_OPENGL "OpenGL context couldn't initialize: %s."
#define RDE_ERROR_GLAD_INIT "Failed to initialize GLAD."

#ifdef RDE_AUDIO_MODULE
#define RDE_ERROR_MA_CONTEXT "Failed to initialize context."
#define RDE_ERROR_MA_DEVICE_INFO "Failed to retrieve device information."
#define RDE_ERROR_MA_DEVICE_INIT "Could not init audio device. Error code for MiniAudio %d"
#define RDE_ERROR_MA_DEVICE_START "Could not start the audio device to play sounds. Error code for MiniAudio %d"
#define RDE_ERROR_MA_FILE_NOT_FOUND "Could not load sound '%s'. error code for MiniAudio %d."
#endif

#ifdef RDE_FBX_MODULE
#define RDE_ERROR_FBX_COULD_NOT_LOAD "Could not load fbx file at '%s' due to error %s"
#define RDE_ERROR_FBX_MODEL_FACE_INDICES "Model '%s' has a face with a different amount of indices of 3 or 4, '%u'"
#endif

#ifdef RDE_OBJ_MODULE
#define RDE_ERROR_OBJ_COULD_NOT_LOAD "Could not load obj file at '%s', file not found"
#endif

#ifdef RDE_ERROR_MODULE
#include <signal.h>

#define RDE_MAX_STACK 100

#if IS_WINDOWS()
#include <dbghelp.h>
void rde_print_stack_trace(FILE* _f) {
	void* _stack[RDE_MAX_STACK];
	HANDLE _process = GetCurrentProcess();

	SymInitialize(_process, NULL, TRUE);

	unsigned short _frames = CaptureStackBackTrace(0, RDE_MAX_STACK, _stack, NULL);
	SYMBOL_INFO* _symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	IMAGEHLP_LINE* _line = (IMAGEHLP_LINE *)malloc(sizeof(IMAGEHLP_LINE));
	_line->SizeOfStruct = sizeof(IMAGEHLP_LINE);

	_symbol->MaxNameLen = 255;
	_symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	for(int _i = 0; _i < _frames; _i++) {
		SymFromAddr(_process, (DWORD64)(_stack[_i] ), 0, _symbol);
		DWORD _dwDisplacement;
		SymGetLineFromAddr(_process, (DWORD64)(_stack[_i]), &_dwDisplacement, _line);

		if(strstr(_line->FileName, "errors.c") != NULL) {
			continue;
		}

		fprintf(_f, "%i: %s:%s:%lu - 0x%0llX\n", _frames - _i - 1, _line->FileName, _symbol->Name, _line->LineNumber, _symbol->Address);
	}

	free(_symbol);
	free(_line);
}

LONG WINAPI rde_error_sig_handler(PEXCEPTION_POINTERS _sigfault_info) {
	printf("\n\n");
	switch(_sigfault_info->ExceptionRecord->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ACCESS_VIOLATION ERROR:The thread tried to read from or write to a virtual address for which it does not have the appropriate access.");
		} break;
		
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED ERROR: The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.");
		} break;

		case EXCEPTION_ILLEGAL_INSTRUCTION: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ILLEGAL_INSTRUCTION ERROR: The thread tried to execute an invalid instruction. ");
		} break;

		case EXCEPTION_STACK_OVERFLOW: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_STACK_OVERFLOW ERROR: The thread used up its stack.");
		} break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_FLT_DIVIDE_BY_ZERO ERROR: The thread tried to divide a floating-point value by a floating-point divisor of zero.");
		} break;

		case EXCEPTION_FLT_OVERFLOW: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_FLT_OVERFLOW ERROR: The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.");
		} break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_INT_DIVIDE_BY_ZERO ERROR: The thread tried to divide an integer value by an integer divisor of zero.");
		} break;

		case EXCEPTION_INT_OVERFLOW: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_INT_OVERFLOW ERROR: The result of an integer operation caused a carry out of the most significant bit of the result.");
		} break;

		default: {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "UNHANDLED ERROR: Got an unhadled signal with value %lu", _sigfault_info->ExceptionRecord->ExceptionCode);
		} break;
	}

	#ifdef RDE_DEBUG
		rde_print_stack_trace(stdout);
	#else
		FILE* _f = NULL;
		#if IS_WINDOWS()
			fopen_s(&_f, "rde_crash_logs.txt", "w");
		#else
			_f = fopen("rde_crash_logs.txt", "w");
		#endif
		rde_print_stack_trace(_f);
		fclose(_f);

	#endif

	rde_engine_destroy_engine();
	exit(-1);

	return EXCEPTION_CONTINUE_SEARCH;
}
#else

// TODO: Implement for Mac/Linux

#endif

#endif

void rde_critical_error(bool _condition, const char* _fmt, ...) {
	
	if(!_condition) {
		return;
	}

#ifdef RDE_DEBUG
	rde_log_level(RDE_LOG_LEVEL_ERROR, "An error made the program crash, check 'rde_crash_logs.txt'");
#else
	rde_log_level(RDE_LOG_LEVEL_ERROR, "An error made the program crash, check below");
#endif

	FILE* _f = NULL;
	
#if IS_WINDOWS()
	fopen_s(&_f, "rde_crash_logs.txt", "w");
#else
	_f = fopen("rde_crash_logs.txt", "w");
#endif

	va_list _args;
	va_start(_args, _fmt);
	
#ifdef RDE_DEBUG
	vfprintf(stdout, _fmt, _args);
	rde_print_stack_trace(stdout);
#else
	vfprintf(_f, _fmt, _args);
	rde_print_stack_trace(_f);
#endif
	va_end(_args);
	fclose(_f);
	
	rde_engine_destroy_engine();

#ifdef RDE_DEBUG
	assert(false);
#else
	exit(-1);
#endif
}