#define RDE_ERROR_NO_MEMORY "Could not allocate enought memory (%d bytes) for %s.\n"
#define RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED "Max number of loaded %s (%d) reached.\n"
#define RDE_ERROR_NO_NULL_ALLOWED "Tried to use a NULL %s.\n"
#define RDE_ERROR_FILE_NOT_FOUND "File '%s' not found or could not open it.\n"
#define RDE_ERROR_JSON "Could not load JSON from file '%s' due to error '%s'.\n"
#define RDE_ERROR_HEAP_ALLOC_BAD_VALUE "Heap Allocs Config -> number of %s cannot be <= 0 and %d was provided.\n"
#define RDE_ERROR_MULTIPLE_ENGINE "Only one engine can be created.\n"
#define RDE_ERROR_NULL_MANDATORY_CALLBACK "User-End callback %s' is not defined, before creating the engine call 'rde_setup_initial_info(...)'.\n"
#define RDE_ERROR_UNSUPPORTED_PLATFORM "Unsupported or unimplemented platform.\n"
#define RDE_ERROR_ATLAS_SUB_TEXTURE "Could not load sub texture %s for atlas at %s.\n"
#define RDE_ERROR_BEGIN_RENDER "Tried to begin drawing again before ending the previous one or provided 'camera' or 'window' = NULL.\n"
#define RDE_ERROR_BAD_MESH_DATA "Error while creating mesh, _vertex_count = %d and _index_count = %d. _vertex_count must be >= 3 and _index_count >= 3.\n"
#define RDE_ERROR_SDL_WINDOW "SDL window creation failed: %s.\n"
#define RDE_ERROR_SDL_OPENGL "OpenGL context couldn't initialize: %s.\n"
#define RDE_ERROR_GLAD_INIT "Failed to initialize GLAD.\n"
#define RDE_ERROR_SANITIZATION_PATH "Path '%s' is greater than the output sanitized buffer size '%u'\n"
#define RDE_ERROR_MESSAGE_BOX "Only 'RDE_LOG_LEVEL_INFO', 'RDE_LOG_LEVEL_WARNING', 'RDE_LOG_LEVEL_ERROR' work on rde_engine_show_message_box. Defaulting to 'RDE_LOG_LEVEL_INFO'"
#define RDE_ERROR_FEATURE_NOT_SUPPORTED_YET "Feature '%s' is not supported or implemented yet."
#define RDE_ERROR_EVENT_NOT_HANDLED "%s Event: %i, not handled."

#ifdef RDE_AUDIO_MODULE
#define RDE_ERROR_MA_CONTEXT "Failed to initialize context.\n"
#define RDE_ERROR_MA_DEVICE_INFO "Failed to retrieve device information.\n"
#define RDE_ERROR_MA_DEVICE_INIT "Could not init audio device. Error code for MiniAudio %d\n"
#define RDE_ERROR_MA_DEVICE_START "Could not start the audio device to play sounds. Error code for MiniAudio %d\n"
#define RDE_ERROR_MA_FILE_NOT_FOUND "Could not load sound '%s'. error code for MiniAudio %d.\n"
#endif

#ifdef RDE_RENDERING_MODULE
#define RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT "Tried to load model '%s' which has an unsupported format '%s'. Valid formats are [png,jpeg,jpg]."
#define RDE_ERROR_RENDERING_MODEL_MODULE_FORMAT_NOT_COMPILED "Tried to load model '%s' format that was not compiled as a module in RDE, to fix this error change to another format or compile RDE with module '%s'."
#define RDE_ERROR_RENDERING_MODEL_UNSUPPORTED_FORMAT "Tried to load model '%s' which has an unsupported format '%s'."
#define RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER "Framebuffer is incomplete. \n"
#endif

#ifdef RDE_FBX_MODULE
#define RDE_ERROR_FBX_COULD_NOT_LOAD "Could not load fbx file at '%s' due to error %s\n"
#define RDE_ERROR_FBX_MODEL_FACE_INDICES "Model '%s' has a face with a different amount of indices of 3 or 4, '%u'\n"
#endif

#ifdef RDE_OBJ_MODULE
#define RDE_ERROR_OBJ_COULD_NOT_LOAD "Could not load obj file at '%s', file not found\n"
#define RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD "Could not load obj material file at '%s', file not found\n"
#define RDE_ERROR_OBJ_INVALID_DATA "Some data in '%s' could not be parsed correctly, failed to parse %s at line %lu \n"
#define RDE_ERROR_OBJ_INVALID_MATERIAL_DATA "%s in '%s' with value %s could not be parsed correctly, failed to parse %s at line %lu \n"
#define RDE_WARNING_OBJ_VERTEX_INCLUDES_COLORS "Model '%s' includes vertex colors in 'v' and this is still not supported so it will be ignored \n"
#endif

#ifdef RDE_FILE_SYSTEM_MODULE
#define RDE_ERROR_FILE_SYSTEM_MAX_OPENED_FILES_REACHED "Max number of concurrent files opened '%d' by File System Modle has been reached. Please close some files before opening more. \n"
#endif

static int times_enter_in_error = 0;

#ifdef RDE_ERROR_MODULE
	#include <signal.h>

	#define RDE_MAX_STACK 100

	#if IS_WINDOWS()
	#include <dbghelp.h>
	void rde_inner_print_stack_trace(FILE* _f) {
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

	LONG WINAPI rde_inner_error_sig_handler(PEXCEPTION_POINTERS _sigfault_info) {
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
			rde_inner_print_stack_trace(stdout);
		#else
			FILE* _f = NULL;
			#if IS_WINDOWS()
				fopen_s(&_f, "rde_crash_logs.txt", "w");
			#else
				_f = fopen("rde_crash_logs.txt", "w");
			#endif
			rde_inner_print_stack_trace(_f);
			fclose(_f);

		#endif

		rde_engine_destroy_engine();
		exit(-1);

		return EXCEPTION_CONTINUE_SEARCH;
	}
	#else
	#define RDE_STACKTRACE_MAX_DEPTH 1024
	// Same value as SIGSTKSZ
	#define RDE_STACKTRACE_BUFF_SIZE 13504
	#define RDE_STACKTRACE_PRINT_BUFF_SIZE 1024
	#define RDE_STACKTRACE_PRINT_LINE_BUFF_SIZE 2048

	#include <execinfo.h>
	#include <dlfcn.h>
	#include <err.h>

	typedef struct {
	    char* buf;
	    int pos;
	    int size;
	} rde_print_buf;

	typedef struct rde_posix_stacktrace {
	    void* trace[RDE_STACKTRACE_MAX_DEPTH];
	    int trace_size;
	} rde_posix_stacktrace;

	static void rde_inner_buf_printf(FILE* _f, const char* _fmt, ...) {
	    va_list _args;
	    va_start(_args, _fmt);
	    vfprintf(_f, _fmt, _args);
	    va_end(_args);
	}

	void rde_inner_print_stack_trace(FILE* _f) {
		rde_posix_stacktrace* _ret = (rde_posix_stacktrace*)malloc(sizeof(rde_posix_stacktrace));
	    _ret->trace_size = backtrace(_ret->trace, RDE_STACKTRACE_MAX_DEPTH);
	    const rde_posix_stacktrace* _stacktrace = (rde_posix_stacktrace*)(_ret);

	    char** _messages = backtrace_symbols(_stacktrace->trace, _stacktrace->trace_size);
	    rde_print_buf _out = { (char*) malloc(RDE_STACKTRACE_PRINT_BUFF_SIZE), 0, RDE_STACKTRACE_PRINT_BUFF_SIZE };

	    for (int i = 0; i < _stacktrace->trace_size; ++i) {
	        void* _tracei = _stacktrace->trace[i];
	        char* _msg = _messages[i];

	        /* calculate load offset */
	        Dl_info _info;
	        dladdr(_tracei, &_info);
	        if (_info.dli_fbase == (void*)0x400000) {
	            /* address from executable, so don't offset */
	            _info.dli_fbase = NULL;
	        }

	        while (*_msg && *_msg != '(') {
	        	 ++_msg;
	        }

	        *_msg = 0;
	        char _cmd[RDE_STACKTRACE_PRINT_BUFF_SIZE];
	        char _line[RDE_STACKTRACE_PRINT_LINE_BUFF_SIZE];

	        FILE* _fp;
	        snprintf(_cmd, RDE_STACKTRACE_PRINT_BUFF_SIZE, "addr2line -e %s -f -C -p %p 2>/dev/null", _messages[i], (void*)((char*)_tracei - (char*)_info.dli_fbase));

	        _fp = popen(_cmd, "r");
	        if (!_fp) {
	            rde_inner_buf_printf(_f, "Failed to generate trace further...\n");
	            break;
	        }

	        while (fgets(_line, sizeof(_line), _fp)) {
	            rde_inner_buf_printf(_f, "%s: ", _messages[i]);
	            if (strstr(_line, "?? ")) {
	                /* just output address if nothing can be found */
	                rde_inner_buf_printf(_f, "%p\n", _tracei);
	            }
	            else {
	                rde_inner_buf_printf(_f, "%s", _line);
	            }
	        }

	        pclose(_fp);
	    }

	    free(_messages);
	    free(_ret);

	    printf("%s \n", _out.buf);
	}

	void rde_inner_posix_signal_handler(int _sig, siginfo_t* _sig_info, void* _context) {
		(void)_context;

		FILE* _f = NULL;
	#ifdef RDE_DEBUG
	  	_f = stdout;
	#else
		_f = fopen("rde_crash_logs.txt", "w");
	#endif
	  
		switch(_sig) {
			case SIGSEGV:
				fprintf(_f, "Caught SIGSEGV: Segmentation Fault\n");
				break;
			case SIGINT:
				fprintf(_f, "stderr, Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n");
				break;
			case SIGFPE:
				switch(_sig_info->si_code) {
					case FPE_INTDIV:
						fprintf(_f, "Caught SIGFPE: (integer divide by zero)\n");
						break;
					case FPE_INTOVF:
						fprintf(_f, "Caught SIGFPE: (integer overflow)\n");
						break;
					case FPE_FLTDIV:
						fprintf(_f, "Caught SIGFPE: (floating-point divide by zero)\n");
						break;
					case FPE_FLTOVF:
						fprintf(_f, "Caught SIGFPE: (floating-point overflow)\n");
						break;
					case FPE_FLTUND:
						fprintf(_f, "Caught SIGFPE: (floating-point underflow)\n");
						break;
					case FPE_FLTRES:
						fprintf(_f, "Caught SIGFPE: (floating-point inexact result)\n");
						break;
					case FPE_FLTINV:
						fprintf(_f, "Caught SIGFPE: (floating-point invalid operation)\n");
						break;
					case FPE_FLTSUB:
						fprintf(_f, "Caught SIGFPE: (subscript out of range)\n");
						break;
					default:
						fprintf(_f, "Caught SIGFPE: Arithmetic Exception\n");
						break;
				}
			case SIGILL:
				switch(_sig_info->si_code) {
					case ILL_ILLOPC:
						fprintf(_f, "Caught SIGILL: (illegal opcode)\n");
						break;
					case ILL_ILLOPN:
						fprintf(_f, "Caught SIGILL: (illegal operand)\n");
						break;
					case ILL_ILLADR:
						fprintf(_f, "Caught SIGILL: (illegal addressing mode)\n");
						break;
					case ILL_ILLTRP:
						fprintf(_f, "Caught SIGILL: (illegal trap)\n");
						break;
					case ILL_PRVOPC:
						fprintf(_f, "Caught SIGILL: (privileged opcode)\n");
						break;
					case ILL_PRVREG:
						fprintf(_f, "Caught SIGILL: (privileged register)\n");
						break;
					case ILL_COPROC:
						fprintf(_f, "Caught SIGILL: (coprocessor error)\n");
						break;
					case ILL_BADSTK:
						fprintf(_f, "Caught SIGILL: (internal stack error)\n");
						break;
					default:
						fprintf(_f, "Caught SIGILL: Illegal Instruction\n");
						break;
				}
				break;
			case SIGTERM:
				fprintf(_f, "Caught SIGTERM: a termination request was sent to the program\n");
				break;
			case SIGABRT:
				fprintf(_f, "Caught SIGABRT: usually caused by an abort() or assert()\n");
				break;
			default:
				break;
		}

	  	rde_inner_print_stack_trace(_f);

	#ifndef RDE_DEBUG
		fclose(_f);
	#endif

	  exit(-1);
	}

	void rde_inner_set_posix_signal_handler() {
		stack_t _ss = {};
		uint8_t* _alternate_stack = (uint8_t*)malloc(sizeof(uint8_t) * RDE_STACKTRACE_BUFF_SIZE);
		_ss.ss_sp = (void*)_alternate_stack;
		_ss.ss_size = RDE_STACKTRACE_BUFF_SIZE;
		_ss.ss_flags = 0;

		#if !IS_MAC()
			if (sigaltstack(&_ss, NULL) != 0) { err(1, "sigaltstack"); }
		#endif

		struct sigaction _sig_action = {};
		_sig_action.sa_sigaction = rde_inner_posix_signal_handler;
		sigemptyset(&_sig_action.sa_mask);

		#ifdef __APPLE__
		    _sig_action.sa_flags = SA_SIGINFO;
		#else
		    _sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
		#endif

		if (sigaction(SIGSEGV, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGFPE,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGINT,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGILL,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGTERM, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGABRT, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
	}
	#endif
#endif




// ==============================================================================
// =									API										=
// ==============================================================================

void rde_critical_error(bool _condition, const char* _fmt, ...) {
	
	if(!_condition) {
		return;
	}

	times_enter_in_error++;

	if(times_enter_in_error > 1) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "There was an error loop, check previous message to fix it. \n");
		exit(-1);
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
	#ifdef RDE_ERROR_MODULE
	rde_inner_print_stack_trace(stdout);
	#endif
#else
	vfprintf(_f, _fmt, _args);
	#ifdef RDE_ERROR_MODULE
	rde_inner_print_stack_trace(_f);
	#endif
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