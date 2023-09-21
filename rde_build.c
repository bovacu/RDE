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

#define MAX_SIZE_FOR_OPTIONS 64
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
	}
	
	printf("\n");
	rde_log_level(RDE_LOG_LEVEL_INFO, "Executing command: %s", _command_flat);

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
	if(strlen(get_filename_extension(_argv[0])) == 0) {
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
		arrput(_recompile_command, "clang -g -O0 ");
		arrput(_recompile_command, __FILE__);
		arrput(_recompile_command, " -o ");
		
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
			arrput(_redirect_original_command, " ");
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
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not copy %s into %s. (err_msg) -> %s.", _file_path, _new_path, GetLastError());
		return false;
	}
	#else
	char _buf;
	FILE* _fd_0 = NULL;
	FILE* _fd_1 = NULL;

	_fd_0 = fopen(_file_path, "r");

	if (_fd_0 == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not open file %s. (err_msg) -> File could not be found.", _file_name);
		fclose(_fd_0);
		return false;
	}

	_fd_1 = fopen(_new_path, "w+");
	if (_fd_1 == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not create file %s.", _new_path);
		fclose(_fd_1);
		return false;
	}

	while(fread(_fd_0, &_buf, 1)) {
		fwrite(_fd_1, &_buf, 1);
	}

	close(_fd_0);
	close_(_fd_1);
	#endif

	rde_log_level(RDE_LOG_LEVEL_INFO, "Copied file %s to %s.", _file_path, _new_path);
	return true;
}






bool compile_windows() {
	errno = 0;
	if(!make_dir_if_not_exists("build")) {
		exit(-1);
	}
	
	if(!make_dir_if_not_exists("build\\windows")) {
		exit(-1);
	}

	if(strlen(build_type) == 0) {
		strcat(build_type, "debug");
	}

	if(strlen(output) == 0) {
		strcat(output, this_file_full_path);
		
		if(strcmp(build_type, "debug") == 0) {
			if(!make_dir_if_not_exists("build\\windows\\debug")) {
				exit(-1);
			}
			strcat(output, "build\\windows\\debug\\");
		} else {
			if(!make_dir_if_not_exists("build\\windows\\release")) {
				exit(-1);
			}
			strcat(output, "build\\windows\\release\\");
		}
	}

	if(strlen(build) == 0) {
		strcat(build, "all");
	}

	rde_command _build_command = NULL;
	
	#define BUILD_ENGINE() 																					\
	do {																									\
		char output_engine[MAX_PATH];																		\
		memset(output_engine, 0, MAX_PATH);																	\
		strcat(output_engine, output);																		\
		strcat(output_engine, "RDE.dll");																	\
		arrput(_build_command, "clang ");																	\
																											\
		if(strcmp(build_type, "debug") == 0) {																\
			arrput(_build_command, "-g -O0 ");																\
		} else {																							\
			arrput(_build_command, "-O3 ");																	\
		}																									\
																											\
		arrput(_build_command, "-std=c99 ");																\
																											\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "engine\\src\\rde.c ");														\
																											\
		arrput(_build_command, "-shared ");																	\
																											\
		arrput(_build_command, "-I ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "engine\\include ");															\
																											\
		arrput(_build_command, "-I ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "engine\\src ");																\
																											\
		arrput(_build_command, "-I ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "external\\include ");														\
																											\
		arrput(_build_command, "-L ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "external\\libs\\windows ");													\
																											\
		arrput(_build_command, "-L ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "external\\libs\\windows\\manual-link ");									\
																											\
		arrput(_build_command, "-lSDL2main -lSDL2 -lglad -lcglm ");											\
		arrput(_build_command, "-Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare ");	\
																											\
		arrput(_build_command, "-o ");																		\
		arrput(_build_command, output_engine);																\
																											\
		if(!run_command(_build_command)) {																	\
			exit(-1);																						\
		}																									\
	} while(0);

	#define BUILD_TOOLS()																					\
	do {																									\
		_build_command = NULL;																				\
		char output_atlas[MAX_PATH];																		\
		memset(output_atlas, 0, MAX_PATH);																	\
		strcat(output_atlas, output);																		\
		strcat(output_atlas, "atlas_generator.exe");														\
		arrput(_build_command, "clang++ ");																	\
		arrput(_build_command, "-O3 ");																		\
		arrput(_build_command, "-std=c++17 ");																\
																											\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "tools\\atlas_generator\\atlas_generator.cpp ");								\
																											\
		arrput(_build_command, "-I ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "tools\\atlas_generator\\external\\include ");								\
																											\
		arrput(_build_command, "-Werror -Wall -Wextra ");													\
																											\
		arrput(_build_command, "-o ");																		\
		arrput(_build_command, output_atlas);																\
																											\
		if(!run_command(_build_command)) {																	\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");								\
			exit(-1);																						\
		}																									\
																											\
		_build_command = NULL;																				\
		char output_fonts[MAX_PATH];																		\
		memset(output_fonts, 0, MAX_PATH);																	\
		strcat(output_fonts, output);																		\
		strcat(output_fonts, "font_generator.exe");															\
		arrput(_build_command, "clang ");																	\
		arrput(_build_command, "-O3 ");																		\
		arrput(_build_command, "-std=c99 ");																\
																											\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "tools\\font_generator\\font_generator.c ");									\
																											\
		arrput(_build_command, "-I ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "tools\\font_generator\\external\\include ");								\
																											\
		arrput(_build_command, "-L ");																		\
		arrput(_build_command, this_file_full_path);														\
		arrput(_build_command, "tools\\font_generator\\external\\libs ");									\
																											\
		arrput(_build_command, "-lfreetype ");																\
		arrput(_build_command, "-Werror -Wall -Wextra ");													\
																											\
		arrput(_build_command, "-o ");																		\
		arrput(_build_command, output_fonts);																\
																											\
		if(!run_command(_build_command)) {																	\
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Build engine returned error");								\
			exit(-1);																						\
		}																									\
																											\
	} while(0);

	if(strcmp(build, "engine") == 0 || strcmp(build, "all") == 0 || strcmp(build, "examples") == 0) {
		BUILD_ENGINE()
	}

	if(strcmp(build, "tools") == 0 || strcmp(build, "all") == 0) {
		BUILD_TOOLS()
	}

	if(strcmp(build, "examples") == 0 || strcmp(build, "all") == 0) {
		//BUILD_EXAMPLES()
	}

	return true;
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
	"If no option is passed, it will be assumed -b=all. \n";

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
		} else if(strcmp(_command, "-h") == 0 || strcmp(_command, "--help") == 0) {
			print_help();
			exit(0);
		}
	}

	if(strlen(platform) == 0) {
		#if _WIN32
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to windows");
		strcat(platform, "windows");
		#elif (defined(__APPLE__) && defined(MAC_PLATFORM))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to osx");
		strcat(platform, "osx");
		#elif (defined(__linux__))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to linux");
		strcat(platform, "linux");
		#elif (defined(__ANDROID__))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to android");
		strcat(platform, "android");
		#elif (defined(__APPLE__) && defined(IOS_PLATFORM))
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to ios");
		strcat(platform, "ios");
		#elif __EMSCRIPTEN__
		rde_log_level(RDE_LOG_LEVEL_INFO, "Platform set to wasm");
		strcat(platform, "wasm");
		#else
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to compile from an unsupported platfom");
		exit(-1);
		#endif
	}
}

bool compile_test() {
	return false;
}

bool run_test() {
	return false;
}

int main(int _argc, char** _argv) {
	#if _WIN32
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	char _temp[MAX_PATH];
	_fullpath(_temp, __FILE__, _MAX_PATH);
	const char _delimiter = '\\';
	const char* _value = strrchr(_temp, _delimiter);
	strncpy(this_file_full_path, _temp, strlen(_temp) - strlen(_value));
	strcat(this_file_full_path, "\\");
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

	parse_arguments(_argc, _argv);

	if(strcmp(platform, "windows") == 0) {
		if(compile_windows()) {
			printf("\n");
			rde_log_level(RDE_LOG_LEVEL_INFO, "Build finished successfully. \n");
		}
	} else if(strcmp(platform, "osx") == 0) {
		compile_osx();
	} else if(strcmp(platform, "linux") == 0) {
		compile_linux();
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

	return 0;
}

#pragma clang diagnostic pop