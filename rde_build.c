#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#if !_WIN32
#define MAX_PATH 2048
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

// Fuck Windows deprecations
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define STB_DS_IMPLEMENTATION
#define STBDS_SIPHASH_2_4
#include "external/include/stb/stb_ds.h"

#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
	#include <shellapi.h>
	#include "external/include/dirent.h"
	HANDLE console_handle;
	typedef HANDLE rde_proc;
	#define rde_proc_invalid INVALID_HANDLE_VALUE
#else
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <dirent.h>
	typedef int rde_proc;
	#define rde_proc_invalid (-1)
#endif

#if __APPLE__
#include <mach-o/dyld.h>
#endif

typedef struct {
    char* str;
    unsigned int size;
} dyn_str;

typedef char** rde_command;

typedef enum { 
	RDE_LOG_COLOR_RED, 
	RDE_LOG_COLOR_GREEN, 
	RDE_LOG_COLOR_YELLOW, 
	RDE_LOG_COLOR_BLUE, 
	RDE_LOG_COLOR_PURPLE, 
	RDE_LOG_COLOR_CYAN, 
	RDE_LOG_COLOR_WHITE
} RDE_LOG_COLOR_;

typedef enum { 
	RDE_LOG_LEVEL_DEBUG,
	RDE_LOG_LEVEL_INFO,
	RDE_LOG_LEVEL_WARNING,
	RDE_LOG_LEVEL_ERROR
} RDE_LOG_LEVEL_;

#define MAX_BUILD_OPTIONS 6
#define BUILD_OPTION_ENGINE 0
#define BUILD_OPTION_TOOLS 1
#define BUILD_OPTION_EXAMPLES 2
#define BUILD_OPTION_TESTS 3
#define BUILD_OPTION_ALL 4
#define BUILD_OPTION_PROJECT 5
const char* BUILD_OPTIONS_STR[MAX_BUILD_OPTIONS] = {
	"engine",
	"tools",
	"examples",
	"tests",
	"all",
	"project"
};

#define MAX_BUILD_PLATFORM_OPTIONS 4
#define BUILD_PLATFORM_OPTION_DESKTOP 0
#define BUILD_PLATFORM_OPTION_ANDROID 1
#define BUILD_PLATFORM_OPTION_IOS 2
#define BUILD_PLATFORM_OPTION_WASM 3
const char* BUILD_PLATFORM_OPTIONS_STR[MAX_BUILD_PLATFORM_OPTIONS] = {
	"desktop",
	"android",
	"ios",
	"wasm"
};

#define MAX_TOOL_OPTIONS 3
#define TOOL_OPTION_ATLAS_GENERATOR 0
#define TOOL_OPTION_FONT_GENERATOR 1
#define TOOL_OPTION_PROJECT_GENERATOR 2
const char* TOOL_OPTIONS_STR[MAX_TOOL_OPTIONS] = {
	"atlas_generator",
	"font_generator",
	"project_generator"
};

#define MAX_SIZE_FOR_OPTIONS 64
#define MAX_SIZE_FOR_MODULES 256
#define MAX_MODULES 9
#define GOD_MODE "-DRDE_GOD"
#define STRSIZE 100
#define DEBUG_STR "debug"
#define RELEASE_STR "release"
#define SHARED_STR "shared"
#define STATIC_STR "static"

typedef enum {
	RDE_MODULES_NONE = 0,
	RDE_MODULES_AUDIO = 1,
	RDE_MODULES_PHYSICS_2D = 2,
	RDE_MODULES_PHYSICS_3D = 4,
	RDE_MODULES_RENDERING = 8,
	RDE_MODULES_FBX = 16,
	RDE_MODULES_OBJ = 32,
	RDE_MODULES_UI = 64,
	RDE_MODULES_ERROR = 128,
	RDE_MODULES_IMGUI = 256,
} RDE_MODULES_;

const char* MODULES_STR[MAX_MODULES] = {
	"audio",
	"physics_2d",
	"physics_3d",
	"rendering",
	"fbx",
	"obj",
	"ui",
	"error",
	"imgui"
};
char* MODULES_DEFINES[MAX_MODULES] = {
	"-DRDE_AUDIO_MODULE",
	"-DRDE_PHYSICS_2D_MODULE",
	"-DRDE_PHYSICS_3D_MODULE",
	"-DRDE_RENDERING_MODULE",
	"-DRDE_FBX_MODULE",
	"-DRDE_OBJ_MODULE",
	"-DRDE_UI_MODULE",
	"-DRDE_ERROR_MODULE",
	"-DRDE_IMGUI_MODULE"
};
RDE_MODULES_ modules;

char platform[MAX_SIZE_FOR_OPTIONS];
char build_type[MAX_SIZE_FOR_OPTIONS];
char lib_type[MAX_SIZE_FOR_OPTIONS];
char build[MAX_SIZE_FOR_OPTIONS];
char tool[MAX_SIZE_FOR_OPTIONS];

dyn_str** project_compile_files;
dyn_str** project_include_paths;
dyn_str** project_link_paths;
dyn_str** project_flags;
char project_name[MAX_PATH];
char android_sdk[MAX_PATH];
char android_ndk[MAX_PATH];
char android_sign[MAX_PATH];
bool android_clean;

bool is_god = false;

char builder_exe_full_path_dir[MAX_PATH];
char builder_exe_full_path[MAX_PATH];
char working_dir[MAX_PATH];

void ds_array_to_string(char** _ds_array, char* _string, size_t _string_size);

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...);
void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

const char* get_filename_extension(const char* _file_name);

bool pid_wait(rde_proc _pid);
bool run_command(rde_command _command);
int needs_rebuild(const char* _output_path, const char** _input_paths, size_t _input_paths_count);
void try_recompile(int _argc, char** _argv);

bool make_dir_if_not_exists(const char* _dir_path);
bool create_file_if_not_exists(const char* _file_path);
bool rename_file_if_exists(const char* _file, const char* _new_name);
bool remove_file_if_exists(const char* _file_path);
bool remove_dir_recursively_if_exists(const char* _file_path);

char* read_full_file_if_exists(const char* _file_path); // You are responsible for deallocating the returned memory
bool write_to_file_if_exists(const char* _file_path, const char* _contents);
char* replace_string(const char* str, const char* from, const char* to); // This function is greacefully stolen from https://creativeandcritical.net/str-replace-c, so thank you so much
bool string_starts_with(const char* _string, const char* _prefix);
bool string_ends_with(const char* _string, const char* _prefix);

bool copy_file_if_exists(const char* _file_path, const char* _new_path);
bool copy_folder_if_exists(const char* _folder_path, const char* _new_path);

bool dir_exists(const char* _dir_path);

bool has_admin_rights();

#if __APPLE__
void get_mac_version(char* _buf);
#endif

bool compile_windows_rde();
bool compile_osx_rde();
bool compile_linux_rde();
bool build_desktop_project();
bool build_android_project();
bool build_ios_project();
bool build_wasm_project();

void print_help();
void parse_arguments(int _argc, char** _argv);

void ds_array_to_string(char** _ds_array, char* _string, size_t _string_size) {
	memset(_string, 0, _string_size);
	size_t _ds_arr_size = arrlenu(_ds_array);

	for(size_t _i = 0; _i < _ds_arr_size; _i++) {
		#if _WIN32
			strcat_s(_string, _string_size, _ds_array[_i]);
		#else
			strcat(_string, _ds_array[_i]);
		#endif
	}
}

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
	switch(_color) {
		case RDE_LOG_COLOR_RED: {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 12);
			#else
			fprintf(stdout, "\033[0;31m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_GREEN:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 10);
			#else
			fprintf(stdout, "\033[0;32m");	
			#endif
		} break;
		
		case RDE_LOG_COLOR_YELLOW:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 6);
			#else
			fprintf(stdout, "\033[0;33m");	
			#endif
		} break;
		
		case RDE_LOG_COLOR_BLUE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 9);
			#else
			fprintf(stdout, "\033[0;34m");		
			#endif
		} break;
		
		case RDE_LOG_COLOR_PURPLE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 5);
			#else
			fprintf(stdout, "\033[0;35m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_CYAN:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 11);
			#else
			fprintf(stdout, "\033[0;36m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_WHITE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 15);
			#else
			fprintf(stdout, "\033[0;37m");
			#endif
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);

	#if _WIN32
	SetConsoleTextAttribute(console_handle, 7);
	fprintf(stdout, "\n");
	#else
	fprintf(stdout, "\033[0m \n");
	#endif
}

void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...) {
	switch(_level) {
		case RDE_LOG_LEVEL_ERROR: {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 12);
			#else
			fprintf(stdout, "\033[0;31m");
			#endif
			fprintf(stdout, "[ERROR] ");
		} break;
		
		case RDE_LOG_LEVEL_WARNING:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 6);
			#else
			fprintf(stdout, "\033[0;33m");	
			#endif
			fprintf(stdout, "[WARNING] ");
		} break;
		
		case RDE_LOG_LEVEL_DEBUG:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 11);
			#else
			fprintf(stdout, "\033[0;36m");
			#endif
			fprintf(stdout, "[DEBUG] ");
		} break;

		case RDE_LOG_LEVEL_INFO:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 7);
			#endif
			fprintf(stdout, "[INFO] ");
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);
	
	#if _WIN32
	SetConsoleTextAttribute(console_handle, 7);
	fprintf(stdout, "\n");
	#else
	fprintf(stdout, "\033[0m \n");
	#endif
}

const char* get_filename_extension(const char* _file_name) {
    const char* _dot = strrchr(_file_name, '.');
    if(!_dot || _dot == _file_name) return "";
    return _dot;
}

int needs_recompile(const char* _output_path, const char** _input_paths, size_t _input_paths_count) {
	#ifdef _WIN32
	BOOL _success;
	HANDLE _output_path_fd = CreateFile(_output_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (_output_path_fd == INVALID_HANDLE_VALUE) {
		// NOTE: if output does not exist it 100% must be rebuilt
		if (GetLastError() == ERROR_FILE_NOT_FOUND) return 1;
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not open file %s: %lu", _output_path, GetLastError());
		return -1;
	}
	FILETIME _output_path_time;
	_success = GetFileTime(_output_path_fd, NULL, NULL, &_output_path_time);
	CloseHandle(_output_path_fd);
	if (!_success) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not get time of %s: %lu", _output_path, GetLastError());
		return -1;
	}

	for (size_t i = 0; i < _input_paths_count; ++i) {
		const char* _input_path = _input_paths[i];
		HANDLE _input_path_fd = CreateFile(_input_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		if (_input_path_fd == INVALID_HANDLE_VALUE) {
			// NOTE: non-existing input is an error cause it is needed for building in the first place
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not open file %s: %lu", _input_path, GetLastError());
			return -1;
		}
		FILETIME _input_path_time;
		_success = GetFileTime(_input_path_fd, NULL, NULL, &_input_path_time);
		CloseHandle(_input_path_fd);
		if (!_success) {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not get time of %s: %lu", _input_path, GetLastError());
			return -1;
		}

		// NOTE: if even a single input_path is fresher than output_path that's 100% rebuild
		if (CompareFileTime(&_input_path_time, &_output_path_time) == 1) return 1;
	}

	return 0;
	#else
	struct stat _statbuf = {0};

	if (stat(_output_path, &_statbuf) < 0) {
		// NOTE: if output does not exist it 100% must be rebuilt
		if (errno == ENOENT) return 1;
		rde_log_level(RDE_LOG_LEVEL_ERROR, "could not stat %s: %s", _output_path, strerror(errno));
		return -1;
	}
	int _output_path_time = _statbuf.st_mtime;

	for (size_t i = 0; i < _input_paths_count; ++i) {
		const char* _input_path = _input_paths[i];
		if (stat(_input_path, &_statbuf) < 0) {
			// NOTE: non-existing input is an error cause it is needed for building in the first place
			rde_log_level(RDE_LOG_LEVEL_ERROR, "could not stat %s: %s", _input_path, strerror(errno));
			return -1;
		}
		int input_path_time = _statbuf.st_mtime;
		// NOTE: if even a single input_path is fresher than output_path that's 100% rebuild
		rde_log_level(RDE_LOG_LEVEL_INFO, "Here needs recompile -> %s", _input_path); 
		if (input_path_time > _output_path_time) return 1;
	}

	return 0;
	#endif
}

bool pid_wait(rde_proc _pid) {
	errno = 0;
    if (_pid == rde_proc_invalid) {
		exit(-1);
	}

	#ifdef _WIN32
    DWORD _result = WaitForSingleObject(_pid, INFINITE);

    if (_result == WAIT_FAILED) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "could not wait on child process: %lu", GetLastError());
		CloseHandle(_pid);
		return false;
    }

    DWORD _exit_status;
    if (!GetExitCodeProcess(_pid, &_exit_status)) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "could not get process exit code: %lu", GetLastError());
		CloseHandle(_pid);
		return false;
    }

    if (_exit_status != 0) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "command exited with exit code %lu", _exit_status);
    }

    CloseHandle(_pid);
	return _exit_status == 0;
	#else
    for (;;) {
        int _wstatus = 0;
        if (waitpid(_pid, &_wstatus, 0) < 0) {
            rde_log_level(RDE_LOG_LEVEL_ERROR, "could not wait on command (pid %d): %s", _pid, strerror(errno));
			return false;
        }

        if (WIFEXITED(_wstatus)) {
            int _exit_status = WEXITSTATUS(_wstatus);
            if (_exit_status != 0) {
                rde_log_level(RDE_LOG_LEVEL_ERROR, "command exited with exit code %d", _exit_status);
            }

            return _exit_status == 0;
        }

        if (WIFSIGNALED(_wstatus)) {
            rde_log_level(RDE_LOG_LEVEL_ERROR, "command process was terminated by %s", strsignal(WTERMSIG(_wstatus)));
			return false;
        }
    }
	#endif
}

char* ltrim(char* _s) {
    while(isspace(*_s)) _s++;
    return _s;
}

char* rtrim(char* _s) {
    char* _back = _s + strlen(_s);
    while(isspace(*--_back));
    *(_back+1) = '\0';
    return _s;
}

char* trim(char* _s) {
    return rtrim(ltrim(_s)); 
}

bool run_command(rde_command _command) {
	errno = 0;
    if (arrlen(_command) < 1) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not run empty command");
        return false;
    }

#if _WIN32
    char* _command_flat = (char*)malloc(sizeof(char) * 2048);
	memset(_command_flat, 0, 2048);

	for(int _i = 0; _i < arrlen(_command); _i++) {
		strcat(_command_flat, _command[_i]);
		strcat(_command_flat, " ");
	}

	printf("\n");
	rde_log_level(RDE_LOG_LEVEL_INFO, "Executing command: %s", _command_flat);
#else
	int _total_args = arrlen(_command) + 1;
	char* _argument_list[2048];

	printf("[INFO] ");
	for(int _i = 0; _i < _total_args - 1; _i++) {
		_argument_list[_i] = _command[_i];
		printf("%s ", _argument_list[_i]);
	}
	printf("\n"); 
	_argument_list[_total_args - 1] = NULL; 
#endif

#ifdef _WIN32
    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    BOOL bSuccess = CreateProcessA(NULL, _command_flat, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
	free(_command_flat);
	arrfree(_command);

    if (!bSuccess) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not create child process: %lu", GetLastError());
		free(_command_flat);
		arrfree(_command);
        return false;
    }

    CloseHandle(piProcInfo.hThread);

    return pid_wait(piProcInfo.hProcess);
#else
    pid_t _pid = fork();
    if (_pid < 0) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not fork child process: %s", strerror(errno));
		arrfree(_command);
		// free(_argument_list);
        return false; 
    }

    if (_pid == 0) {
        if (execvp(_argument_list[0], _argument_list) < 0) {
            rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not exec child process: %s", strerror(errno));
			arrfree(_command);
			// free(_argument_list);
            return false;
        }
		arrfree(_command);
		// free(_argument_list);
    }
	
	return pid_wait(_pid);
#endif
}

void try_recompile_and_redirect_execution(int _argc, char** _argv) {
	errno = 0;
	char* _source_path = (char*)malloc(MAX_PATH);
	memset(_source_path, 0, MAX_PATH);
	strcat(_source_path, builder_exe_full_path_dir);
	strcat(_source_path, "rde_build.c");

	int _needs_recompile = needs_recompile(builder_exe_full_path, (const char**)(&_source_path), 1);
	free(_source_path);
	if(_needs_recompile < 0) {
		exit(-1);
	}

	if(_needs_recompile) {
		// First we move the current executable to trash
		
#if _WIN32
		const char _delimiter = '\\';
#else
		const char _delimiter = '/';
#endif
		// Be careful, if no delimter is found, then _just_binary_name will be NULL and we will use _binary_path directly
		char* _just_binary_name = strrchr(builder_exe_full_path, _delimiter);

		char _to_old[MAX_PATH];
		memset(_to_old, 0, MAX_PATH);
		snprintf(_to_old, MAX_PATH, "%s", builder_exe_full_path);
		strcat(_to_old, ".old");

#if _WIN32
		char _to_trash[MAX_PATH];
		memset(_to_trash, 0, MAX_PATH);
		// This is the way of deleting the old exe in Windows
		strcat(_to_trash, "C:\\$Recycle.Bin\\");
		
		if(_just_binary_name != NULL) {
			strcat(_to_trash, _just_binary_name);
		} else {
			strcat(_to_trash, builder_exe_full_path);
		}
		strcat(_to_trash, ".old");


		if(!rename_file_if_exists(builder_exe_full_path, _to_old)) {
			exit(-1);
		}
#else
		struct passwd* _pw = getpwuid(getuid());
		const char* _homedir = _pw->pw_dir;

		char _to_trash[MAX_PATH];
		memset(_to_trash, 0, MAX_PATH);
		strcat(_to_trash, _homedir);
		strcat(_to_trash, "/.Tr/");
		
		if(!make_dir_if_not_exists(_to_trash)) {
			exit(-1);
		}

		if(_just_binary_name != NULL) {
			strcat(_to_trash, _just_binary_name);
		} else {
			strcat(_to_trash, _binary_path);
		}
		strcat(_to_trash, ".old");

		if(!rename_file_if_exists(_binary_path, _to_old)) {
			exit(-1);
		}
#endif


		// Then we recompile it
		rde_log_level(RDE_LOG_LEVEL_INFO, "Recompiling %s first", _just_binary_name != NULL ? _just_binary_name : builder_exe_full_path);
		rde_command _recompile_command = NULL;
		arrput(_recompile_command, "clang");
		arrput(_recompile_command, "-g");
		arrput(_recompile_command, "-O0");
#if _WIN32
		// This is for has_admin_rights() on Windows
		arrput(_recompile_command, "-ladvapi32");
#endif
		
		char _source_path[MAX_PATH];
		memset(_source_path, 0, MAX_PATH);
#if _WIN32
		snprintf(_source_path, MAX_PATH, "%s""rde_build.c", builder_exe_full_path_dir);
		arrput(_recompile_command, _source_path);
#else
		snprintf(_source_path, MAX_PATH, "%s/rde_build.c", builder_exe_full_path_dir);
		arrput(_recompile_command, _source_path);
#endif

		arrput(_recompile_command, "-o");

		char _out_path[MAX_PATH];
		memset(_out_path, 0, MAX_PATH);
		
#if _WIN32
		snprintf(_out_path, MAX_PATH, "%s", builder_exe_full_path);
#else
		snprintf(_out_path, MAX_PATH, "%s", builder_exe_full_path);
#endif

		arrput(_recompile_command, _out_path);

		char _old_binary_path[MAX_PATH];
		memset(_old_binary_path, 0, MAX_PATH);

		strcat(_old_binary_path, builder_exe_full_path);
		strcat(_old_binary_path, ".old");

		if(!run_command(_recompile_command)) {
			if(!rename_file_if_exists(_old_binary_path, builder_exe_full_path)) {
				exit(-1);
			}
			exit(-1);
		} else {
			rde_log_level(RDE_LOG_LEVEL_INFO, "Compiled successfully.");

			if(has_admin_rights()) {
				if(!rename_file_if_exists(_old_binary_path, _to_trash)) {
					exit(-1);
				}
			} else {
				rde_log_level(RDE_LOG_LEVEL_WARNING, "Process has no admin rights, so '%s' has to be deleted manually", _old_binary_path);
			}
		}
		rde_log_level(RDE_LOG_LEVEL_INFO, "Recompiled successfully, redirecting original command...");

		rde_command _redirect_original_command = NULL;
		arrput(_redirect_original_command, builder_exe_full_path);
		for(int _i = 1; _i < _argc; _i++) {
			arrput(_redirect_original_command, _argv[_i]);
		}

		if(!run_command(_redirect_original_command)) {
			exit(-1);
		}

		exit(0);
	}
}

bool make_dir_if_not_exists(const char* _dir_path) {
	errno = 0;
	#ifdef _WIN32
	int _result = mkdir(_dir_path);
	#else
	int _result = mkdir(_dir_path, 0777);
	#endif

    if (_result < 0) {
        if (errno == EEXIST) {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "directory '%s' exists", _dir_path);
            return true;
        }
		rde_log_level(RDE_LOG_LEVEL_ERROR, "failed to create directory '%s'. (err_msg %d) -> %s", _dir_path, strerror(errno));
        return false;
    }

	rde_log_level(RDE_LOG_LEVEL_INFO, "created directory '%s'", _dir_path);
    return true;
}

bool create_file_if_not_exists(const char* _file_path) {
	errno = 0;
	FILE* _file;
	_file = fopen(_file_path, "rb+");
	if(_file == NULL) {
		_file = fopen(_file_path, "wb");
		rde_log_level(RDE_LOG_LEVEL_INFO, "File '%s' created", _file_path);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "File '%s' exists, not creating again", _file_path);
	}

	fclose(_file);
	return true;
}

bool rename_file_if_exists(const char* _file, const char* _new_name) {
	errno = 0;
	#ifdef _WIN32
	if (!MoveFileEx(_file, _new_name, MOVEFILE_REPLACE_EXISTING)) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "could not rename %s to %s: %lu", _file, _new_name, GetLastError());
		return false;
	}
	#else
	rename(_file, _new_name);
	if (errno != 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "could not rename %s to %s: %s", _file, _new_name, strerror(errno));
		return false;
	}
	#endif
	rde_log_level(RDE_LOG_LEVEL_INFO, "renamed %s -> %s", _file, _new_name);
	return true;
}

bool remove_file_if_exists(const char* _file_path) { 
	errno = 0;
	FILE* _file = fopen(_file_path, "rb+");
	if(_file == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "tried to delete '%s' but it does not exist \n", _file_path);
		return false;
	}
	fclose(_file);

	remove(_file_path);
	if(errno != 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "could not delete file '%s'. (err_msg) -> %s \n", _file_path, strerror(errno));
		return false;
	}

	rde_log_level(RDE_LOG_LEVEL_INFO, "deleted '%s' \n", _file_path);
	return true;
}

bool remove_dir_recursively_if_exists(const char* _file_path) {
	errno = 0;
	DIR* _d = opendir(_file_path);
	size_t _path_len = strlen(_file_path);
	int _r = -1;

	if (_d) {
		struct dirent* _p;

		_r = 0;
		while (!_r && (_p = readdir(_d))) {
			int _r2 = -1;
			char* _buf;
			size_t _len;

			if (!strcmp(_p->d_name, ".") || !strcmp(_p->d_name, "..")){
				continue;
			}

			_len = _path_len + strlen(_p->d_name) + 2; 
			_buf = malloc(_len);

			if (_buf) {
				struct stat _statbuf;

				snprintf(_buf, _len, "%s/%s", _file_path, _p->d_name);
				if (!stat(_buf, &_statbuf)) {
					if (S_ISDIR(_statbuf.st_mode)){
						#if _WIN32
						_r2 = RemoveDirectoryA(_buf) ? 0 : -1;
						#else
						_r2 = remove(_buf);
						#endif
					}
					else{
						_r2 = remove(_buf);
					}
				}
				free(_buf);
			}
			_r = _r2;
		}
		closedir(_d);
	}

	if (!_r) {
		_r = rmdir(_file_path);
	}

	if(errno != 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "could not delete '%s'. (err_msg) -> %s \n", _file_path, strerror(errno));
	} else {
		rde_log_level(RDE_LOG_LEVEL_INFO, "deleted '%s' \n", _file_path);
	}

   return _r == 0;
}

char* read_full_file_if_exists(const char* _file_path) {
	FILE* _file = NULL;
	char* _text = NULL;

	_file = fopen(_file_path, "r");
	if(_file == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not read file %s. (err_msg) -> File could not be found", _file_path);
		return false;
	}

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	assert(_text != NULL && "Could not allocate memory for reading the config file");
	fread(_text, sizeof(char), _num_bytes, _file);

	return _text;
}

bool write_to_file_if_exists(const char* _file_path, const char* _contents) {
	FILE* _file = fopen(_file_path, "w");
	if(_file == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not read file %s. (err_msg) -> File could not be found", _file_path);
		return false;
	}

	size_t _contents_size = strlen(_contents);
	size_t _total_writen = fwrite(_contents, 1, _contents_size, _file);
	fclose(_file);

	if(_total_writen != _contents_size) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "The amount of bytes (%zu) written to %s differs from the size of the contents (%zu) \n", _total_writen, _file_path, _contents_size);
		return false;
	}

	return true;
}

char* replace_string(const char* str, const char* from, const char* to) {

	/* Adjust each of the below values to suit your needs. */

	/* Increment positions cache size initially by this number. */
	size_t cache_sz_inc = 16;
	/* Thereafter, each time capacity needs to be increased,
	 * multiply the increment by this factor. */
	const size_t cache_sz_inc_factor = 3;
	/* But never increment capacity by more than this number. */
	const size_t cache_sz_inc_max = 1048576;

	char *pret, *ret = NULL;
	const char *pstr2, *pstr = str;
	size_t i, count = 0;
	#if (__STDC_VERSION__ >= 199901L)
	uintptr_t *pos_cache_tmp, *pos_cache = NULL;
	#else
	ptrdiff_t *pos_cache_tmp, *pos_cache = NULL;
	#endif
	size_t cache_sz = 0;
	size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

	/* Find all matches and cache their positions. */
	while ((pstr2 = strstr(pstr, from)) != NULL) {
		count++;

		/* Increase the cache size when necessary. */
		if (cache_sz < count) {
			cache_sz += cache_sz_inc;
			pos_cache_tmp = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
			if (pos_cache_tmp == NULL) {
				goto end_repl_str;
			} else pos_cache = pos_cache_tmp;
			cache_sz_inc *= cache_sz_inc_factor;
			if (cache_sz_inc > cache_sz_inc_max) {
				cache_sz_inc = cache_sz_inc_max;
			}
		}

		pos_cache[count-1] = pstr2 - str;
		pstr = pstr2 + fromlen;
	}

	orglen = pstr - str + strlen(pstr);

	/* Allocate memory for the post-replacement string. */
	if (count > 0) {
		tolen = strlen(to);
		retlen = orglen + (tolen - fromlen) * count;
	} else	retlen = orglen;
	ret = malloc(retlen + 1);
	if (ret == NULL) {
		goto end_repl_str;
	}

	if (count == 0) {
		/* If no matches, then just duplicate the string. */
		strcpy(ret, str);
	} else {
		/* Otherwise, duplicate the string whilst performing
		 * the replacements using the position cache. */
		pret = ret;
		memcpy(pret, str, pos_cache[0]);
		pret += pos_cache[0];
		for (i = 0; i < count; i++) {
			memcpy(pret, to, tolen);
			pret += tolen;
			pstr = str + pos_cache[i] + fromlen;
			cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - fromlen;
			memcpy(pret, pstr, cpylen);
			pret += cpylen;
		}
		ret[retlen] = '\0';
	}

end_repl_str:
	/* Free the cache and return the post-replacement string,
	 * which will be NULL in the event of an error. */
	free(pos_cache);
	return ret;
}

bool string_starts_with(const char* _string, const char* _prefix) {
	if(_string == NULL || _prefix == NULL) {
		return false;
	}
	return strncmp(_string, _prefix, strlen(_prefix)) == 0;
}

bool string_ends_with(const char* _string, const char* _suffix) {
	if(_string == NULL || _suffix == NULL) {
		return false;
	}

	size_t _suffix_size = strlen(_suffix);
	size_t _string_size = strlen(_string);
	return strncmp(_string + (_string_size - _suffix_size), _suffix, _suffix_size) == 0;
}

bool copy_file_if_exists(const char* _file_path, const char* _new_path) {
	#if _WIN32
	if(CopyFile(_file_path, _new_path, FALSE) == 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not copy %s into %s. (err_msg) -> %lu.", _file_path, _new_path, GetLastError());
		return false;
	}
	#else
	rde_command _command = NULL;
	arrput(_command, "cp");
	arrput(_command, (char*)_file_path);
	arrput(_command, (char*)_new_path);
	run_command(_command);
	#endif
	rde_log_level(RDE_LOG_LEVEL_INFO, "Copied file %s to %s.", _file_path, _new_path);
	return true;
}

bool copy_folder_if_exists(const char* _dir_path, const char* _new_path) {

	if(!dir_exists(_dir_path)) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not copy %s into %s. %s does not exist.", _dir_path, _new_path);
		return false;
	}

	rde_command _command = NULL;
	arrput(_command, "cp");
	arrput(_command, "-r");
	arrput(_command, (char*)_dir_path);
	arrput(_command, (char*)_new_path);
	run_command(_command);
	rde_log_level(RDE_LOG_LEVEL_INFO, "Copied folder %s to %s.", _dir_path, _new_path);
	return true;
}

bool dir_exists(const char* _dir_path) {
	DIR* _dir = opendir(_dir_path);

	if(_dir == NULL) return false;

	closedir(_dir);
	return true;
}

bool rm_dir_if_exists(const char* _dir_path) {
	if(!dir_exists(_dir_path)) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not remove %s.", _dir_path);
		return false;
	}

	rde_command _command = NULL;
	arrput(_command, "rm");
	arrput(_command, "-rf");
	arrput(_command, (char*)_dir_path);
	run_command(_command);
	rde_log_level(RDE_LOG_LEVEL_INFO, "Removed folder %s.", _dir_path);
	return true;
}

bool has_admin_rights() {
#if _WIN32
	bool _ret = false;
	HANDLE _h_token = NULL;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &_h_token)) {
		TOKEN_ELEVATION _elevation;
		DWORD _cb_size = sizeof(TOKEN_ELEVATION);
		if(GetTokenInformation( _h_token, TokenElevation, &_elevation, sizeof(_elevation), &_cb_size)) {
			_ret = _elevation.TokenIsElevated;
		}
	}
	if(_h_token) {
		CloseHandle(_h_token);
	}
	return _ret;
#else
	return true;
#endif
}

#if __APPLE__
void get_mac_version(char* _buf) {
	FILE* _min_version_pipe = popen("sw_vers -productVersion","r");																	
	if (_min_version_pipe != NULL ) {
		int _i = 0;																					
		while (!feof(_min_version_pipe) && (_i < 256) ) {											
			fread(&_buf[_i++], 1, 1, _min_version_pipe);											
		}																							
		_buf[_i] = 0;																				
		_buf[_i - 1] = 0;																			
		_buf[_i - 2] = 0;																			
		pclose(_min_version_pipe);																	
	}																								
	else {																							
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Error getting the min macOS version");					
		exit(-1);																					
	}																								
}
#endif

void include_imgui_in_compilation(rde_command* _build_command) {
	rde_command _inner_command = NULL;
	arrput(_inner_command, "clang++");
	
	#if !_WIN32
	arrput(_inner_command, "-c");
	#endif

	char _rde_imgui_cpp[MAX_PATH];
	memset(_rde_imgui_cpp, 0, MAX_PATH);
	snprintf(_rde_imgui_cpp, MAX_PATH, "%s%s", builder_exe_full_path_dir, "external/include/imgui/rde_imgui.cpp");
	arrput(_inner_command, _rde_imgui_cpp);

	char _lib_output_path[MAX_PATH];
	memset(_lib_output_path, 0, MAX_PATH);
	strcat(_lib_output_path, builder_exe_full_path_dir);

	arrput(_inner_command, "-I");
	char _imgui_path[MAX_PATH];
	memset(_imgui_path, 0, MAX_PATH);
	snprintf(_imgui_path, MAX_PATH, "%s%s", builder_exe_full_path_dir, "external/include/imgui");
	arrput(_inner_command, _imgui_path);

	arrput(_inner_command, "-I");
	char _sdl_path[MAX_PATH];
	memset(_sdl_path, 0, MAX_PATH);
	snprintf(_sdl_path, MAX_PATH, "%s%s", builder_exe_full_path_dir, "external/include/SDL2");
	arrput(_inner_command, _sdl_path);

	#if _WIN32
	arrput(_inner_command, "-o");

	strcat(_lib_output_path, "external/libs/windows/imgui.lib");
	arrput(_inner_command, _lib_output_path);
	#endif

	if(!run_command(_inner_command)) {
		printf("Error compiling imgui\n");
		exit(-1);
	}	

	printf("Good to go!\n");
	(void)_build_command;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
dyn_str* dyn_str_new(char* _init) {
    dyn_str* _s = (dyn_str*)malloc(sizeof(dyn_str));

    if(_s == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not allocate memory for dyn_str");
		return NULL;
	}

    _s->size = MAX(strlen(_init) + 1, STRSIZE);
    _s->str = (char *)malloc(_s->size);
	memset(_s->str, 0, _s->size);

    strcpy(_s->str, _init);

    return _s;
}

void dyn_str_free(dyn_str* _s) {
    free(_s->str);
    free(_s);
}

void dyn_str_growto(dyn_str* _s, unsigned int _new_size) {
    if(_new_size <= _s->size) {
		return;
	}

    unsigned int _old_size = _s->size;

    while(_s->size < _new_size) {
        _s->size *= 2;
	}
    
    _s->str = (char*)realloc((void*)_s->str, _s->size);

    memset((void*)(_s->str + _old_size), '\0', _s->size - _old_size);
}

void dyn_str_grow(dyn_str* _s) {
    dyn_str_growto(_s, _s->size * 2);
}

void dyn_str_shrink(dyn_str* _s) {
    _s->size /= 2;
    _s->size = MAX(_s->size, STRSIZE);
    _s->str = (char*)realloc((void*)_s->str, _s->size);
    _s->str[_s->size-1] = '\0';
}

unsigned int dyn_str_sizeof(dyn_str* _s) {
    return _s->size;
}

unsigned int dyn_str_length(dyn_str* _s) {
    return strlen(_s->str);
}

char dyn_str_getc(dyn_str* _s, unsigned int _index) {
    if(_index >= _s->size) return '\0';

    return _s->str[_index];
}

char* dyn_str_get(dyn_str* _s) {
    return _s->str;
}

void dyn_str_append(dyn_str* _s, char* _string) {
    int _len = strlen(_s->str);

    dyn_str_growto(_s, _s->size + _len);

    for(int _i = 0; _i < strlen(_string); _i++) {
        _s->str[_len + _i] = _string[_i];
    }
}

void dyn_str_cappend(dyn_str* _s, char _c) {
    unsigned int _len = strlen(_s->str);

    if(_len >= _s->size - 1)
        dyn_str_grow(_s);

    _s->str[_len] = _c;
}

void dyn_str_set(dyn_str* _s, char* _new_string) {
    dyn_str_growto(_s, _s->size + strlen(_new_string));
    strcpy(_s->str, _new_string);
}










#define ADD_PATH(_path_var, _path) 							\
	dyn_str* _path_var = dyn_str_new(builder_exe_full_path_dir);	\
	dyn_str_append(_path_var, _path);						\
	arrput(_build_command, dyn_str_get(_path_var));

#define ADD_PATH_EX(_path_var, _path) 				\
	_path_var = dyn_str_new(builder_exe_full_path_dir);	\
	dyn_str_append(_path_var, _path);				\
	arrput(_build_command, dyn_str_get(_path_var));

#define INCLUDE_PATH(_include_var_name, _path) 	\
	arrput(_build_command, "-I");				\
	ADD_PATH(_include_var_name, _path)

#define INCLUDE_PATH_EX(_include_var_name, _path) 	\
	arrput(_build_command, "-I");					\
	ADD_PATH_EX(_include_var_name, _path)

#define LINK_PATH(_include_var_name, _path) 	\
	arrput(_build_command, "-L");				\
	ADD_PATH(_include_var_name, _path)

#define LINK_PATH_EX(_include_var_name, _path) 	\
	arrput(_build_command, "-L");				\
	ADD_PATH_EX(_include_var_name, _path)

#define ADD_FLAG(_flag) arrput(_build_command, _flag);	

#define COPY_FILE(_origin, _dest) 								\
	if(_in != NULL) {											\
		dyn_str_free(_in);										\
		dyn_str_free(_out);										\
		_in = NULL;												\
		_out = NULL;											\
	}															\
																\
	_in = dyn_str_new(builder_exe_full_path_dir);				\
	_out = dyn_str_new(builder_exe_full_path_dir);				\
	dyn_str_append(_in, _origin);								\
	dyn_str_append(_out, _dest);								\
	copy_file_if_exists(dyn_str_get(_in), dyn_str_get(_out));
		
#define COPY_FOLDER(_origin, _dest) 							\
	if(_in != NULL) {											\
		dyn_str_free(_in);										\
		dyn_str_free(_out);										\
		_in = NULL;												\
		_out = NULL;											\
	}															\
																\
	_in = dyn_str_new(builder_exe_full_path_dir);				\
	_out = dyn_str_new(builder_exe_full_path_dir);				\
	dyn_str_append(_in, _origin);								\
	dyn_str_append(_out, _dest);								\
	copy_folder_if_exists(dyn_str_get(_in), dyn_str_get(_out));

#define COPY_FILE_CWD(_origin, _dest) 							\
	if(_in != NULL) {											\
		dyn_str_free(_in);										\
		dyn_str_free(_out);										\
		_in = NULL;												\
		_out = NULL;											\
	}															\
																\
	_in = dyn_str_new(builder_exe_full_path_dir);				\
	_out = dyn_str_new(working_dir);							\
	dyn_str_append(_in, _origin);								\
	dyn_str_append(_out, _dest);								\
	copy_file_if_exists(dyn_str_get(_in), dyn_str_get(_out));

#define COPY_FOLDER_CWD(_origin, _dest) 						\
	if(_in != NULL) {											\
		dyn_str_free(_in);										\
		dyn_str_free(_out);										\
		_in = NULL;												\
		_out = NULL;											\
	}															\
																\
	_in = dyn_str_new(builder_exe_full_path_dir);				\
	_out = dyn_str_new(working_dir);							\
	dyn_str_append(_in, _origin);								\
	dyn_str_append(_out, _dest);								\
	copy_folder_if_exists(dyn_str_get(_in), dyn_str_get(_out));

#define FREE_ATLAS_ALLOCS()			\
	dyn_str_free(_tools_path);		\
	dyn_str_free(_atlas_gen_path);	\
	dyn_str_free(_output_atlas);
	
#define FREE_FONT_ALLOCS()			\
	dyn_str_free(_font_gen_path);	\
	dyn_str_free(_output_font);	
	
#define FREE_PROJECT_ALLOCS()		\
	dyn_str_free(_project_gen_path);\
	dyn_str_free(_output_project);	\
	dyn_str_free(_in);				\
	dyn_str_free(_out);

#define COMPILE_TOOLS(_platform, _exe_ext, _ag_link_flags, _fg_link_flags, _pg_link_flags, _copy_files)									\
	do {																																\
		_build_command = NULL;																											\
																																		\
		dyn_str* _tools_path = dyn_str_new(builder_exe_full_path_dir);																		\
		dyn_str_append(_tools_path, "build/"_platform"/tools/");																		\
																																		\
		if(!make_dir_if_not_exists(dyn_str_get(_tools_path))) {																			\
			exit(-1);																													\
		}																																\
																																		\
		dyn_str* _atlas_gen_path = dyn_str_new(dyn_str_get(_tools_path));																\
		dyn_str_append(_atlas_gen_path, "atlas_generator/");																			\
		if(!make_dir_if_not_exists(dyn_str_get(_atlas_gen_path))) {																		\
			exit(-1);																													\
		}																																\
																																		\
		dyn_str* _output_atlas = dyn_str_new(dyn_str_get(_atlas_gen_path));																\
		dyn_str_append(_output_atlas, "atlas_generator"_exe_ext);																		\
																																		\
		ADD_FLAG("clang");																												\
		ADD_FLAG("-O3");																												\
		ADD_FLAG("-std=c99");																											\
																																		\
		ADD_PATH(_ag_source_path, "tools/atlas_generator/atlas_generator.c");															\
																																		\
		INCLUDE_PATH(_ag_include_path, "tools/atlas_generator/external/include/");														\
																																		\
		_ag_link_flags																													\
		ADD_FLAG("-Werror");																											\
		ADD_FLAG("-Wall");																												\
		ADD_FLAG("-Wextra");																											\
																																		\
		ADD_FLAG("-o");																													\
		ADD_FLAG(dyn_str_get(_output_atlas));																							\
																																		\
		if(!run_command(_build_command)) {																								\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
			FREE_ATLAS_ALLOCS()																											\
			exit(-1);																													\
		}																																\
		rde_log_level(RDE_LOG_LEVEL_INFO, "Atlas generator built successfully");														\
																																		\
																																		\
																																		\
																																		\
		_build_command = NULL;																											\
																																		\
		dyn_str* _font_gen_path = dyn_str_new(dyn_str_get(_tools_path));																\
		dyn_str_append(_font_gen_path, "font_generator/");																				\
		if(!make_dir_if_not_exists(dyn_str_get(_font_gen_path))) {																		\
			exit(-1);																													\
		}																																\
																																		\
		dyn_str* _output_font = dyn_str_new(dyn_str_get(_font_gen_path));																\
		dyn_str_append(_output_font, "font_generator"_exe_ext);																			\
																																		\
		ADD_FLAG("clang");																												\
		ADD_FLAG("-O3");																												\
		ADD_FLAG("-std=c99");																											\
																																		\
		ADD_PATH(_fg_source_path, "tools/font_generator/font_generator.c");																\
																																		\
		INCLUDE_PATH(_fg_include_path, "tools/font_generator/external/include/");														\
		LINK_PATH(_fg_libs_path, "tools/font_generator/external/libs/windows/");														\
																																		\
		_fg_link_flags																													\
		ADD_FLAG("-Werror");																											\
		ADD_FLAG("-Wall");																												\
		ADD_FLAG("-Wextra");																											\
																																		\
		ADD_FLAG("-o");																													\
		ADD_FLAG(dyn_str_get(_output_font));																							\
																																		\
		if(!run_command(_build_command)) {																								\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
			FREE_ATLAS_ALLOCS()																											\
			FREE_FONT_ALLOCS()																											\
			exit(-1);																													\
		}																																\
		rde_log_level(RDE_LOG_LEVEL_INFO, "Font generator built successfully");															\
																																		\
																																		\
		dyn_str* _in = NULL;																											\
		dyn_str* _out = NULL;																											\
		_copy_files																														\
																																		\
		_build_command = NULL;																											\
																																		\
		dyn_str* _project_gen_path = dyn_str_new(dyn_str_get(_tools_path));																\
		dyn_str_append(_project_gen_path, "project_generator/");																		\
		if(!make_dir_if_not_exists(dyn_str_get(_project_gen_path))) {																	\
			exit(-1);																													\
		}																																\
																																		\
		dyn_str* _output_project = dyn_str_new(dyn_str_get(_project_gen_path));															\
		dyn_str_append(_output_project, "project_generator"_exe_ext);																	\
																																		\
		ADD_FLAG("clang");																												\
		ADD_FLAG("-O3");																												\
		ADD_FLAG("-std=c99");																											\
																																		\
		ADD_PATH(_pg_source_path, "tools/project_generator/project_generator.c");														\
																																		\
		INCLUDE_PATH(_pg_include_path, "tools/project_generator/external/include/");													\
																																		\
		_pg_link_flags																													\
		ADD_FLAG("-Werror");																											\
		ADD_FLAG("-Wall");																												\
		ADD_FLAG("-Wextra");																											\
																																		\
		ADD_FLAG("-o");																													\
		ADD_FLAG(dyn_str_get(_output_project));																							\
																																		\
		if(!run_command(_build_command)) {																								\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
			FREE_ATLAS_ALLOCS()																											\
			FREE_FONT_ALLOCS()																											\
			FREE_PROJECT_ALLOCS()																										\
			exit(-1);																													\
		}																																\
		rde_log_level(RDE_LOG_LEVEL_INFO, "Project generator built successfully");														\
																																		\
		COPY_FILE("tools/project_generator/duck_logo.png", "build/"_platform"/tools/project_generator/duck_logo.png")					\
																																		\
		FREE_ATLAS_ALLOCS()																												\
		FREE_FONT_ALLOCS()																												\
		FREE_PROJECT_ALLOCS()																											\
	} while(0);


#define FREE_EXAMPLES_ALLOCS()				\
	dyn_str_free(_examples_path);			\
	dyn_str_free(_output_examples);			\
	dyn_str_free(_hub_source_path);			\
	dyn_str_free(_ext_include_path);		\
	dyn_str_free(_ext_imgui_include_path);	\
	dyn_str_free(_engine_include_path);		\
	dyn_str_free(_engine_link_path);		\
	dyn_str_free(_in);						\
	dyn_str_free(_out); \

	// On some Windows OS the entry point not define error is solved by adding '-Xlinker /subsystem:console -lShell32' to the flags
#define COMPILE_EXAMPLE(_platform, _exe_ext, _link_flags, _copy_files)									\
	do {																								\
		_build_command = NULL;																			\
																										\
		dyn_str* _examples_path = dyn_str_new(dyn_str_get(_path));										\
		dyn_str_append(_examples_path, "examples/");													\
																										\
		if (!make_dir_if_not_exists(dyn_str_get(_examples_path))) {										\
			exit(-1);																					\
		}																								\
																										\
		dyn_str* _output_examples = dyn_str_new(dyn_str_get(_examples_path));							\
		dyn_str_append(_output_examples, "hub"_exe_ext);												\
																										\
		arrput(_build_command, "clang");																\
																										\
		if (strcmp(build_type, DEBUG_STR) == 0) {														\
			ADD_FLAG("-g");																				\
			ADD_FLAG("-O0");																			\
			ADD_FLAG("-DRDE_DEBUG");																	\
		} else {																						\
			ADD_FLAG("-O3");																			\
		}																								\
		ADD_FLAG("-std=c99");																			\
																										\
		ADD_PATH(_hub_source_path, "examples/hub.c");													\
																										\
		INCLUDE_PATH(_ext_include_path, "external/include/");											\
		INCLUDE_PATH(_ext_imgui_include_path, "external/include/imgui");								\
		INCLUDE_PATH(_engine_include_path, "engine/include/");											\
		LINK_PATH(_external_link_path, "external/libs/"_platform"/");									\
																										\
		dyn_str* _engine_link_path = NULL;																\
		if (strcmp(build_type, DEBUG_STR) == 0) {														\
			LINK_PATH_EX(_engine_link_path, "build/"_platform"/debug/engine/");							\
		} else {																						\
			LINK_PATH_EX(_engine_link_path, "build/"_platform"/release/engine/");						\
		}																								\
																										\
		ADD_FLAG("-Werror");																			\
		ADD_FLAG("-Wall");																				\
		ADD_FLAG("-Wextra");																			\
		ADD_FLAG("-lRDE");																				\
		_link_flags																						\
																										\
		ADD_FLAG("-o");																					\
		ADD_FLAG(dyn_str_get(_output_examples));														\
																										\
		if (!run_command(_build_command)) {																\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");							\
			exit(-1);																					\
		}																								\
																										\
		dyn_str* _in = NULL;																			\
		dyn_str* _out = NULL;																			\
		_copy_files																						\
		FREE_EXAMPLES_ALLOCS()																			\
	} while(0);

#define COMPILE_TESTS(_platform, _exe_ext, _link_flags, _copy_files)				\
	do {																			\
		_build_command = NULL;														\
																					\
		dyn_str* _tests_path = dyn_str_new(dyn_str_get(_path));						\
		dyn_str_append(_tests_path, "unit_tests/");									\
																					\
		if(!make_dir_if_not_exists(dyn_str_get(_tests_path))) {						\
			exit(-1);																\
		}																			\
																					\
		dyn_str* _output_tests = dyn_str_new(dyn_str_get(_tests_path));				\
		dyn_str_append(_output_tests, "run_tests"_exe_ext);							\
																					\
		arrput(_build_command, "clang");											\
		if(strcmp(build_type, DEBUG_STR) == 0) {									\
			ADD_FLAG("-g");															\
			ADD_FLAG("-O0");														\
		} else {																	\
			ADD_FLAG("-O3");														\
		}																			\
		ADD_FLAG("-std=c99");														\
																					\
		ADD_PATH(_tests_source_path, "tests/test_run.c");							\
																					\
		INCLUDE_PATH(_ext_include_path, "external/include/");						\
		INCLUDE_PATH(_engine_include_path, "engine/include/");						\
		LINK_PATH(_external_link_path, "external/libs/"_platform"/");				\
																					\
		dyn_str* _engine_link_path = NULL;											\
		if(strcmp(build_type, DEBUG_STR) == 0) {									\
			LINK_PATH_EX(_engine_link_path, "build/"_platform"/debug/engine/");		\
		} else {																	\
			LINK_PATH_EX(_engine_link_path, "build/"_platform"/release/engine/");	\
		}																			\
																					\
		ADD_FLAG("-Werror")															\
		ADD_FLAG("-Wall")															\
		ADD_FLAG("-Wextra")															\
		_link_flags																	\
		ADD_FLAG("-lRDE")															\
																					\
		ADD_FLAG("-o");																\
		ADD_FLAG(dyn_str_get(_output_tests));										\
																					\
		if(!run_command(_build_command)) {											\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");		\
			exit(-1);																\
		}																			\
																					\
		dyn_str* _in = NULL;														\
		dyn_str* _out = NULL;														\
		_copy_files																	\
	} while(0);


#define FREE_ENGINE_ALLOCS()						\
	dyn_str_free(_output_engine);					\
	dyn_str_free(_rde_src_path);					\
	dyn_str_free(_glad_src_path);					\
	dyn_str_free(_rde_include_path);				\
	dyn_str_free(_rde_src_include_path);			\
	dyn_str_free(_external_include_path);			\
	if(_external_imgui_include_path != NULL) {		\
		dyn_str_free(_external_imgui_include_path);	\
	}												\
	dyn_str_free(_external_lib_path);				\
	dyn_str_free(_external_manual_lib_path);		\

#if _WIN32
void compile_windows_engine(dyn_str* _path, rde_command _build_command) {
	dyn_str* _output_engine = dyn_str_new(dyn_str_get(_path));
	dyn_str_append(_output_engine, "engine/RDE.dll");

	arrput(_build_command, "clang");

	unsigned int _module = 1;
	for(int _i = 0; _i < MAX_MODULES; _i++) {
		if((modules & _module) == _module) {
			ADD_FLAG(MODULES_DEFINES[_i]);
		}
		_module = _module << 1;
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		ADD_FLAG("-g");
		ADD_FLAG("-O0");
		ADD_FLAG("-DRDE_DEBUG");
	} else {
		ADD_FLAG("-O3");
	}

	ADD_FLAG("-std=c99");

	if(is_god) {
		ADD_FLAG(GOD_MODE);
	}

	ADD_FLAG("-shared");

	ADD_PATH(_rde_src_path, "engine/src/rde.c");
	ADD_PATH(_glad_src_path, "external/include/glad/glad.c");

	INCLUDE_PATH(_rde_include_path, "engine/include/");
	INCLUDE_PATH(_rde_src_include_path, "engine/src/");
	INCLUDE_PATH(_external_include_path, "external/include/");

	dyn_str* _external_imgui_include_path = NULL;
	if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
		INCLUDE_PATH_EX(_external_imgui_include_path, "external/include/imgui/");
	}

	LINK_PATH(_external_lib_path, "external/libs/windows/");
	LINK_PATH(_external_manual_lib_path, "external/libs/windows/manual-link/");

	ADD_FLAG("-lSDL2main");
	ADD_FLAG("-lSDL2");
	ADD_FLAG("-lcglm");

	if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
		ADD_FLAG("-Xlinker /WHOLEARCHIVE:rde_imgui.lib");
	}

	if((modules & RDE_MODULES_PHYSICS_3D) == RDE_MODULES_PHYSICS_3D) {
		ADD_FLAG("-ljolt");
	}

	ADD_FLAG("-ldbghelp")
	ADD_FLAG("-lshlwapi")
	ADD_FLAG("-lAdvapi32")
	ADD_FLAG("-Werror")
	ADD_FLAG("-Wall")
	ADD_FLAG("-Wextra")
	ADD_FLAG("-Wno-tautological-constant-out-of-range-compare")

	ADD_FLAG("-o");
	ADD_FLAG(dyn_str_get(_output_engine));

	if(!run_command(_build_command)) {
		FREE_ENGINE_ALLOCS()
		exit(-1);
	}
	FREE_ENGINE_ALLOCS()
}

bool compile_windows_rde() {
	errno = 0;

	dyn_str* _path = dyn_str_new("");
	dyn_str_append(_path, builder_exe_full_path_dir);
	dyn_str_append(_path, "build/");
	
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}
	
	dyn_str_append(_path, "windows/");
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		dyn_str_append(_path, "debug/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	} else {
		dyn_str_append(_path, "release/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	}

	dyn_str* _engine_folder = dyn_str_new(dyn_str_get(_path));
	dyn_str_append(_engine_folder, "engine/");
	if(!make_dir_if_not_exists(dyn_str_get(_engine_folder))) {
		exit(-1);
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0 || strcmp(build, "tests") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		compile_windows_engine(_path, _build_command);
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		COMPILE_TOOLS("windows", ".exe",
		{
		}, 
		{
			ADD_FLAG("-lfreetype");
		}, 
		{
		},
		{
			COPY_FILE("tools/font_generator/external/libs/windows/zlib1.dll", "build/windows/tools/font_generator/zlib1.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/brotlicommon.dll", "build/windows/tools/font_generator/brotlicommon.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/brotlidec.dll", "build/windows/tools/font_generator/brotlidec.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/brotlienc.dll", "build/windows/tools/font_generator/brotlienc.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/bz2.dll", "build/windows/tools/font_generator/bz2.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/freetype.dll", "build/windows/tools/font_generator/freetype.dll")
			COPY_FILE("tools/font_generator/external/libs/windows/libpng16.dll", "build/windows/tools/font_generator/libpng16.dll")
		})
	}

	if(strcmp(build, "examples") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING EXAMPLES --- \n");
		COMPILE_EXAMPLE("windows", ".exe",
		{
			ADD_FLAG("-lwinmm");
			ADD_FLAG("-lgdi32");
			ADD_FLAG("-lmsvcrt");
			ADD_FLAG("-Xlinker");
			ADD_FLAG("/nodefaultlib:msvcrt");
			ADD_FLAG("-Xlinker");
			ADD_FLAG("/nodefaultlib:libcmt");
		},
		{
			if (strcmp(build_type, DEBUG_STR) == 0) {
				COPY_FILE("build/windows/debug/engine/RDE.dll", "build/windows/debug/examples/RDE.dll")
				COPY_FILE("external/libs/windows/SDL2.dll", "build/windows/debug/examples/SDL2.dll")
				COPY_FOLDER("examples/hub_assets", "build/windows/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/windows/debug/examples/shaders/")
			} else {
				COPY_FILE("build/windows/release/engine/RDE.dll", "build/windows/release/examples/RDE.dll")
				COPY_FILE("external/libs/windows/SDL2.dll", "build/windows/release/examples/SDL2.dll")
				COPY_FOLDER("examples/hub_assets", "build/windows/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/windows/release/examples/shaders/")
			}
		})
	}

	if(strcmp(build, "tests") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TESTS --- \n");
		COMPILE_TESTS("windows", ".exe",
		{
			ADD_FLAG("-lwinmm")
			ADD_FLAG("-lgdi32")
		},
		{
			if(strcmp(build_type, DEBUG_STR) == 0) {
				COPY_FILE("build/windows/debug/engine/RDE.dll", "build/windows/debug/examples/RDE.dll")
				COPY_FILE("external/libs/windows/SDL2.dll", "build/windows/debug/examples/SDL2.dll")
				COPY_FOLDER("examples/hub_assets", "build/windows/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/windows/debug/examples/shaders/")	
			} else {
				COPY_FILE("build/windows/release/engine/RDE.dll", "build/windows/release/examples/RDE.dll")
				COPY_FILE("external/libs/windows/SDL2.dll", "build/windows/release/examples/SDL2.dll")
				COPY_FOLDER("examples/hub_assets", "build/windows/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/windows/release/examples/shaders/")
			}
		})
	}

	return true;
}
#endif


#if __APPLE__
void compile_osx_engine(dyn_str* _path, rde_command _build_command) {
	dyn_str* _output_engine = dyn_str_new(dyn_str_get(_path));
	
	if(strcmp(lib_type, SHARED_STR) == 0) {
		dyn_str_append(_output_engine, "engine/libRDE.dylib");
	} else {
		dyn_str_append(_output_engine, "engine/libRDE.o");
	}

	arrput(_build_command, "clang");

	unsigned int _module = 1;
	for(int _i = 0; _i < MAX_MODULES; _i++) {
		if((modules & _module) == _module) {
			ADD_FLAG(MODULES_DEFINES[_i]);
		}
		_module = _module << 1;
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		ADD_FLAG("-g");
		ADD_FLAG("-O0");
		ADD_FLAG("-DRDE_DEBUG");
	} else {
		ADD_FLAG("-O3");
	}

	ADD_FLAG("-std=c99");

	if(is_god) {
		ADD_FLAG(GOD_MODE);
	}

	char _mac_version_buf[256];
	memset(_mac_version_buf, 0, 256);
	get_mac_version(_mac_version_buf);
	char _min_version[MAX_PATH];
	memset(_min_version, 0, MAX_PATH);
	snprintf(_min_version, MAX_PATH, "-mmacosx-version-min=%s", _mac_version_buf);
	ADD_FLAG(_min_version)

	if(strcmp(lib_type, SHARED_STR) == 0) {
		ADD_FLAG("-shared");
		ADD_FLAG("-fPIC");
	} else {
		ADD_FLAG("-c");
	}

	ADD_PATH(_rde_src_path, "engine/src/rde.c");
	ADD_PATH(_glad_src_path, "external/include/glad/glad.c");

	INCLUDE_PATH(_rde_include_path, "engine/include/");
	INCLUDE_PATH(_rde_src_include_path, "engine/src/");
	INCLUDE_PATH(_external_include_path, "external/include/");

	dyn_str* _external_imgui_include_path = NULL;
	if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
		INCLUDE_PATH_EX(_external_imgui_include_path, "external/include/imgui/");
	}

	dyn_str* _imgui_whole_flag = NULL;
	if(strcmp(lib_type, SHARED_STR) == 0) {
		LINK_PATH(_external_lib_path, "external/libs/osx/");

		arrput(_build_command, "-ldl");
		arrput(_build_command, "-lm");
		arrput(_build_command, "-lpthread");
		arrput(_build_command, "-lSDL2main");
		arrput(_build_command, "-lSDL2");
		arrput(_build_command, "-framework");
		arrput(_build_command, "OpenGL");
		arrput(_build_command, "-lcglm");
		arrput(_build_command, "-lc++");
		arrput(_build_command, "-ObjC");

		if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
			ADD_FLAG("-Wl,-all_load");
			_imgui_whole_flag = dyn_str_new(builder_exe_full_path_dir);
			dyn_str_append(_imgui_whole_flag, "external/libs/osx/librde_imgui.dylib");
			ADD_FLAG(dyn_str_get(_imgui_whole_flag));
		}
		if((modules & RDE_MODULES_PHYSICS_3D) == RDE_MODULES_PHYSICS_3D) {
			ADD_FLAG("-ljolt");
		}
	}

	ADD_FLAG("-Werror");
	ADD_FLAG("-Wall");
	ADD_FLAG("-Wextra");
	ADD_FLAG("-Wno-tautological-constant-out-of-range-compare");

	ADD_FLAG("-o");
	ADD_FLAG(dyn_str_get(_output_engine));

	if(!run_command(_build_command)) {
		exit(-1);
	}

	if(strcmp(lib_type, "static") == 0) {
		rde_command _build_static_lib_command = NULL;
		arrput(_build_static_lib_command, "libtool");
		arrput(_build_static_lib_command, "-static");
		arrput(_build_static_lib_command, "-o");

		dyn_str* _output_lib = dyn_str_new(builder_exe_full_path_dir);

		if(strcmp(build_type, DEBUG_STR) == 0) {
			dyn_str_append(_output_lib, "build/linux/debug/engine/libRDE.a");
		} else {
			dyn_str_append(_output_lib, "build/linux/release/engine/libRDE.a");
		}

		arrput(_build_static_lib_command, dyn_str_get(_output_lib));
		arrput(_build_static_lib_command, dyn_str_get(_output_engine));
		rde_log_level(RDE_LOG_LEVEL_INFO, "Building static lib with ar...");
		if(!run_command(_build_static_lib_command)) {
			exit(-1);
		}
		rde_log_level(RDE_LOG_LEVEL_INFO, "Static library built!");

		rde_command _rm_dot_o_command = NULL;
		arrput(_rm_dot_o_command, "rm");
		arrput(_rm_dot_o_command, dyn_str_get(_output_engine));
		if(!run_command(_rm_dot_o_command)) {
			exit(-1);
		}

		dyn_str_free(_output_lib);
	}
}

bool compile_osx_rde() {
		dyn_str* _path = dyn_str_new("");
	dyn_str_append(_path, builder_exe_full_path_dir);
	dyn_str_append(_path, "build/");
	
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}
	
	dyn_str_append(_path, "osx/");
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		dyn_str_append(_path, "debug/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	} else {
		dyn_str_append(_path, "release/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	}

	dyn_str* _engine_folder = dyn_str_new(dyn_str_get(_path));
	dyn_str_append(_engine_folder, "engine/");
	if(!make_dir_if_not_exists(dyn_str_get(_engine_folder))) {
		exit(-1);
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		compile_osx_engine(_path, _build_command);
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		COMPILE_TOOLS("osx", "",
		{
			ADD_FLAG("-D_DEFAULT_SOURCE")
			ADD_FLAG("-lm")
		},
		{
			ADD_FLAG("-lfreetype")
			ADD_FLAG("-lz")
		},
		{
			ADD_FLAG("-D_DEFAULT_SOURCE")
		},
		{
		})
	}

	if(strcmp(build, "examples") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING EXAMPLES --- \n");
		COMPILE_EXAMPLE("osx", "",
		{
		    ADD_FLAG("-lrde_imgui");
			ADD_FLAG("-ljolt");
			ADD_FLAG("-lm");
			char _mac_version_buf[256];
			memset(_mac_version_buf, 0, 256);
			get_mac_version(_mac_version_buf);
			char _min_version[MAX_PATH];
			memset(_min_version, 0, MAX_PATH);
			snprintf(_min_version, MAX_PATH, "-mmacosx-version-min=%s", _mac_version_buf);
			ADD_FLAG(_min_version)
		},
		{
			if (strcmp(build_type, DEBUG_STR) == 0) {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/osx/debug/engine/libRDE.dylib", "build/osx/debug/examples/libRDE.dylib")
					COPY_FILE("external/libs/osx/libSDL2.dylib", "build/osx/debug/examples/libSDL2.dylib")
				}

				COPY_FOLDER("examples/hub_assets", "build/osx/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/osx/debug/examples/shaders/")
			} else {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/osx/release/engine/libRDE.dylib", "build/osx/release/examples/libRDE.dylib")
					COPY_FILE("external/libs/osx/libSDL2.dylib", "build/osx/release/examples/libSDL2.dylib")
				}

				COPY_FOLDER("examples/hub_assets", "build/osx/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/osx/release/examples/shaders/")
			}
		})
	}

	if(strcmp(build, "tests") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TESTS --- \n");
		COMPILE_EXAMPLE("osx", "",
		{
		    ADD_FLAG("-lrde_imgui");
			ADD_FLAG("-ljolt");
			ADD_FLAG("-lm");
			char _mac_version_buf[256];
			memset(_mac_version_buf, 0, 256);
			get_mac_version(_mac_version_buf);
			char _min_version[MAX_PATH];
			memset(_min_version, 0, MAX_PATH);
			snprintf(_min_version, MAX_PATH, "-mmacosx-version-min=%s", _mac_version_buf);
			ADD_FLAG(_min_version)
		},
		{
			if (strcmp(build_type, DEBUG_STR) == 0) {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/osx/debug/engine/libRDE.dylib", "build/osx/debug/examples/libRDE.dylib")
					COPY_FILE("external/libs/osx/libSDL2.dylib", "build/osx/debug/examples/libSDL2.dylib")
				}

				COPY_FOLDER("examples/hub_assets", "build/osx/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/osx/debug/examples/shaders/")
			} else {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/osx/release/engine/libRDE.dylib", "build/osx/release/examples/libRDE.dylib")
					COPY_FILE("external/libs/osx/libSDL2.dylib", "build/osx/release/examples/libSDL2.dylib")
				}

				COPY_FOLDER("examples/hub_assets", "build/osx/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/osx/release/examples/shaders/")
			}
		})
	}

	return true;
}
#endif

#if defined(__linux__)

void compile_linux_engine(dyn_str* _path, rde_command _build_command) {
	dyn_str* _output_engine = dyn_str_new(dyn_str_get(_path));
	
	if(strcmp(lib_type, SHARED_STR) == 0) {
		dyn_str_append(_output_engine, "engine/libRDE.so");
	} else {
		dyn_str_append(_output_engine, "engine/libRDE.o");
	}

	arrput(_build_command, "clang");

	unsigned int _module = 1;
	for(int _i = 0; _i < MAX_MODULES; _i++) {
		if((modules & _module) == _module) {
			ADD_FLAG(MODULES_DEFINES[_i]);
		}
		_module = _module << 1;
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		ADD_FLAG("-g");
		ADD_FLAG("-O0");
		ADD_FLAG("-DRDE_DEBUG");
	} else {
		ADD_FLAG("-O3");
	}

	ADD_FLAG("-std=c99");

	if(is_god) {
		ADD_FLAG(GOD_MODE);
	}

	if((modules & RDE_MODULES_ERROR) == RDE_MODULES_ERROR) {
		ADD_FLAG("-D_POSIX_C_SOURCE=200809L");
		ADD_FLAG("-D_XOPEN_SOURCE=500");
		ADD_FLAG("-D_GNU_SOURCE=1");
	}

	if(strcmp(lib_type, SHARED_STR) == 0) {
		ADD_FLAG("-shared");
		ADD_FLAG("-fPIC");
	} else {
		ADD_FLAG("-c");
	}

	ADD_PATH(_rde_src_path, "engine/src/rde.c");
	ADD_PATH(_glad_src_path, "external/include/glad/glad.c");

	INCLUDE_PATH(_rde_include_path, "engine/include/");
	INCLUDE_PATH(_rde_src_include_path, "engine/src/");
	INCLUDE_PATH(_external_include_path, "external/include/");

	dyn_str* _external_imgui_include_path = NULL;
	if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
		INCLUDE_PATH_EX(_external_imgui_include_path, "external/include/imgui/");
	}

	dyn_str* _imgui_whole_flag = NULL;
	if(strcmp(lib_type, SHARED_STR) == 0) {
		LINK_PATH(_external_lib_path, "external/libs/linux/");

		ADD_FLAG("-ldl");
		ADD_FLAG("-lm");
		ADD_FLAG("-lpthread");
		ADD_FLAG("-lSDL2main");
		ADD_FLAG("-lSDL2");
		ADD_FLAG("-lcglm");

		if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
			ADD_FLAG("-Wl,--whole-archive");
			_imgui_whole_flag = dyn_str_new(builder_exe_full_path_dir);
			dyn_str_append(_imgui_whole_flag, "external/libs/linux/librde_imgui.so");
			ADD_FLAG(dyn_str_get(_imgui_whole_flag));
			ADD_FLAG("-Wl,--no-whole-archive");
		}
		if((modules & RDE_MODULES_PHYSICS_3D) == RDE_MODULES_PHYSICS_3D) {
			ADD_FLAG("-ljolt");
		}
	}

	ADD_FLAG("-Werror");
	ADD_FLAG("-Wall");
	ADD_FLAG("-Wextra");
	ADD_FLAG("-Wno-tautological-constant-out-of-range-compare");

	ADD_FLAG("-o");
	ADD_FLAG(dyn_str_get(_output_engine));

	if(!run_command(_build_command)) {
		exit(-1);
	}

	if(strcmp(lib_type, "static") == 0) {
		rde_command _build_static_lib_command = NULL;
		arrput(_build_static_lib_command, "ar");
		arrput(_build_static_lib_command, "-crs");

		dyn_str* _output_lib = dyn_str_new(builder_exe_full_path_dir);

		if(strcmp(build_type, DEBUG_STR) == 0) {
			dyn_str_append(_output_lib, "build/linux/debug/engine/libRDE.a");
		} else {
			dyn_str_append(_output_lib, "build/linux/release/engine/libRDE.a");
		}

		arrput(_build_static_lib_command, dyn_str_get(_output_lib));
		arrput(_build_static_lib_command, dyn_str_get(_output_engine));
		rde_log_level(RDE_LOG_LEVEL_INFO, "Building static lib with ar...");
		if(!run_command(_build_static_lib_command)) {
			exit(-1);
		}
		rde_log_level(RDE_LOG_LEVEL_INFO, "Static library built!");

		rde_command _rm_dot_o_command = NULL;
		arrput(_rm_dot_o_command, "rm");
		arrput(_rm_dot_o_command, dyn_str_get(_output_engine));
		if(!run_command(_rm_dot_o_command)) {
			exit(-1);
		}

		dyn_str_free(_output_lib);
	}
}

bool compile_linux_rde() {
	errno = 0;

	dyn_str* _path = dyn_str_new("");
	dyn_str_append(_path, builder_exe_full_path_dir);
	dyn_str_append(_path, "build/");
	
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}
	
	dyn_str_append(_path, "linux/");
	if(!make_dir_if_not_exists(dyn_str_get(_path))) {
		exit(-1);
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		dyn_str_append(_path, "debug/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	} else {
		dyn_str_append(_path, "release/");
		if(!make_dir_if_not_exists(dyn_str_get(_path))) {
			exit(-1);
		}
	}

	dyn_str* _engine_folder = dyn_str_new(dyn_str_get(_path));
	dyn_str_append(_engine_folder, "engine/");
	if(!make_dir_if_not_exists(dyn_str_get(_engine_folder))) {
		exit(-1);
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0 || strcmp(build, "tests") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		compile_linux_engine(_path, _build_command);
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		COMPILE_TOOLS("linux", "",
		{
			ADD_FLAG("-D_DEFAULT_SOURCE")
			ADD_FLAG("-lm")
		},
		{
			ADD_FLAG("-lfreetype")
			ADD_FLAG("-lz")
		},
		{
			ADD_FLAG("-D_DEFAULT_SOURCE")
		},
		{
		})
	}

    if(strcmp(build, "examples") == 0 || strcmp(build, "all") == 0) {
        printf("\n");
        printf("--- BUILDING EXAMPLES --- \n");
		COMPILE_EXAMPLE("linux", "",
		{
		    ADD_FLAG("-lrde_imgui");
			ADD_FLAG("-ljolt");
			ADD_FLAG("-lm");
		},
		{
			if (strcmp(build_type, DEBUG_STR) == 0) {
				if(strcmp(build_type, SHARED_STR) == 0) {
					COPY_FILE("build/linux/debug/engine/libRDE.so", "build/linux/debug/examples/libRDE.so")
					COPY_FILE("external/libs/linux/libSDL2.so", "build/linux/debug/examples/libSDL2.so")
				}

				COPY_FOLDER("examples/hub_assets", "build/linux/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/linux/debug/examples/shaders/")
			} else {
				if(strcmp(build_type, SHARED_STR) == 0) {
					COPY_FILE("build/linux/release/engine/libRDE.so", "build/linux/release/examples/libRDE.so")
					COPY_FILE("external/libs/linux/libSDL2.so", "build/linux/release/examples/libSDL2.so")
				}

				COPY_FOLDER("examples/hub_assets", "build/linux/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/linux/release/examples/shaders/")
			}
		})
    }

	if(strcmp(build, "tests") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TESTS --- \n");
		COMPILE_TESTS("linux", "",
		{
			ADD_FLAG("-lrde_imgui");
			ADD_FLAG("-ljolt");
			ADD_FLAG("-lm");
		},
		{
			if (strcmp(build_type, DEBUG_STR) == 0) {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/linux/debug/engine/libRDE.so", "build/linux/debug/examples/libRDE.so")
					COPY_FILE("external/libs/linux/libSDL2.so", "build/linux/debug/examples/libSDL2.so")
				}

				COPY_FOLDER("examples/hub_assets", "build/linux/debug/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/linux/debug/examples/shaders/")
			} else {
				if(strcmp(lib_type, SHARED_STR) == 0) {
					COPY_FILE("build/linux/release/engine/libRDE.so", "build/linux/release/examples/libRDE.so")
					COPY_FILE("external/libs/linux/libSDL2.so", "build/linux/release/examples/libSDL2.so")
				}

				COPY_FOLDER("examples/hub_assets", "build/linux/release/examples/hub_assets/")
				COPY_FOLDER("engine/shaders", "build/linux/release/examples/shaders/")
			}
		})
	}

	return true;
}
#endif

bool build_desktop_project() {
	errno = 0;

	rde_command _build_command = NULL;

	if(android_clean) {
		rde_command _clean_command = NULL;
		arrput(_clean_command, "gradle");
		arrput(_clean_command, "clean");

		if(!run_command(_clean_command)) {
			exit(-1);
		}
	}

	dyn_str* _dekstop_path = dyn_str_new(working_dir);
	dyn_str_append(_dekstop_path, "build/");
	
	if (!make_dir_if_not_exists(dyn_str_get(_dekstop_path))) {
		exit(-1);
	}

	if(strcmp(build_type, DEBUG_STR) == 0) {
		dyn_str_append(_dekstop_path, "debug/");
	} else {
		dyn_str_append(_dekstop_path, "release/");
	}

	if (!make_dir_if_not_exists(dyn_str_get(_dekstop_path))) {
		exit(-1);
	}

	dyn_str* _output_desktop = dyn_str_new(dyn_str_get(_dekstop_path));
	if(strlen(project_name) > 0) {
		dyn_str_append(_output_desktop, project_name);
	} else {
		dyn_str_append(_output_desktop, "rde_");
		if(strcmp(build_type, DEBUG_STR) == 0) {
			dyn_str_append(_output_desktop, "debug");
		} else {
			dyn_str_append(_output_desktop, "release");
		}
	}

#if _WIN32
	dyn_str_append(_output_desktop, ".exe");
#endif

	bool _is_cpp = false;

	for(size_t _i = 0; _i < arrlenu(project_compile_files); _i++) {
		if(string_ends_with(dyn_str_get(project_compile_files[_i]), ".cpp")) {
			_is_cpp = true;
			break;
		}
	}

	if(_is_cpp) {
		arrput(_build_command, "clang++");
	} else {
		arrput(_build_command, "clang");
	}

	if (strcmp(build_type, DEBUG_STR) == 0) {
		ADD_FLAG("-g")
			ADD_FLAG("-O0")
			ADD_FLAG("-DRDE_DEBUG")
	} else {
		ADD_FLAG("-O3")
	}

	for(size_t _i = 0; _i < arrlenu(project_compile_files); _i++) {
		arrput(_build_command, dyn_str_get(project_compile_files[_i]));
	}

	INCLUDE_PATH(_ext_include_path, "external/include/");
	INCLUDE_PATH(_ext_imgui_include_path, "external/include/imgui");
	INCLUDE_PATH(_engine_include_path, "engine/include/");

	if(strcmp(build_type, DEBUG_STR) == 0) {
#if _WIN32
		LINK_PATH(_engine_link_path, "build/windows/debug/engine/");
#elif __APPLE__
		LINK_PATH(_engine_link_path, "build/osx/debug/engine/");
#else
		LINK_PATH(_engine_link_path, "build/linux/debug/engine/");
#endif
	} else {
#if _WIN32
		LINK_PATH(_engine_link_path, "build/windows/release/engine/");
#elif __APPLE__
		LINK_PATH(_engine_link_path, "build/osx/release/engine/");
#else
		LINK_PATH(_engine_link_path, "build/linux/release/engine/");
#endif
	}

	ADD_FLAG("-lRDE")
	ADD_FLAG("-ferror-limit=200")
#if _WIN32
	ADD_FLAG("-lwinmm");
	ADD_FLAG("-lgdi32");
	ADD_FLAG("-lmsvcrt");
	ADD_FLAG("-Xlinker");
	ADD_FLAG("/nodefaultlib:msvcrt");
	ADD_FLAG("-Xlinker");
	ADD_FLAG("/nodefaultlib:libcmt");
#else
	ADD_FLAG("-lm");
	if((modules & RDE_MODULES_PHYSICS_3D) == RDE_MODULES_PHYSICS_3D) {
		ADD_FLAG("-ljolt");
	}
	if((modules & RDE_MODULES_IMGUI) == RDE_MODULES_IMGUI) {
		ADD_FLAG("-lrde_imgui");
	}
#endif

	for(size_t _i = 0; _i < arrlenu(project_include_paths); _i++) {
		ADD_FLAG("-I");
		ADD_FLAG(dyn_str_get(project_include_paths[_i]));
	}

	for(size_t _i = 0; _i < arrlenu(project_link_paths); _i++) {
		ADD_FLAG("-L");
		ADD_FLAG(dyn_str_get(project_link_paths[_i]));
	}

	for(size_t _i = 0; _i < arrlenu(project_flags); _i++) {
		ADD_FLAG(dyn_str_get(project_flags[_i]));
	}

	ADD_FLAG("-o");
	ADD_FLAG(dyn_str_get(_output_desktop));

	if(!run_command(_build_command)) {
		exit(-1);
	}


	dyn_str* _in = NULL;
	dyn_str* _out = NULL;

	dyn_str* _shaders_path = dyn_str_new(dyn_str_get(_dekstop_path));
	dyn_str_append(_shaders_path, "shaders/");
	
	if(strcmp(build_type, DEBUG_STR) == 0) {
		COPY_FOLDER_CWD("engine/shaders", "build/debug/");
	} else {
		COPY_FOLDER_CWD("engine/shaders", "build/release/");
	}

#if _WIN32
	if (strcmp(build_type, DEBUG_STR) == 0) {
		COPY_FILE_CWD("build/windows/debug/engine/RDE.dll", "build/debug/RDE.dll")
		COPY_FILE_CWD("external/libs/windows/SDL2.dll", "build/debug/SDL2.dll")
	} else {
		COPY_FILE_CWD("build/windows/release/engine/RDE.dll", "build/release/RDE.dll")
		COPY_FILE_CWD("external/libs/windows/SDL2.dll", "build/release/SDL2.dll")
	}
#elif __APPLE__
	if(strcmp(build_type, DEBUG_STR) == 0) {
		if(strcmp(build_type, SHARED_STR) == 0) {
			COPY_FILE_CWD("build/osx/debug/engine/libRDE.dylib", "build/debug/libRDE.dylib")
			COPY_FILE_CWD("external/libs/osx/libSDL2.dylib", "build/debug/libSDL2.dylib")
		}
	} else {
		if(strcmp(build_type, SHARED_STR) == 0) {
			COPY_FILE_CWD("build/osx/release/engine/libRDE.dylib", "build/release/libRDE.dylib")
			COPY_FILE_CWD("external/libs/osx/libSDL2.dylib", "build/release/libSDL2.dylib")
		}
	}
#else
	if(strcmp(build_type, DEBUG_STR) == 0) {
		if(strcmp(build_type, SHARED_STR) == 0) {
			COPY_FILE_CWD("build/linux/debug/engine/libRDE.so", "build/debug/libRDE.so")
			COPY_FILE_CWD("external/libs/linux/libSDL2.so", "build/debug/libSDL2.so")
		}
	} else {
		if(strcmp(build_type, SHARED_STR) == 0) {
			COPY_FILE_CWD("build/linux/release/engine/libRDE.so", "build/release/libRDE.so")
			COPY_FILE_CWD("external/libs/linux/libSDL2.so", "build/release/libSDL2.so")
		}
	}
#endif

	return true;
}

bool build_android_project() {
	errno = 0;
	return false;
}

bool build_ios_project() {
	errno = 0;
	assert(false && "build_ios_project not implemented");
	return false;
}

bool build_wasm_project() {
	errno = 0;
	assert(false && "build_wasm_project not implemented");
	return false;
}

void print_help() {
	const char* _help_message = 
	"\n******************************************\n"
	"* RDE's command line utility help manual *\n"
	 "******************************************\n\n"
	"This tool allows to build the main engine, tests and tool's suite. \n"
	"IMPORTANT: No upper-cases are used, the builder program is case sensitive, for options and program keywords use lower-case, for the values "
	"passed to the options use whatever is needed. \n"
	"\n"
	"--- BUILD TYPES --- \n"
	"Builds of the library can be both [debug, release] and [static, shared] (each group can be combined as needed, no combinations of the same group). \n"
	"To set the build type use --build_type= (or -bt=) and one option between [debug, release]. \n"
	"To set the library type use --lib_type= (or -lt=) and one option between [static, shared]. Note that on windows this option is omitted as a .lib and .dll will be created. \n"
	"If no options are passed, it will be assumed -bt=debug and -lt=shared. \n"
	"\n"
	"-- BUILDING ---"
	"To build anything, use the option --build= (or -b=) and an action between [engine, tools, examples, tests, all, project]. \n"
	"engine will compile the engine (only). tools will compile atlas_generator and font_generator (only). test will compile the examples and the engine (only). all will compile all previous ones. \n"
	"If no option is passed, it will be assumed -b=all. \n"
	"project option means we want to build our own project made with RDE. If project is selected, option --platform= must be used to specify which type of project \n"
	"we are building. Options are [desktop, android, ios, wasm]. Sub options for each one are:\n"
	"	- desktop: \n"
	"		-L adds a linking path. This option can be used as many times as needed. \n"
	"		-I adds a include path. This option can be used as many times as needed. \n"
	"		--name= Sets the name of the application. If not set, the result will be rde_debug or rde_release \n"
	"	- android: same as desktop and:\n"
	"		--clean force gradle clean before building \n"
	"		--sdk= sets the path to the sdk (required) \n"
	"		--ndk= sets the path to the ndk (required) \n"
	"		--sign_path= sets the path to a file which will contain the jks file path, the password and the alias (requiered if signing is needed). The format of this file is: \n"
	"				jks_path = PATH_TO_JKS_FILE \n"
	"				password_path = PATH_TO_FILE_WITH_PASSWORD \n"
	"				alias_path = PATH_TO_FILE_WITH_ALIAS \n"
	"		--assets_path= path to the folder with all of the assets. This will be included directly on the build \n"
	"\n"
	"-- MODULES ---\n"
	"RDE is composed of several modules, which are completely optional to use and include in your library. Those modules are: \n"
	"	- audio\n"
	"	- physics_2d\n"
	"	- physics_3d\n"
	"	- rendering\n"
	"	- ui\n"
	"	- error\n"
	"	- imgui\n"
	"Each of them can be added to your library. Have in mind that adding 'physics_3d' will add Jolt Physics Engine to your depenency list, in case \n"
	"you are building a static lib, you will probably have to add the Jolt lib to your build system.\n"
	"To add any of the modules use --modules= (or -m=) followed by an array of the included modules, for example:\n"
	"	--modules=\"[rendering,audio]\" \n\n"
	"Have in mind that the only module added by default to the library is 'rendering'. If you specify --module option and you still \n"
	"want 'rendering', you MUST include it in the modules list.\n"
	"Modules Description:"
	"	- audio: adds basic audio control, such as loading/unloading sounds, mixing them, play/pause/stop sounds, modify volume, pitches...\n"
	"	- physics_2d: basic 2D physics engine, with collisions, rigidbodies, handlers...\n"
	"	- physics_3d: 3d physics engine with jolt.\n"
	"	- rendering: adds 2D rendering for 2D textures in a batched mode and also 3D batched drawing.\n"
	"	- ui: adds custom inmediate mode UI.\n"
	"	- error: adds handling for the most common error crashes and prints the stacktrace of the crash to the logs of the console (in debug) and to a error log file (in release).\n"
	"	- imgui: adds imgui native support.\n"
	"\n"
	"--- GOD MODE --- \n"
	"As described in the documentation, many structs shown to the user are simply pointers where you cannot access the \n"
	"inner fields, due to some of them not being modificable at random points or because they are part of other calculations \n"
	"on the engine, but if for any reason you want to be able to access them, use --god option, that will give you the power you seek.\n"
	"\n\n";

	printf("%s", _help_message);
}

void parse_arguments(int _argc, char** _argv) {

	if(_argc < 2) {
		print_help();
		exit(0);
	}

	const char* _delimiter = "=";
	const char _delimiter_2 = '=';
	for(int _i = 1; _i < _argc; _i++) {
		char _command_copy[256];
		memset(_command_copy, 0, 256);
		strcpy(_command_copy, _argv[_i]);
		char* _command = strtok(_command_copy, _delimiter);
		
		if(strcmp(_command, "-b") == 0 || strcmp(_command, "--build") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(build, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -b or --build incorrect\n");
				exit(-1);
			}

			bool _valid_option = false;
			for(size_t _i = 0; _i < MAX_BUILD_OPTIONS; _i++) {
				if(strcmp(_value + 1, BUILD_OPTIONS_STR[_i]) == 0) {
					_valid_option = true;
					break;
				}
			}

			if(!_valid_option) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument value for -b or --build is not valid '%s'. Valid options are:", _value);
				printf("\t[");
				for(size_t _i = 0; _i < MAX_BUILD_OPTIONS; _i++) {
					printf("%s, ", BUILD_OPTIONS_STR[_i]);
				}
				printf("]\n");
				exit(-1);
			}

			_value++;
			strcat(build, _value);
		} 
		
		
		else if(strcmp(_command, "-bt") == 0 || strcmp(_command, "--build_type") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(build_type, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -bt or --build_type incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(build_type, _value);
		} 
		
		
		else if(strcmp(_command, "-lt") == 0 || strcmp(_command, "--lib_type") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(lib_type, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -bt or --lib_type incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(lib_type, _value);
		} 
		
		
		else if(strcmp(_command, "-m") == 0 || strcmp(_command, "--modules") == 0) {
			char* _value = strrchr(_argv[_i], _delimiter_2);
			modules = RDE_MODULES_NONE;

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -m or --module incorrect\n");
				exit(-1);
			}

			_value += 1; // remove = chars
			if(_value[0] != '[' || _value[strlen(_value) - 1] != ']' ) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -m or --module must be a list like this '[module_0,module_1,...] must start with '[' and end with ']''\n");
				exit(-1);
			}

			_value += 1; // remove [ char
			_value[strlen(_value) - 1] = 0; // remove ] last character 

			char _modules_str[256];
			memset(_modules_str, 0, 256);
			strcat(_modules_str, _value);

			char* _module = strtok(_modules_str, ",");
			while( _module != NULL ) {
				const char* _trimed_module = trim(_module);
				
				RDE_MODULES_ _current_modules = modules;
				unsigned _m = 1;
				for(int _i = 0; _i < MAX_MODULES; _i++) {
					if(strcmp(MODULES_STR[_i], _trimed_module) == 0) {
						modules |= (RDE_MODULES_)_m;
						rde_log_level(RDE_LOG_LEVEL_INFO, "Added module '%s'", _trimed_module);
						break;
					}
					_m = _m << 1;
				}

				if(_current_modules == modules) {
					rde_log_level(RDE_LOG_LEVEL_WARNING, "Module '%s' not recognized, so ignoring it", _trimed_module);
				}
				
				_module = strtok(NULL, ",");
			}
		} 
		
		
		else if(strcmp(_command, "--god") == 0) {
			is_god = true;
		}
		
		else if(strcmp(_command, "--platform") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(platform, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for --platform incorrect\n");
				exit(-1);
			}

			bool _valid_option = false;
			for(size_t _i = 0; _i < MAX_BUILD_PLATFORM_OPTIONS; _i++) {
				if(strcmp(_value + 1, BUILD_PLATFORM_OPTIONS_STR[_i]) == 0) {
					_valid_option = true;
					break;
				}
			}

			if(!_valid_option) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument value for --platform is not valid '%s'. Valid options are:", _value);
				printf("\t[");
				for(size_t _i = 0; _i < MAX_BUILD_PLATFORM_OPTIONS; _i++) {
					printf("%s, ", BUILD_PLATFORM_OPTIONS_STR[_i]);
				}
				printf("]\n");
				exit(-1);
			}

			_value++;
			strcat(platform, _value);
		}
		

		
		else if (strcmp(_command, "-t") == 0 || strcmp(_command, "--tool") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(tool, 0 , MAX_SIZE_FOR_OPTIONS);

			if (_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -t or --tool incorrect\n");
				exit(-1);
			}

			bool _valid_option = false;
			for (size_t _i = 0; _i < MAX_TOOL_OPTIONS; _i++) {
				if (strcmp(_value + 1, TOOL_OPTIONS_STR[_i]) == 0) {
					_valid_option = true;
					break;
				}
			}

			if (!_valid_option) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument value for -t or --tool is not valid '%s'. Valid options are:", _value);
				printf("\t[");
				for (size_t _i = 0; _i < MAX_TOOL_OPTIONS; _i++) {
					printf("%s, ", TOOL_OPTIONS_STR[_i]);
				}
				printf("]\n");
				exit(-1);
			}

			_value++;
			strcat(tool, _value);
		} 
		
		
		
		else if(strcmp(_command, "-h") == 0 || strcmp(_command, "--help") == 0) {
			print_help();
			exit(0);
		}



		else if(strcmp(_command, "--clean") == 0) {
			android_clean = true;
		}


		else if(string_starts_with(_command, "-L")) {
			_command += 2;
			arrput(project_link_paths, dyn_str_new(_command));
		}

		else if(string_starts_with(_command, "-I")) {
			_command += 2;
			arrput(project_include_paths, dyn_str_new(_command));
		}

		else if(string_starts_with(_command, "--name")) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(project_name, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for --name incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(project_name, _value);
		}

		else {
			arrput(project_compile_files, dyn_str_new(_command));
		}
	}

	if(strlen(lib_type) == 0) {
		strcat(lib_type, SHARED_STR);
	}

	if(strlen(build_type) == 0) {
		strcat(build_type, DEBUG_STR);
	}
}

void execute_build_option() {
	if(strcmp(build, BUILD_OPTIONS_STR[BUILD_OPTION_ENGINE]) == 0 || 
		strcmp(build, BUILD_OPTIONS_STR[BUILD_OPTION_EXAMPLES]) == 0 || 
		strcmp(build, BUILD_OPTIONS_STR[BUILD_OPTION_TESTS]) == 0 || 
		strcmp(build, BUILD_OPTIONS_STR[BUILD_OPTION_ALL]) == 0) {
#if _WIN32
		compile_windows_rde();
#elif __APPLE__
		compile_osx_rde();
#elif defined(__linux__)
		compile_linux_rde();
#else
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Compiling on an unsupported OS, exiting.");
		exit(-1);
#endif
	} else if(strcmp(build, "project") == 0) {
		if(strlen(platform) == 0) {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Building project, but platform was not specify. Please use --platform= one of these options [desktop, android, ios, wasm]");
			exit(-1);
		}

		if(strcmp(platform, BUILD_PLATFORM_OPTIONS_STR[BUILD_PLATFORM_OPTION_DESKTOP]) == 0) {
			build_desktop_project();
		} else if(strcmp(platform, BUILD_PLATFORM_OPTIONS_STR[BUILD_PLATFORM_OPTION_ANDROID]) == 0) {
			build_android_project();
		} else if(strcmp(platform, BUILD_PLATFORM_OPTIONS_STR[BUILD_PLATFORM_OPTION_IOS]) == 0) {
			build_ios_project();
		} else if(strcmp(platform, BUILD_PLATFORM_OPTIONS_STR[BUILD_PLATFORM_OPTION_WASM]) == 0) {
			build_wasm_project();
		}
	}

	printf("\n");
	rde_log_level(RDE_LOG_LEVEL_INFO, "Build finished successfully. \n");
}

void execute_tool() {
	assert(false && "execute_tool not implemented yet");
}

void load_paths(char** _argv) {
	memset(builder_exe_full_path_dir, 0, MAX_PATH);
	memset(working_dir, 0, MAX_PATH);

	char _path_save[PATH_MAX];
	char* _path_ptr;

	if(!(_path_ptr = strrchr(_argv[0], '/')))
		getcwd(working_dir, sizeof(working_dir));
	else {
		*_path_ptr = '\0';
		getcwd(_path_save, sizeof(_path_save));
		chdir(_argv[0]);
		getcwd(working_dir, sizeof(working_dir));
		chdir(_path_save);
	}

#if _WIN32
	GetModuleFileName(NULL, builder_exe_full_path_dir, MAX_PATH);
	GetModuleFileName(NULL, builder_exe_full_path, MAX_PATH);
#elif __APPLE__
	size_t _size = MAX_PATH;
	_NSGetExecutablePath(builder_exe_full_path_dir, &_size)
	_NSGetExecutablePath(builder_exe_full_path, &_size)
#else
	readlink("/proc/self/exe", builder_exe_full_path_dir, MAX_PATH);
	readlink("/proc/self/exe", builder_exe_full_path, MAX_PATH);
#endif
	
#if _WIN32
	char _char = '\\';
#else
	char _char = '/';
#endif

	char* _split_ptr = strrchr(builder_exe_full_path_dir, _char);
	int _index_to_start_ignoring = strlen(builder_exe_full_path_dir) - strlen(_split_ptr) + 1;
	builder_exe_full_path_dir[_index_to_start_ignoring] = '\0';

	for(size_t _i = 0; _i < strlen(builder_exe_full_path_dir); _i++) {
		if(builder_exe_full_path_dir[_i] == '\\') {
			builder_exe_full_path_dir[_i] = '/';
		}
	}

	for(size_t _i = 0; _i < strlen(builder_exe_full_path); _i++) {
		if(builder_exe_full_path[_i] == '\\') {
			builder_exe_full_path[_i] = '/';
		}
	}

	for(size_t _i = 0; _i < strlen(working_dir); _i++) {
		if(working_dir[_i] == '\\') {
			working_dir[_i] = '/';
		}
	}

	if(working_dir[strlen(working_dir)] != '/') {
		working_dir[strlen(working_dir)] = '/';
	}
}

int main(int _argc, char** _argv) {
	load_paths(_argv);

	try_recompile_and_redirect_execution(_argc, _argv);

	memset(platform, 0, MAX_SIZE_FOR_OPTIONS);
	memset(build_type, 0, MAX_SIZE_FOR_OPTIONS);
	memset(lib_type, 0, MAX_SIZE_FOR_OPTIONS);
	memset(build, 0, MAX_SIZE_FOR_OPTIONS);
	memset(tool, 0, MAX_SIZE_FOR_OPTIONS);
	modules = RDE_MODULES_RENDERING;
	project_compile_files = NULL;
	project_include_paths = NULL;
	project_link_paths = NULL;
	project_flags = NULL;
	memset(project_name, 0, MAX_PATH);
	memset(android_sdk, 0, MAX_PATH);
	memset(android_ndk, 0, MAX_PATH);
	memset(android_sign, 0, MAX_PATH);
	android_clean = false;

	parse_arguments(_argc, _argv);

	if(strlen(build) > 0) {
		execute_build_option();
	} else if(strlen(tool) > 0) {
		execute_tool();
	}

	return 0;
}

#pragma clang diagnostic pop
