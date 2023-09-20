#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

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
	#define rde_proc_invalid INVALID_HANDLE_VALUE;
#else
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <dirent.h>
	typedef int rde_proc;
	#define rde_proc_invalid (-1);
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


char this_file_full_path[MAX_PATH];

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...);
void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

const char *get_filename_ext(const char* _file_name);

bool pid_wait(rde_proc _pid);
bool run_command(rde_command _command);
int needs_rebuild(const char* _output_path, const char** _input_paths, size_t _input_paths_count);
void try_recompile(int _argc, char** _argv);

bool make_dir_if_not_exists(const char* _dir_path);
bool create_file_if_not_exists(const char* _file_path);
bool rename_file_if_exists(const char* _file, const char* _new_name);
bool remove_file_if_exists(const char* _file_path);
bool remove_dir_recursively_if_exists(const char* _file_path);

bool compile_windows();
bool compile_osx();
bool compile_linux();
bool compile_android();
bool compile_ios();
bool compile_wasm();

void parse_arguments(int _argc, char** _argv);

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
	switch(_color) {
		case RDE_LOG_COLOR_RED: {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 12);
			#else
			fprintf("\033[0;31m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_GREEN:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 10);
			#else
			fprintf("\033[0;32m");	
			#endif
		} break;
		
		case RDE_LOG_COLOR_YELLOW:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 6);
			#else
			fprintf("\033[0;33m");	
			#endif
		} break;
		
		case RDE_LOG_COLOR_BLUE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 9);
			#else
			fprintf("\033[0;34m");		
			#endif
		} break;
		
		case RDE_LOG_COLOR_PURPLE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 5);
			#else
			fprintf("\033[0;35m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_CYAN:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 11);
			#else
			fprintf("\033[0;36m");
			#endif
		} break;
		
		case RDE_LOG_COLOR_WHITE:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 15);
			#else
			fprintf("\033[0;37m");
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
			fprintf("\033[0;31m");
			#endif
			fprintf(stdout, "[ERROR] ");
		} break;
		
		case RDE_LOG_LEVEL_WARNING:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 6);
			#else
			fprintf("\033[0;33m");	
			#endif
			fprintf(stdout, "[WARNING] ");
		} break;
		
		case RDE_LOG_LEVEL_DEBUG:  {
			#if _WIN32
			SetConsoleTextAttribute(console_handle, 11);
			#else
			fprintf("\033[0;36m");
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

const char *get_filename_ext(const char* _file_name) {
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
    if (_pid == INVALID_HANDLE_VALUE) {
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

        if (WIFEXITED(wstatus)) {
            int _exit_status = WEXITSTATUS(wstatus);
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

bool run_command(rde_command _command) {
	errno = 0;
    if (arrlen(_command) < 1) {
        rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not run empty command");
        return false;
    }

    char* _command_flat = (char*)malloc(sizeof(char) * 2048);
	memset(_command_flat, 0, 2048);

	for(int _i = 0; _i < arrlen(_command); _i++) {
		strcat(_command_flat, _command[_i]);
		strcat(_command_flat, " ");
	}

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
		free(_command_flat);
		arrfree(_command);
        return false;
    }

    if (_pid == 0) {
        if (execvp(_command_flat[0], (char * const*)_command_flat) < 0) {
            rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not exec child process: %s", strerror(errno));
			free(_command_flat);
			arrfree(_command);
            return false;
        }
		free(_command_flat);
		arrfree(_command);
    }
	
	return pid_wait(_pid);
	#endif
}

void try_recompile_and_redirect_execution(int _argc, char** _argv) {
	errno = 0;
	const char* _source_path = __FILE__;

	char _binary_path[MAX_PATH];
	memset(_binary_path, 0, MAX_PATH);

	#if _WIN32
	// In windows we can execute a exe without using .exe in the name. This program assumes that it will always contains .exe so if it does not have it, it is added.
	if(strlen(get_filename_ext(_argv[0])) == 0) {
		strcat(_binary_path, _argv[0]);
		strcat(_binary_path, ".exe");	
	} else {
		strcpy(_binary_path, _argv[0]);
	}
	
	#else
	strcat(_binary_path, _argv[0]);
	#endif
	int _needs_recompile = needs_recompile(_binary_path, &_source_path, 1);
	if(_needs_recompile < 0) {
		exit(-1);
	}

	if(_needs_recompile) {
		// First we move the current executable to trash
		
		const char _delimiter = '\\';
		// Be careful, if no delimter is found, then _just_binary_name will be NULL and we will use _binary_path directly
		char* _just_binary_name = strrchr(_binary_path, _delimiter);

		char _to_old[MAX_PATH];
		memset(_to_old, 0, MAX_PATH);
		if(_just_binary_name != NULL) {
			_just_binary_name++;
			strcat(_to_old, _just_binary_name);
		} else {
			strcat(_to_old, _binary_path);
		}
		strcat(_to_old, ".old");

		#if _WIN32
		char _to_trash[MAX_PATH];
		memset(_to_trash, 0, MAX_PATH);
		// This is the way of deleting the old exe in Windows
		strcat(_to_trash, "C:\\$Recycle.Bin\\");
		
		if(_just_binary_name != NULL) {
			_just_binary_name++;
			strcat(_to_trash, _just_binary_name);
		} else {
			strcat(_to_trash, _binary_path);
		}
		strcat(_to_trash, ".old");


		if(!rename_file_if_exists(_binary_path, _to_old)) {
			exit(-1);
		}
		#else
		if(!rename_file_if_exists(_binary_path, _to_old)) {
			exit(-1);
		}
		#endif

		// Then we recompile it
		rde_log_level(RDE_LOG_LEVEL_INFO, "Recompiling %s first", _just_binary_name != NULL ? _just_binary_name : _binary_path);
		rde_command _recompile_command = NULL;
		arrput(_recompile_command, "clang");
		arrput(_recompile_command, __FILE__);
		arrput(_recompile_command, "-o");
		
		if(_just_binary_name != NULL) {
			arrput(_recompile_command, _just_binary_name);
		} else {
			arrput(_recompile_command, _binary_path);
		}

		char _old_binary_path[MAX_PATH];
		strcat(_old_binary_path, _binary_path);
		strcat(_old_binary_path, ".old");
		if(!run_command(_recompile_command)) {
			if(!rename_file_if_exists(_old_binary_path, _just_binary_name != NULL ? _just_binary_name : _binary_path)) {
				exit(-1);
			}
			exit(-1);
		} else {
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
		run_command(_redirect_original_command);

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
		rde_log_level(RDE_LOG_LEVEL_ERROR, "failed to create directory '%s'. (err_msg) -> %s", _dir_path, strerror(errno));
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
	rename(_file, _new_name)
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
						_r2 = remove_directory(_buf);
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

bool compile_windows() {
	errno = 0;
	assert(false && "compile_windows not implemented");
	return false;
}

bool compile_osx() {
	errno = 0;
	assert(false && "compile_osx not implemented");
	return false;
}

bool compile_linux() {
	errno = 0;
	assert(false && "compile_linux not implemented");
	return false;
}

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

void parse_arguments(int _argc, char** _argv) {
	
}

int main(int _argc, char** _argv) {
	#if _WIN32
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	char _temp[MAX_PATH];
	_fullpath(_temp, __FILE__, _MAX_PATH);
	const char _delimiter = '\\';
	const char* _value = strrchr(_temp, _delimiter);
	strncpy(this_file_full_path, _temp, strlen(_temp) - strlen(_value));
	#else
	char* _temp = realpath(__FILE__, NULL);
	const char _delimiter = '\\';
	const char* _value = strrchr(_temp, _delimiter);
	strncpy(this_file_full_path, _temp, strlen(_temp) - strlen(_value));
	#endif

	(void)_argc;
	(void)_argv;
	try_recompile_and_redirect_execution(_argc, _argv);
	rde_log_level(RDE_LOG_LEVEL_INFO, "So this worked 17 times!!!");

	return 0;
}

#pragma clang diagnostic pop