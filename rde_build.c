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

#define MAX_SIZE_FOR_OPTIONS 64
#define MAX_SIZE_FOR_MODULES 256
#define MAX_MODULES 8

typedef enum {
	RDE_MODULES_NONE = 0,
	RDE_MODULES_AUDIO = 1,
	RDE_MODULES_PHYSICS_2D = 2,
	RDE_MODULES_PHYSICS_3D = 4,
	RDE_MODULES_RENDERING_2D = 8,
	RDE_MODULES_RENDERING_3D = 16,
	RDE_MODULES_FBX = 32,
	RDE_MODULES_UI = 64,
	RDE_MODULES_ERROR = 128
} RDE_MODULES_;

const char* MODULES_STR[MAX_MODULES] = {
	"audio",
	"physics_2d",
	"pyhisics_3d",
	"rendering_2d",
	"rendering_3d",
	"fbx",
	"ui",
	"error"
};
char* MODULES_DEFINES[MAX_MODULES] = {
	"-DRDE_AUDIO_MODULE",
	"-DRDE_PHYSICS_2D_MODULE",
	"-DRDE_PHYSICS_3D_MODULE",
	"-DRDE_RENDERING_2D_MODULE",
	"-DRDE_RENDERING_3D_MODULE",
	"-DRDE_FBX_MODULE",
	"-DRDE_UI_MODULE",
	"-DRDE_ERROR_MODULE"
};
RDE_MODULES_ modules;

char platform[MAX_SIZE_FOR_OPTIONS];
char build_type[MAX_SIZE_FOR_OPTIONS];
char lib_type[MAX_SIZE_FOR_OPTIONS];
char build[MAX_SIZE_FOR_OPTIONS];

char this_file_full_path[MAX_PATH];

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

bool copy_file_if_exists(const char* _file_path, const char* _new_path);

bool compile_windows();
bool compile_osx();
bool compile_linux();
bool compile_android();
bool compile_ios();
bool compile_wasm();

bool compile_test();
bool run_test();

void print_help();
void parse_arguments(int _argc, char** _argv);

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
	char* _argument_list[32];

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
	strcat(_source_path, this_file_full_path);
	strcat(_source_path, "rde_build.c");

	char _binary_path[MAX_PATH];
	memset(_binary_path, 0, MAX_PATH);

	#if _WIN32
	// In windows we can execute a exe without using .exe in the name. This program assumes that it will always contains .exe so if it does not have it, it is added.
	if(strlen(get_filename_extension(_argv[0])) == 0) {
		strcat(_binary_path, _argv[0]);
		strcat(_binary_path, ".exe");	
	} else {
		strcpy(_binary_path, _argv[0]);
	}
	
	#else
	strcat(_binary_path, _argv[0]);
	#endif
	int _needs_recompile = needs_recompile(_binary_path, (const char**)(&_source_path), 1);
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
		char* _just_binary_name = strrchr(_binary_path, _delimiter);

		char _to_old[MAX_PATH];
		memset(_to_old, 0, MAX_PATH);
		if(_just_binary_name != NULL) {
			_just_binary_name++;
			snprintf(_to_old, MAX_PATH, "%s%s", this_file_full_path, _just_binary_name);
		} else {
			snprintf(_to_old, MAX_PATH, "%s%s", this_file_full_path, _binary_path);
		}
		strcat(_to_old, ".old");

#if _WIN32
		char _to_trash[MAX_PATH];
		memset(_to_trash, 0, MAX_PATH);
		// This is the way of deleting the old exe in Windows
		strcat(_to_trash, "C:\\$Recycle.Bin\\");
		
		if(_just_binary_name != NULL) {
			strcat(_to_trash, _just_binary_name);
		} else {
			strcat(_to_trash, _binary_path);
		}
		strcat(_to_trash, ".old");


		if(!rename_file_if_exists(_binary_path, _to_old)) {
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
		rde_log_level(RDE_LOG_LEVEL_INFO, "Recompiling %s first", _just_binary_name != NULL ? _just_binary_name : _binary_path);
		rde_command _recompile_command = NULL;
		arrput(_recompile_command, "clang");
		arrput(_recompile_command, "-g");
		arrput(_recompile_command, "-O0");
		
		char _source_path[MAX_PATH];
		memset(_source_path, 0, MAX_PATH);
#if _WIN32
		snprintf(_source_path, MAX_PATH, "%s""rde_build.c", this_file_full_path);
		arrput(_recompile_command, _source_path);
#else
		snprintf(_source_path, MAX_PATH, "%s/rde_build.c", this_file_full_path);
		arrput(_recompile_command, _source_path);
#endif

		arrput(_recompile_command, "-o");

		char _out_path[MAX_PATH];
		memset(_out_path, 0, MAX_PATH);
		
#if _WIN32
		if(_just_binary_name != NULL) {
			snprintf(_out_path, MAX_PATH, "%s\%s", this_file_full_path, _just_binary_name);
		} else {
			snprintf(_out_path, MAX_PATH, "%s\%s", this_file_full_path, _binary_path);
		}
#else
		if(_just_binary_name != NULL) {
			snprintf(_out_path, MAX_PATH, "%s%s", this_file_full_path, _just_binary_name);
		} else {
			snprintf(_out_path, MAX_PATH, "%s%s", this_file_full_path, _binary_path);
		}
#endif

		arrput(_recompile_command, _out_path);

		char _old_binary_path[MAX_PATH];
		memset(_old_binary_path, 0, MAX_PATH);
		
#if _WIN32
//		strcat(_old_binary_path, "\\");
		//strcat(_old_binary_path, this_file_full_path);
#endif

		strcat(_old_binary_path, _binary_path);
		strcat(_old_binary_path, ".old");

		if(!run_command(_recompile_command)) {
			if(!rename_file_if_exists(_old_binary_path, _just_binary_name != NULL ? _just_binary_name : _binary_path)) {
				exit(-1);
			}
			exit(-1);
		} else {
			rde_log_level(RDE_LOG_LEVEL_INFO, "Compiled successfully.");
			if(!rename_file_if_exists(_old_binary_path, _to_trash)) {
				exit(-1);
			}
		}
		rde_log_level(RDE_LOG_LEVEL_INFO, "Recompiled successfully, redirecting original command...");

		rde_command _redirect_original_command = NULL;
		arrput(_redirect_original_command, _binary_path);
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

bool copy_file_if_exists(const char* _file_path, const char* _new_path) {
	#if _WIN32
	if(CopyFile(_file_path, _new_path, FALSE) == 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not copy %s into %s. (err_msg) -> %lu.", _file_path, _new_path, GetLastError());
		return false;
	}
	#else
	char _buf;
	FILE* _fd_0 = NULL;
	FILE* _fd_1 = NULL;

	_fd_0 = fopen(_file_path, "r");

	if (_fd_0 == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not open file %s. (err_msg) -> File could not be found.", _file_path);
		fclose(_fd_0);
		return false;
	}

	_fd_1 = fopen(_new_path, "w+");
	if (_fd_1 == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not create file %s.", _new_path);
		fclose(_fd_1);
		return false;
	}

	long _num_bytes = 0;
	fseek(_fd_0, 0L, SEEK_END);
	_num_bytes = ftell(_fd_0);
	fseek(_fd_0, 0L, SEEK_SET);

	while(fread(&_buf, _num_bytes, 1, _fd_0)) {
		fwrite(&_buf, _num_bytes, 1, _fd_1);
	}

	fclose(_fd_0);
	fclose(_fd_1);
	#endif

	rde_log_level(RDE_LOG_LEVEL_INFO, "Copied file %s to %s.", _file_path, _new_path);
	return true;
}





#if _WIN32
bool compile_windows() {
	errno = 0;

	char _path[MAX_PATH];
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}
	
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}

	memset(_path, 0, MAX_PATH);
	if(strcmp(build_type, "debug") == 0) {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\debug");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	} else {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\release");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;
	
	#define BUILD_ENGINE() 																					\
	do {																									\
		char _output[256];																					\
		memset(_output, 0, 256);																			\
		strcat(_output, this_file_full_path);																\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\debug\\engine");		\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build\\windows\\debug\\engine\\");												\
		} else {																							\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\release\\engine");		\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build\\windows\\release\\engine\\");											\
		}																									\
																											\
		char output_engine[MAX_PATH];																		\
		memset(output_engine, 0, MAX_PATH);																	\
		strcat(output_engine, _output);																		\
		strcat(output_engine, "RDE.dll");																	\
		arrput(_build_command, "clang");																	\
																											\
		unsigned int _module = 1;																			\
		for(int _i = 0; _i < MAX_MODULES; _i++) {															\
			if((modules & _module) == _module) {															\
				arrput(_build_command, MODULES_DEFINES[_i]);												\
			}																								\
			_module = _module << 1;																			\
		}																									\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			arrput(_build_command, "-g");																	\
			arrput(_build_command, "-O0");																	\
			arrput(_build_command, "-DRDE_DEBUG");															\
		} else {																							\
			arrput(_build_command, "-O3");																	\
		}																									\
																											\
		arrput(_build_command, "-std=c99");																	\
																											\
		char _temp_path_0[MAX_PATH];																		\
		memset(_temp_path_0, 0, MAX_PATH);																	\
		snprintf(_temp_path_0, MAX_PATH, "%s%s", this_file_full_path, "engine\\src\\rde.c");				\
		arrput(_build_command, _temp_path_0);																\
																											\
		arrput(_build_command, "-shared");																	\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_1[MAX_PATH];																		\
		memset(_temp_path_1, 0, MAX_PATH);																	\
		snprintf(_temp_path_1, MAX_PATH, "%s%s", this_file_full_path, "engine\\include");					\
		arrput(_build_command, _temp_path_1);																\
																											\
		char _temp_path_2[MAX_PATH];																		\
		arrput(_build_command, "-I");																		\
		memset(_temp_path_2, 0, MAX_PATH);																	\
		snprintf(_temp_path_2, MAX_PATH, "%s%s", this_file_full_path, "engine\\src");						\
		arrput(_build_command, _temp_path_2);																\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_3[MAX_PATH];																		\
		memset(_temp_path_3, 0, MAX_PATH);																	\
		snprintf(_temp_path_3, MAX_PATH, "%s%s", this_file_full_path, "external\\include");					\
		arrput(_build_command, _temp_path_3);																\
																											\
		arrput(_build_command, "-L");																		\
		char _temp_path_4[MAX_PATH];																		\
		memset(_temp_path_4, 0, MAX_PATH);																	\
		snprintf(_temp_path_4, MAX_PATH, "%s%s", this_file_full_path, "external\\libs\\windows");			\
		arrput(_build_command, _temp_path_4);																\
																											\
		arrput(_build_command, "-L");																		\
		char _temp_path_5[MAX_PATH];																		\
		memset(_temp_path_5, 0, MAX_PATH);																	\
		snprintf(_temp_path_5, MAX_PATH, "%s%s", this_file_full_path, "external\\libs\\windows\\manual-link");\
		arrput(_build_command, _temp_path_5);																\
																											\
		arrput(_build_command, "-lSDL2main");																\
		arrput(_build_command, "-lSDL2");																	\
		arrput(_build_command, "-lglad");																	\
		arrput(_build_command, "-lcglm");																	\
		arrput(_build_command, "-ldbghelp");																\
		arrput(_build_command, "-Werror");																	\
		arrput(_build_command, "-Wall");																	\
		arrput(_build_command, "-Wextra");																	\
		arrput(_build_command, "-Wno-tautological-constant-out-of-range-compare");							\
																											\
		arrput(_build_command, "-o");																		\
		arrput(_build_command, output_engine);																\
																											\
		if(!run_command(_build_command)) {																	\
			exit(-1);																						\
		}																									\
	} while(0);

	#define BUILD_TOOLS()																											\
	do {																															\
		char _output_a[256];																										\
		memset(_output_a, 0, 256);																									\
		strcat(_output_a, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools");											\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\atlas_generator");							\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_a, "build\\windows\\tools\\atlas_generator\\");																\
																																	\
		_build_command = NULL;																										\
		char output_atlas[MAX_PATH];																								\
		memset(output_atlas, 0, MAX_PATH);																							\
		strcat(output_atlas, _output_a);																							\
		strcat(output_atlas, "atlas_generator.exe");																				\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _ag_source_path[MAX_PATH];																								\
		memset(_ag_source_path, 0, MAX_PATH);																						\
		snprintf(_ag_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\atlas_generator\\atlas_generator.c");				\
		arrput(_build_command, _ag_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _ag_include_path[MAX_PATH];																							\
		memset(_ag_include_path, 0, MAX_PATH);																						\
		snprintf(_ag_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\atlas_generator\\external\\include");				\
		arrput(_build_command, _ag_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_atlas);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
		_build_command = NULL;																										\
		char _output_f[256];																										\
		memset(_output_f, 0, 256);																									\
		strcat(_output_f, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator");							\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_f, "build\\windows\\tools\\font_generator\\");																\
																																	\
		char output_fonts[MAX_PATH];																								\
		memset(output_fonts, 0, MAX_PATH);																							\
		strcat(output_fonts, _output_f);																							\
		strcat(output_fonts, "font_generator.exe");																					\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _fg_source_path[MAX_PATH];																								\
		memset(_fg_source_path, 0, MAX_PATH);																						\
		snprintf(_fg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\font_generator.c");				\
		arrput(_build_command, _fg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _fg_include_path[MAX_PATH];																							\
		memset(_fg_include_path, 0, MAX_PATH);																						\
		snprintf(_fg_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\include");				\
		arrput(_build_command, _fg_include_path);																					\
																																	\
		arrput(_build_command, "-L");																								\
		char _fg_libs_path[MAX_PATH];																								\
		memset(_fg_libs_path, 0, MAX_PATH);																							\
		snprintf(_fg_libs_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows");			\
		arrput(_build_command, _fg_libs_path);																						\
																																	\
		arrput(_build_command, "-lfreetype");																						\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_fonts);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
																																	\
		char _p_0[MAX_PATH];																										\
		char _p_1[MAX_PATH];																										\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\zlib1.dll");			\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\zlib1.dll");					\
		copy_file_if_exists(_p_0, _p_1); 																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\brotlicommon.dll");	\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\brotlicommon.dll");			\
		copy_file_if_exists(_p_0, _p_1);																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\brotlidec.dll");		\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\brotlidec.dll");				\
		copy_file_if_exists(_p_0, _p_1);																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\brotlienc.dll");		\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\brotlienc.dll");				\
		copy_file_if_exists(_p_0, _p_1);																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\bz2.dll");			\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\bz2.dll");					\
		copy_file_if_exists(_p_0, _p_1);																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\freetype.dll");		\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\freetype.dll");				\
		copy_file_if_exists(_p_0, _p_1);																							\
		memset(_p_0, 0, MAX_PATH);																									\
		memset(_p_1, 0, MAX_PATH);																									\
		snprintf(_p_0, MAX_PATH, "%s%s", this_file_full_path, "tools\\font_generator\\external\\libs\\windows\\libpng16.dll");		\
		snprintf(_p_1, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\font_generator\\libpng16.dll");				\
		copy_file_if_exists(_p_0, _p_1);																							\
																																	\
																																	\
		_build_command = NULL;																										\
		char _output_p[256];																										\
		memset(_output_p, 0, 256);																									\
		strcat(_output_p, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\project_generator");							\
		if(!make_dir_if_not_exists(_path)) {																						\
					exit(-1);																										\
		}																															\
		strcat(_output_f, "build\\windows\\tools\\project_generator\\");															\
																																	\
		char output_project[MAX_PATH];																								\
		memset(output_project, 0, MAX_PATH);																						\
		strcat(output_project, _output_p);																							\
		strcat(output_project, "project_generator.exe");																			\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _pg_source_path[MAX_PATH];																								\
		memset(_pg_source_path, 0, MAX_PATH);																						\
		snprintf(_pg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\project_generator\\project_generator.c");			\
		arrput(_build_command, _pg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _pg_include_path[MAX_PATH];																							\
		memset(_pg_include_path, 0, MAX_PATH);																						\
		snprintf(_pg_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools\\project_generator\\external\\include");			\
		arrput(_build_command, _pg_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_project);																						\
																																	\
		if(!run_command(_build_command)) {																							\
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
		exit(-1);																													\
		}																															\
																																	\
		char _duck_img_src[MAX_PATH];																								\
		char _duck_img_dst[MAX_PATH];																								\
		memset(_duck_img_src, 0, MAX_PATH);																							\
		memset(_duck_img_dst, 0, MAX_PATH);																							\
		snprintf(_duck_img_src, MAX_PATH, "%s%s", this_file_full_path, "tools\\project_generator\\duck_logo.png");					\
		snprintf(_duck_img_dst, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\tools\\project_generator\\duck_logo.png");	\
		copy_file_if_exists(_duck_img_src, _duck_img_dst); 																			\
	} while(0);

	#define BUILD_EXAMPLES()																						\
	do {																											\
		char _output[256];																							\
		memset(_output, 0, 256);																					\
		strcat(_output, this_file_full_path);																		\
																													\
		memset(_path, 0, MAX_PATH);																					\
		if(strcmp(build_type, "debug") == 0) {																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\debug\\examples");				\
			if(!make_dir_if_not_exists(_path)) {																	\
				exit(-1);																							\
			}																										\
			strcat(_output, "build\\windows\\debug\\examples\\");													\
		} else {																									\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build\\windows\\release\\examples");			\
			if(!make_dir_if_not_exists(_path)) {																	\
				exit(-1);																							\
			}																										\
			strcat(_output, "build\\windows\\release\\examples\\");													\
		}																											\
																													\
		_build_command = NULL;																						\
		char output_atlas[MAX_PATH];																				\
		memset(output_atlas, 0, MAX_PATH);																			\
		strcat(output_atlas, _output);																				\
		strcat(output_atlas, "test.exe");																			\
		arrput(_build_command, "clang");																			\
		if(strcmp(build_type, "debug") == 0) {																		\
			arrput(_build_command, "-g");																			\
			arrput(_build_command, "-O0");																			\
		} else {																									\
			arrput(_build_command, "-O3");																			\
		}																											\
		arrput(_build_command, "-std=c99");																			\
																													\
		char _t_source_path[MAX_PATH];																				\
		memset(_t_source_path, 0, MAX_PATH);																		\
		snprintf(_t_source_path, MAX_PATH, "%s%s", this_file_full_path, "test\\test.c");							\
		arrput(_build_command, _t_source_path);																		\
																													\
		arrput(_build_command, "-I");																				\
		char _t_include_path_0[MAX_PATH];																			\
		memset(_t_include_path_0, 0, MAX_PATH);																		\
		snprintf(_t_include_path_0, MAX_PATH, "%s%s", this_file_full_path, "external\\include\\");					\
		arrput(_build_command, _t_include_path_0);																	\
																													\
		arrput(_build_command, "-I");																				\
		char _t_include_path_1[MAX_PATH];																			\
		memset(_t_include_path_1, 0, MAX_PATH);																		\
		snprintf(_t_include_path_1, MAX_PATH, "%s%s", this_file_full_path, "engine\\include\\");					\
		arrput(_build_command, _t_include_path_1);																	\
																													\
		arrput(_build_command, "-L");																				\
		char _t_libs_path[MAX_PATH];																				\
		memset(_t_libs_path, 0, MAX_PATH);																			\
		snprintf(_t_libs_path, MAX_PATH, "%s""build\\%s\\%s\\engine", this_file_full_path, platform, build_type);	\
		arrput(_build_command, _t_libs_path);																		\
																													\
		arrput(_build_command, "-Werror");																			\
		arrput(_build_command, "-Wall");																			\
		arrput(_build_command, "-Wextra");																			\
		arrput(_build_command, "-lRDE");																			\
		arrput(_build_command, "-lwinmm");																			\
		arrput(_build_command, "-lgdi32");																			\
																													\
		arrput(_build_command, "-o");																				\
		arrput(_build_command, output_atlas);																		\
																													\
		if(!run_command(_build_command)) {																			\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");										\
			exit(-1);																								\
		}																											\
																													\
		char _rde_lib_path[256];																					\
		memset(_rde_lib_path, 0, 256);																				\
		strcat(_rde_lib_path, this_file_full_path);																	\
		char _example_path_sdl[256];																				\
		char _example_path_rde[256];																				\
		char _example_path_glad[256];																				\
		memset(_example_path_sdl, 0, 256);																			\
		memset(_example_path_rde, 0, 256);																			\
		memset(_example_path_glad, 0, 256);																			\
		strcat(_example_path_sdl, this_file_full_path);																\
		strcat(_example_path_glad, this_file_full_path);															\
		strcat(_example_path_rde, this_file_full_path);																\
																													\
		if(strcmp(build_type, "debug") == 0) {																		\
			strcat(_rde_lib_path, "build\\windows\\debug\\engine\\RDE.dll");										\
			strcat(_example_path_sdl, "build\\windows\\debug\\examples\\SDL2.dll");									\
			strcat(_example_path_glad, "build\\windows\\debug\\examples\\glad.dll");								\
			strcat(_example_path_rde, "build\\windows\\debug\\examples\\RDE.dll");									\
		} else {																									\
			strcat(_rde_lib_path, "build\\windows\\release\\engine\\RDE.dll");										\
			strcat(_example_path_sdl, "build\\windows\\release\\examples\\SDL2.dll");								\
			strcat(_example_path_glad, "build\\windows\\release\\examples\\glad.dll");								\
			strcat(_example_path_rde, "build\\windows\\release\\examples\\RDE.dll");								\
		}																											\
																													\
		memset(_path, 0, MAX_PATH);																					\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "external\\libs\\windows\\SDL2.dll");				\
		copy_file_if_exists(_path, _example_path_sdl);																\
		memset(_path, 0, MAX_PATH);																					\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "external\\libs\\windows\\glad.dll");				\
		copy_file_if_exists(_path, _example_path_glad);																\
		copy_file_if_exists(_rde_lib_path, _example_path_rde); 														\
	} while(0);

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		BUILD_ENGINE()
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		BUILD_TOOLS()
	}

	if(strcmp(build, "examples") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING EXAMPLES --- \n");
		BUILD_EXAMPLES()
	}

	#undef BUILD_EXAMPLES
	#undef BUILD_ENGINE
	#undef BUILD_TOOLS

	return true;
}
#endif

#if __APPLE__
bool compile_osx() {
	errno = 0;

	char _path[MAX_PATH];
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}
	
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}

	memset(_path, 0, MAX_PATH);
	if(strcmp(build_type, "debug") == 0) {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/debug");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	} else {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/release");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;
	
	#define BUILD_ENGINE() 																					\
	do {																									\
		char _output[256];																					\
		memset(_output, 0, 256);																			\
		strcat(_output, this_file_full_path);																\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/debug/engine");				\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build/osx/debug/engine/");														\
		} else {																							\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/release/engine");				\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build/osx/release/engine/");													\
		}																									\
																											\
		char _output_engine[MAX_PATH];																		\
		memset(_output_engine, 0, MAX_PATH);																\
		strcat(_output_engine, _output);																	\
																											\
		arrput(_build_command, "clang");																	\
																											\
		unsigned int _module = 1;																			\
		for(int _i = 0; _i < MAX_MODULES; _i++) {															\
			if((modules & _module) == _module) {															\
						arrput(_build_command, MODULES_DEFINES[_i]);										\
			}																								\
			_module = _module << 1;																			\
		}																									\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			arrput(_build_command, "-g");																	\
			arrput(_build_command, "-O0");																	\
			arrput(_build_command, "-DRDE_DEBUG");															\
		} else {																							\
			arrput(_build_command, "-O3");																	\
		}																									\
																											\
		arrput(_build_command, "-std=c99");																	\
		FILE* _min_version_pipe = popen("sw_vers -productVersion","r");										\
	    char _buf[256];																						\
	    memset(_buf, 0, 256);																				\
	    if (_min_version_pipe != NULL ) {																	\
	    	int _i = 0;																						\
	        while (!feof(_min_version_pipe) && (_i < 256) ) {												\
	            fread(&_buf[_i++], 1, 1, _min_version_pipe);												\
	        }																								\
	        _buf[_i] = 0;																					\
	        _buf[_i - 1] = 0;																				\
	        _buf[_i - 2] = 0;																				\
	        pclose(_min_version_pipe);																		\
	    }																									\
	    else {																								\
	    	rde_log_level(RDE_LOG_LEVEL_ERROR, "Error getting the min macOS version");						\
			exit(-1);																						\
	    }																									\
	    char _min_version[MAX_PATH];																		\
		memset(_min_version, 0, MAX_PATH);																	\
		snprintf(_min_version, MAX_PATH, "-mmacosx-version-min=%s", _buf);									\
		arrput(_build_command, _min_version);																\
																											\
		if(strcmp(lib_type, "shared") == 0) {																\
			strcat(_output_engine, "libRDE.dylib");															\
			arrput(_build_command, "-shared");																\
			arrput(_build_command, "-fPIC");																\
		} else {																							\
			strcat(_output_engine, "libRDE.o");																\
			arrput(_build_command, "-c");																	\
		}																									\
																											\
		char _temp_path_0[MAX_PATH];																		\
		memset(_temp_path_0, 0, MAX_PATH);																	\
		snprintf(_temp_path_0, MAX_PATH, "%s%s", this_file_full_path, "engine/src/rde.c");					\
		arrput(_build_command, _temp_path_0);																\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_1[MAX_PATH];																		\
		memset(_temp_path_1, 0, MAX_PATH);																	\
		snprintf(_temp_path_1, MAX_PATH, "%s%s", this_file_full_path, "engine/include");					\
		arrput(_build_command, _temp_path_1);																\
																											\
		char _temp_path_2[MAX_PATH];																		\
		arrput(_build_command, "-I");																		\
		memset(_temp_path_2, 0, MAX_PATH);																	\
		snprintf(_temp_path_2, MAX_PATH, "%s%s", this_file_full_path, "engine/src");						\
		arrput(_build_command, _temp_path_2);																\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_3[MAX_PATH];																		\
		memset(_temp_path_3, 0, MAX_PATH);																	\
		snprintf(_temp_path_3, MAX_PATH, "%s%s", this_file_full_path, "external/include");					\
		arrput(_build_command, _temp_path_3);																\
																											\
		if(strcmp(lib_type, "shared") == 0) {																\
			arrput(_build_command, "-L");																	\
			char _temp_path_4[MAX_PATH];																	\
			memset(_temp_path_4, 0, MAX_PATH);																\
			snprintf(_temp_path_4, MAX_PATH, "%s%s", this_file_full_path, "external/libs/osx_x86_64");		\
																											\
			arrput(_build_command, _temp_path_4);															\
			arrput(_build_command, "-ldl");																	\
			arrput(_build_command, "-lm");																	\
			arrput(_build_command, "-lpthread");															\
			arrput(_build_command, "-lSDL2main");															\
			arrput(_build_command, "-lSDL2");																\
			arrput(_build_command, "-lglad");																\
			arrput(_build_command, "-lcglm");																\
		}																									\
		arrput(_build_command, "-Werror");																	\
		arrput(_build_command, "-Wall");																	\
		arrput(_build_command, "-Wextra");																	\
		arrput(_build_command, "-Wno-tautological-constant-out-of-range-compare");							\
																											\
		arrput(_build_command, "-o");																		\
		arrput(_build_command, _output_engine);																\
																											\
		if(!run_command(_build_command)) {																	\
			exit(-1);																						\
		}																									\
																											\
		if(strcmp(lib_type, "static") == 0) {																\
			rde_command _build_static_lib_command = NULL;													\
			arrput(_build_static_lib_command, "libtool");													\
			arrput(_build_static_lib_command, "-static");													\
			arrput(_build_static_lib_command, "-o");														\
																											\
			char _output_lib[MAX_PATH];																		\
			snprintf(_output_lib, MAX_PATH, "%s%s", _output, "libRDE.a");									\
			arrput(_build_static_lib_command, _output_lib);													\
			arrput(_build_static_lib_command, _output_engine);												\
			rde_log_level(RDE_LOG_LEVEL_INFO, "Building static lib with libtool...");						\
			if(!run_command(_build_static_lib_command)) {													\
				exit(-1);																					\
			}																								\
			rde_log_level(RDE_LOG_LEVEL_INFO, "Static library built!");										\
																											\
			rde_command _rm_dot_o_command = NULL;															\
			arrput(_rm_dot_o_command, "rm");																\
			arrput(_rm_dot_o_command, _output_engine);														\
			if(!run_command(_rm_dot_o_command)) {															\
				exit(-1);																					\
			}																								\
		}																									\
	} while(0);

	#define BUILD_TOOLS()																											\
	do {																															\
		char _output_a[256];																										\
		memset(_output_a, 0, 256);																									\
		strcat(_output_a, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/tools");													\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/tools/atlas_generator");									\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_a, "build/osx/tools/atlas_generator/");																		\
																																	\
		_build_command = NULL;																										\
		char output_atlas[MAX_PATH];																								\
		memset(output_atlas, 0, MAX_PATH);																							\
		strcat(output_atlas, _output_a);																							\
		strcat(output_atlas, "atlas_generator");																					\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
		arrput(_build_command, "-D_DEFAULT_SOURCE");																				\
																																	\
		char _ag_source_path[MAX_PATH];																								\
		memset(_ag_source_path, 0, MAX_PATH);																						\
		snprintf(_ag_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/atlas_generator/atlas_generator.c");				\
		arrput(_build_command, _ag_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _ag_include_path[MAX_PATH];																							\
		memset(_ag_include_path, 0, MAX_PATH);																						\
		snprintf(_ag_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/atlas_generator/external/include");				\
		arrput(_build_command, _ag_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-lm");																								\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_atlas);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
																																	\
		_build_command = NULL;																										\
		char _output_f[256];																										\
		memset(_output_f, 0, 256);																									\
		strcat(_output_f, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/tools/font_generator");									\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_f, "build/osx/tools/font_generator/");																		\
																																	\
		char output_fonts[MAX_PATH];																								\
		memset(output_fonts, 0, MAX_PATH);																							\
		strcat(output_fonts, _output_f);																							\
		strcat(output_fonts, "font_generator");																						\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _fg_source_path[MAX_PATH];																								\
		memset(_fg_source_path, 0, MAX_PATH);																						\
		snprintf(_fg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/font_generator.c");					\
		arrput(_build_command, _fg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _include_path[MAX_PATH];																								\
		memset(_include_path, 0, MAX_PATH);																							\
		snprintf(_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/external/include");					\
		arrput(_build_command, _include_path);																						\
																																	\
		arrput(_build_command, "-L");																								\
		char _lib_path[MAX_PATH];																									\
		memset(_lib_path, 0, MAX_PATH);																								\
		snprintf(_lib_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/external/libs/osx_x86_64");				\
		arrput(_build_command, _lib_path);																							\
																																	\
		arrput(_build_command, "-lfreetype");																						\
		arrput(_build_command, "-lz");																								\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_fonts);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
																																	\
		_build_command = NULL;																										\
		char _output_p[256];																										\
		memset(_output_p, 0, 256);																									\
		strcat(_output_p, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/osx/tools/project_generator");								\
		if(!make_dir_if_not_exists(_path)) {																						\
		exit(-1);																													\
		}																															\
		strcat(_output_f, "build/osx/tools/project_generator/");																	\
																																	\
		char output_project[MAX_PATH];																								\
		memset(output_project, 0, MAX_PATH);																						\
		strcat(output_project, _output_p);																							\
		strcat(output_project, "project_generator");																				\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _pg_source_path[MAX_PATH];																								\
		memset(_pg_source_path, 0, MAX_PATH);																						\
		snprintf(_pg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/project_generator.c");			\
		arrput(_build_command, _pg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _pg_include_path[MAX_PATH];																							\
		memset(_pg_include_path, 0, MAX_PATH);																						\
		snprintf(_pg_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/external/include");				\
		arrput(_build_command, _pg_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_project);																						\
																																	\
		if(!run_command(_build_command)) {																							\
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
		exit(-1);																													\
		}																															\
																																	\
		char _duck_img_src[MAX_PATH];																								\
		char _duck_img_dst[MAX_PATH];																								\
		memset(_duck_img_src, 0, MAX_PATH);																							\
		memset(_duck_img_dst, 0, MAX_PATH);																							\
		snprintf(_duck_img_src, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/duck_logo.png");					\
		snprintf(_duck_img_dst, MAX_PATH, "%s%s", this_file_full_path, "build/osx/tools/project_generator/duck_logo.png");			\
		copy_file_if_exists(_duck_img_src, _duck_img_dst); 																			\
	} while(0);

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		BUILD_ENGINE()
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		BUILD_TOOLS()
	}

	#undef BUILD_ENGINE
	#undef BUILD_TOOLS

	return true;
}
#endif

#if defined(__linux__)
bool compile_linux() {
	errno = 0;

	char _path[MAX_PATH];
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}
	
	memset(_path, 0, MAX_PATH);
	snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux");
	if(!make_dir_if_not_exists(_path)) {
		exit(-1);
	}

	memset(_path, 0, MAX_PATH);
	if(strcmp(build_type, "debug") == 0) {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/debug");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	} else {
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/release");
		if(!make_dir_if_not_exists(_path)) {
			exit(-1);
		}
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;
	
	#define BUILD_ENGINE() 																					\
	do {																									\
		char _output[256];																					\
		memset(_output, 0, 256);																			\
		strcat(_output, this_file_full_path);																\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/debug/engine");				\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build/linux/debug/engine/");													\
		} else {																							\
			memset(_path, 0, MAX_PATH);																		\
			snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/release/engine");			\
			if(!make_dir_if_not_exists(_path)) {															\
				exit(-1);																					\
			}																								\
			strcat(_output, "build/linux/release/engine/");													\
		}																									\
																											\
		char _output_engine[MAX_PATH];																		\
		memset(_output_engine, 0, MAX_PATH);																\
		strcat(_output_engine, _output);																	\
																											\
		arrput(_build_command, "clang");																	\
																											\
		unsigned int _module = 1;																			\
		for(int _i = 0; _i < MAX_MODULES; _i++) {															\
			if((modules & _module) == _module) {															\
				arrput(_build_command, MODULES_DEFINES[_i]);												\
			}																								\
			_module = _module << 1;																			\
		}																									\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			arrput(_build_command, "-g");																	\
			arrput(_build_command, "-O0");																	\
			arrput(_build_command, "-DRDE_DEBUG");															\
		} else {																							\
			arrput(_build_command, "-O3");																	\
		}																									\
																											\
		arrput(_build_command, "-std=c99");																	\
																											\
		if(strcmp(lib_type, "shared") == 0) {																\
			strcat(_output_engine, "libRDE.so");															\
			arrput(_build_command, "-shared");																\
			arrput(_build_command, "-fPIC");																\
		} else {																							\
			strcat(_output_engine, "libRDE.o");																\
			arrput(_build_command, "-c");																	\
		}																									\
																											\
		char _temp_path_0[MAX_PATH];																		\
		memset(_temp_path_0, 0, MAX_PATH);																	\
		snprintf(_temp_path_0, MAX_PATH, "%s%s", this_file_full_path, "engine/src/rde.c");					\
		arrput(_build_command, _temp_path_0);																\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_1[MAX_PATH];																		\
		memset(_temp_path_1, 0, MAX_PATH);																	\
		snprintf(_temp_path_1, MAX_PATH, "%s%s", this_file_full_path, "engine/include");					\
		arrput(_build_command, _temp_path_1);																\
																											\
		char _temp_path_2[MAX_PATH];																		\
		arrput(_build_command, "-I");																		\
		memset(_temp_path_2, 0, MAX_PATH);																	\
		snprintf(_temp_path_2, MAX_PATH, "%s%s", this_file_full_path, "engine/src");						\
		arrput(_build_command, _temp_path_2);																\
																											\
		arrput(_build_command, "-I");																		\
		char _temp_path_3[MAX_PATH];																		\
		memset(_temp_path_3, 0, MAX_PATH);																	\
		snprintf(_temp_path_3, MAX_PATH, "%s%s", this_file_full_path, "external/include");					\
		arrput(_build_command, _temp_path_3);																\
																											\
		if(strcmp(lib_type, "shared") == 0) {																\
			arrput(_build_command, "-L");																	\
			char _temp_path_4[MAX_PATH];																	\
			memset(_temp_path_4, 0, MAX_PATH);																\
			snprintf(_temp_path_4, MAX_PATH, "%s%s", this_file_full_path, "external/libs/linux");			\
																											\
			arrput(_build_command, _temp_path_4);															\
			arrput(_build_command, "-ldl");																	\
			arrput(_build_command, "-lm");																	\
			arrput(_build_command, "-lpthread");															\
			arrput(_build_command, "-lSDL2main");															\
			arrput(_build_command, "-lSDL2");																\
			arrput(_build_command, "-lglad");																\
			arrput(_build_command, "-lcglm");																\
		}																									\
		arrput(_build_command, "-Werror");																	\
		arrput(_build_command, "-Wall");																	\
		arrput(_build_command, "-Wextra");																	\
		arrput(_build_command, "-Wno-tautological-constant-out-of-range-compare");							\
																											\
		arrput(_build_command, "-o");																		\
		arrput(_build_command, _output_engine);																\
																											\
		if(!run_command(_build_command)) {																	\
			exit(-1);																						\
		}																									\
																											\
		if(strcmp(lib_type, "static") == 0) {																\
			rde_command _build_static_lib_command = NULL;													\
			arrput(_build_static_lib_command, "ar");														\
			arrput(_build_static_lib_command, "-crs");														\
																											\
			char _output_lib[MAX_PATH];																		\
			snprintf(_output_lib, MAX_PATH, "%s%s", _output, "libRDE.a");									\
			arrput(_build_static_lib_command, _output_lib);													\
			arrput(_build_static_lib_command, _output_engine);												\
			rde_log_level(RDE_LOG_LEVEL_INFO, "Building static lib with ar...");							\
			if(!run_command(_build_static_lib_command)) {													\
				exit(-1);																					\
			}																								\
			rde_log_level(RDE_LOG_LEVEL_INFO, "Static library built!");										\
																											\
			rde_command _rm_dot_o_command = NULL;															\
			arrput(_rm_dot_o_command, "rm");																\
			arrput(_rm_dot_o_command, _output_engine);														\
			if(!run_command(_rm_dot_o_command)) {															\
				exit(-1);																					\
			}																								\
		}																									\
	} while(0);

	#define BUILD_TOOLS()																											\
	do {																															\
		char _output_a[256];																										\
		memset(_output_a, 0, 256);																									\
		strcat(_output_a, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/tools");												\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/tools/atlas_generator");								\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_a, "build/linux/tools/atlas_generator/");																	\
																																	\
		_build_command = NULL;																										\
		char output_atlas[MAX_PATH];																								\
		memset(output_atlas, 0, MAX_PATH);																							\
		strcat(output_atlas, _output_a);																							\
		strcat(output_atlas, "atlas_generator");																					\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
		arrput(_build_command, "-D_DEFAULT_SOURCE");																				\
																																	\
		char _ag_source_path[MAX_PATH];																								\
		memset(_ag_source_path, 0, MAX_PATH);																						\
		snprintf(_ag_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/atlas_generator/atlas_generator.c");				\
		arrput(_build_command, _ag_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _ag_include_path[MAX_PATH];																							\
		memset(_ag_include_path, 0, MAX_PATH);																						\
		snprintf(_ag_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/atlas_generator/external/include");				\
		arrput(_build_command, _ag_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-lm");																								\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_atlas);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
		_build_command = NULL;																										\
		char _output_f[256];																										\
		memset(_output_f, 0, 256);																									\
		strcat(_output_f, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/tools/font_generator");									\
		if(!make_dir_if_not_exists(_path)) {																						\
				exit(-1);																											\
		}																															\
		strcat(_output_f, "build/linux/tools/font_generator/");																		\
																																	\
																																	\
		char output_fonts[MAX_PATH];																								\
		memset(output_fonts, 0, MAX_PATH);																							\
		strcat(output_fonts, _output_f);																							\
		strcat(output_fonts, "font_generator");																						\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _fg_source_path[MAX_PATH];																								\
		memset(_fg_source_path, 0, MAX_PATH);																						\
		snprintf(_fg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/font_generator.c");					\
		arrput(_build_command, _fg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _include_path[MAX_PATH];																								\
		memset(_include_path, 0, MAX_PATH);																							\
		snprintf(_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/external/include");					\
		arrput(_build_command, _include_path);																						\
																																	\
		arrput(_build_command, "-L");																								\
		char _lib_path[MAX_PATH];																									\
		memset(_lib_path, 0, MAX_PATH);																								\
		snprintf(_lib_path, MAX_PATH, "%s%s", this_file_full_path, "tools/font_generator/external/libs/linux");						\
		arrput(_build_command, _lib_path);																							\
																																	\
		arrput(_build_command, "-lfreetype");																						\
		arrput(_build_command, "-lz");																								\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_fonts);																						\
																																	\
		if(!run_command(_build_command)) {																							\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");														\
			exit(-1);																												\
		}																															\
																																	\
																																	\
		_build_command = NULL;																										\
		char _output_p[256];																										\
		memset(_output_p, 0, 256);																									\
		strcat(_output_p, this_file_full_path);																						\
																																	\
		memset(_path, 0, MAX_PATH);																									\
		snprintf(_path, MAX_PATH, "%s%s", this_file_full_path, "build/linux/tools/project_generator");								\
		if(!make_dir_if_not_exists(_path)) {																						\
		exit(-1);																													\
		}																															\
		strcat(_output_f, "build/linux/tools/project_generator/");																	\
																																	\
		char output_project[MAX_PATH];																								\
		memset(output_project, 0, MAX_PATH);																						\
		strcat(output_project, _output_p);																							\
		strcat(output_project, "project_generator");																				\
		arrput(_build_command, "clang");																							\
		arrput(_build_command, "-O3");																								\
		arrput(_build_command, "-std=c99");																							\
																																	\
		char _pg_source_path[MAX_PATH];																								\
		memset(_pg_source_path, 0, MAX_PATH);																						\
		snprintf(_pg_source_path, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/project_generator.c");			\
		arrput(_build_command, _pg_source_path);																					\
																																	\
		arrput(_build_command, "-I");																								\
		char _pg_include_path[MAX_PATH];																							\
		memset(_pg_include_path, 0, MAX_PATH);																						\
		snprintf(_pg_include_path, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/external/include");				\
		arrput(_build_command, _pg_include_path);																					\
																																	\
		arrput(_build_command, "-Werror");																							\
		arrput(_build_command, "-Wall");																							\
		arrput(_build_command, "-Wextra");																							\
																																	\
		arrput(_build_command, "-o");																								\
		arrput(_build_command, output_project);																						\
																																	\
		if(!run_command(_build_command)) {																							\
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");															\
		exit(-1);																													\
		}																															\
																																	\
		char _duck_img_src[MAX_PATH];																								\
		char _duck_img_dst[MAX_PATH];																								\
		memset(_duck_img_src, 0, MAX_PATH);																							\
		memset(_duck_img_dst, 0, MAX_PATH);																							\
		snprintf(_duck_img_src, MAX_PATH, "%s%s", this_file_full_path, "tools/project_generator/duck_logo.png");					\
		snprintf(_duck_img_dst, MAX_PATH, "%s%s", this_file_full_path, "build/linux/tools/project_generator/duck_logo.png");		\
		copy_file_if_exists(_duck_img_src, _duck_img_dst); 																			\
	} while(0);

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0) {
		printf("\n");
		printf("--- BUILDING ENGINE --- \n");
		BUILD_ENGINE()
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		printf("\n");
		printf("--- BUILDING TOOLS --- \n");
		BUILD_TOOLS()
	}

	#undef BUILD_ENGINE
	#undef BUILD_TOOLS

	return true;
}
#endif

bool compile_android() {
	errno = 0;
	assert(false && "compile_android not implemented");
	return false;
}

bool compile_ios() {
	errno = 0;
	assert(false && "compile_ios not implemented");
	return false;
}

bool compile_wasm() {
	errno = 0;
	assert(false && "compile_wasm not implemented");
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
	"--- PLATFORM SETTINGS --- \n"
	"To build for a specific platform, use --platform= (or -p=) and select a platform between: [windows, linux, osx, android, ios, wasm] \n"
	"If you are on a specific platform and select another one, an error will be prompted, only a platform can build another platform, except for \n"
	"ios an android. ios can be only built on an osx platform and android can be built on windows, osx and linux if the RDE's Android module \n"
	"is linked and the proper flags are set (check both ios and android sections for more information). \n"
	"If no platform is set, it will be assumed as the same as the OS.\n"
	"\n"
	"--- BUILD TYPES --- \n"
	"Builds of the library can be both [debug, release] and [static, shared] (each group can be combined as needed, no combinations of the same group). \n"
	"To set the build type use --build_type= (or -bt=) and one option between [debug, release]. \n"
	"To set the library type use --lib_type= (or -lt=) and one option between [static, shared]. Note that on windows this option is omitted as a .lib and .dll will be created. \n"
	"If no options are passed, it will be assumed -bt=debug and -lt=shared. \n"
	"\n"
	"-- BUILDING ---"
	"To build anything, use the option --build= (or -b=) and an action between [engine, tools, all, examples]. \n"
	"engine will compile the engine (only). tools will compile atlas_generator and font_generator (only). test will compile the examples and the engine (only). all will compile everything. \n"
	"If no option is passed, it will be assumed -b=all. \n"
	"\n"
	"-- MODULES ---\n"
	"RDE is composed of several modules, which are completely optional to use and include in your library. Those modules are: \n"
	"	- audio\n"
	"	- physics_2d\n"
	"	- physics_3d\n"
	"	- rendering_2d\n"
	"	- rendering_3d\n"
	"	- ui\n"
	"	- error\n"
	"Each of them can be added to your library. Have in mind that adding 'physics_3d' will add Jolt Physics Engine to your depenency list, in case \n"
	"you are building a static lib, you will probably have to add the Jolt lib to your build system.\n"
	"To add any of the modules use --modules= (or -m=) followed by an array of the included modules, for example:\n"
	"	--modules=\"[rendering_2d,rendering_3d,audio]\" \n\n"
	"Have in mind that the only module added by default to the library is 'rendering_2d'. If you specify --module option and you still \n"
	"want 'rendering_2d', you MUST include it in the modules list.\n"
	"Modules Description:"
	"	- audio: adds basic audio control, such as loading/unloading sounds, mixing them, play/pause/stop sounds, modify volume, pitches...\n"
	"	- physics_2d: basic 2D physics engine, with collisions, rigidbodies, handlers...\n"
	"	- physics_3d: 3d physics engine with jolt.\n"
	"	- rendering_2d: adds 2D rendering for 2D textures in a batched mode.\n"
	"	- rendering_3d: adds 3D rendering for custom meshes, models and basic lighting.\n"
	"	- ui: adds custom inmediate mode UI.\n"
	"	- error: adds handling for the most common error crashes and prints the stacktrace of the crash to the logs of the console (in debug) and to a error log file (in release).\n"
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

			_value++;
			strcat(build, _value);
		} else if(strcmp(_command, "-bt") == 0 || strcmp(_command, "--build_type") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(build_type, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -bt or --build_type incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(build_type, _value);
		} else if(strcmp(_command, "-lt") == 0 || strcmp(_command, "--lib_type") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(lib_type, 0 , MAX_SIZE_FOR_OPTIONS);

			if(_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -bt or --lib_type incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(lib_type, _value);
		} else if(strcmp(_command, "-m") == 0 || strcmp(_command, "--modules") == 0) {
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
		} else if(strcmp(_command, "-h") == 0 || strcmp(_command, "--help") == 0) {
			print_help();
			exit(0);
		}
	}

	int _osx_platforms = 0;
	if(strlen(platform) == 0) {
		#if _WIN32
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to windows");
		strcat(platform, "windows");
		#elif __APPLE__
			#include "TargetConditionals.h"
			#if TARGET_OS_MAC
			rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to osx");
			strcat(platform, "osx");
			#else
			_osx_platforms++;
			#endif
		#elif (defined(__linux__))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to linux");
		strcat(platform, "linux");
		#elif (defined(__ANDROID__))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to android");
		strcat(platform, "android");
		#elif __APPLE__
			#include "TargetConditionals.h"
			#if TARGET_OS_IPHONE
			rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to ios");
			strcat(platform, "ios");
			#else
			_osx_platforms++;
			#endif
		#elif __EMSCRIPTEN__
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to wasm");
		strcat(platform, "wasm");
		#else
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to compile from an unsupported platfom");
		exit(-1);
		#endif
	}

	if(_osx_platforms == 2 && strlen(platform) == 0) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to compile an unsupported platfom");
		exit(-1);
	}

	if(strlen(lib_type) == 0) {
		strcat(lib_type, "shared");
	}

	if(strlen(build_type) == 0) {
		strcat(build_type, "debug");
	}
}

bool compile_test() {
	return false;
}

bool run_test() {
	return false;
}

int main(int _argc, char** _argv) {
	memset(this_file_full_path, 0, MAX_PATH);

	#if _WIN32
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	char _temp[MAX_PATH];
	memset(_temp, 0, MAX_PATH);
	strcat(_temp, _argv[0]);
	const char _delimiter = '\\';
	const char* _value = strrchr(_argv[0], _delimiter);
	if(_value != NULL) {
		strncpy(this_file_full_path, _temp, strlen(_temp) - strlen(_value));
		strcat(this_file_full_path, "\\");
	} else {
		strcat(this_file_full_path, ".\\");
	}
	#else
	char* _temp = realpath(__FILE__, NULL);
	const char _delimiter = '/';
	const char* _value = strrchr(_temp, _delimiter);
	strncpy(this_file_full_path, _temp, strlen(_temp) - strlen(_value));
	strcat(this_file_full_path, "/");
	#endif

	try_recompile_and_redirect_execution(_argc, _argv);

	memset(platform, 0, MAX_SIZE_FOR_OPTIONS);
	memset(build_type, 0, MAX_SIZE_FOR_OPTIONS);
	memset(lib_type, 0, MAX_SIZE_FOR_OPTIONS);
	memset(build, 0, MAX_SIZE_FOR_OPTIONS);
	modules = RDE_MODULES_RENDERING_2D;

	parse_arguments(_argc, _argv);

	if(strcmp(platform, "windows") == 0) {
		#if _WIN32
		compile_windows();
		#endif
	} else if(strcmp(platform, "osx") == 0) {
		#if __APPLE__
		compile_osx();
		#endif
	} else if(strcmp(platform, "linux") == 0) {
		#if defined(__linux__)
		compile_linux();
		#endif
	} else if(strcmp(platform, "android") == 0) {
		compile_android();
	} else if(strcmp(platform, "ios") == 0) {
		compile_ios();
	} else if(strcmp(platform, "wasm") == 0) {
		compile_wasm();
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Selected platform is not supported '%s', exiting.", platform);
		exit(-1);
	}

	printf("\n");
	rde_log_level(RDE_LOG_LEVEL_INFO, "Build finished successfully. \n");
	return 0;
}

#pragma clang diagnostic pop