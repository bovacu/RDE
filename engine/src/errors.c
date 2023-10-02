#ifdef RDE_ERROR_MODULE

#include <signal.h>

#define RDE_MAX_STACK 100

// TODO: send the stack trace and error to the log error file also.

#if IS_WINDOWS()
#include <dbghelp.h>
void rde_print_stack_trace() {
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
		printf("%i: func -> %s, line -> %lu, file -> %s, addr -> 0x%0llX\n", _frames - _i - 1, _symbol->Name, _line->LineNumber, _line->FileName, _symbol->Address);
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

	rde_print_stack_trace();

	rde_engine_destroy_engine();
	exit(-1);

	return EXCEPTION_CONTINUE_SEARCH;
}
#else
#endif

#endif