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
	#include "dirent/dirent.h"
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

#define MAX_SIZE_FOR_PROJECT_NAME 256
char project_name[MAX_SIZE_FOR_PROJECT_NAME];
char this_file_full_path[MAX_PATH];

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...);
void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

const char* get_filename_extension(const char* _file_name);

bool pid_wait(rde_proc _pid);
bool run_command(rde_command _command);

bool make_dir_if_not_exists(const char* _dir_path);
bool create_file_if_not_exists(const char* _file_path);
bool rename_file_if_exists(const char* _file, const char* _new_name);
bool remove_file_if_exists(const char* _file_path);
bool remove_dir_recursively_if_exists(const char* _file_path);

char* read_full_file_if_exists(const char* _file_path); // You are responsible for deallocating the returned memory
bool write_to_file_if_exists(const char* _file_path, const char* _contents);
char* replace_string(const char* str, const char* from, const char* to); // This function is greacefully stolen from https://creativeandcritical.net/str-replace-c, so thank you so much

bool copy_file_if_exists(const char* _file_path, const char* _new_path);

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

	rde_log_level(RDE_LOG_LEVEL_INFO, "Written data to %s successfully", _file_path);
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

void parse_arguments(int _argc, char** _argv);

const char* RDE_CODE_TEMPLATE = ""
"#include \"rde.h\"\n"
"\n"
"#define ENGINE_DEBUG\n"
"\n"
"rde_window* _window = NULL;\n"
"rde_texture* _duck_texture = NULL;\n"
"\n"
"void on_update(float _dt);\n"
"void on_fixed_update(float _dt);\n"
"void on_late_update(float _dt);\n"
"void on_render(float _dt, rde_window* _window);\n"
"\n"
"const rde_end_user_mandatory_callbacks _mandatory_callbacks = {\n"
"	&on_update,\n"
"	&on_fixed_update,\n"
"	&on_late_update,\n"
"	&on_render\n"
"};\n"
"\n"
"const rde_engine_heap_allocs_config _heap_allocs_config = {\n"
"	.max_number_of_windows = 5,\n"
"	.max_number_of_vertices_per_batch = 60000,\n"
"	.max_number_of_shaders = 5,\n"
"	.max_number_of_textures = 10,\n"
"	.max_number_of_atlases = 10,\n"
"	.max_number_of_fonts = 10,\n"
"	.max_number_of_models = 5,\n"
"	.max_number_of_models_textures = 5,\n"
"	.max_number_of_sounds = 5\n"
"};\n"
"\n"
"void on_update(float _dt) {\n"
"	UNUSED(_dt)\n"
"}\n"
"\n"
"void on_fixed_update(float _dt) {\n"
"	UNUSED(_dt)\n"
"}\n"
"\n"
"void on_late_update(float _dt) {\n"
"	UNUSED(_dt)\n"
"}\n"
"\n"
"void on_render(float _dt, rde_window* _window) {\n"
"	UNUSED(_dt)\n"
"	rde_rendering_set_background_color(RDE_COLOR_RED);\n"
"\n"
"	rde_transform _t = rde_struct_create_transform();\n"
"	rde_rendering_2d_begin_drawing(&camera, _window);\n"
"	rde_rendering_2d_draw_texture(&_t, _center, RDE_COLOR_WHITE, NULL);\n"
"	rde_rendering_2d_end_drawing();\n"
"}\n"
"\n"
"void init_func(int _argc, char** _argv) {\n"
"	(void)_argc;\n"
"	(void)_argv;\n"
"\n"
"	_duck_texture = rde_rendering_texture_load(\"duck.png\", NULL);\n"
"}\n"
"\n"
"RDE_MAIN(_window, _heap_allocs_config, _mandatory_callbacks, init_func);\n"
"";

void print_help() {
	const char* _help_message = ""
		"This is the RDE's project generator. \n"
		"To generate the project use --name= (or -n=) and the name of your project. \n";
	rde_log_level(RDE_LOG_LEVEL_INFO, _help_message);
}

void parse_arguments(int _argc, char** _argv) {
	if(_argc < 2) {
		print_help();
		exit(0);
	}

	const char* _delimiter = "=";
	const char _delimiter_2 = '=';
	for (int _i = 1; _i < _argc; _i++) {
		char _command_copy[256];
		memset(_command_copy, 0, 256);
		strcpy(_command_copy, _argv[_i]);
		char* _command = strtok(_command_copy, _delimiter);
		
		if (strcmp(_command, "-n") == 0 || strcmp(_command, "--name") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			memset(project_name, 0 , MAX_SIZE_FOR_PROJECT_NAME);

			if (_value == NULL) {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Argument for -n or --name incorrect\n");
				exit(-1);
			}

			_value++;
			strcat(project_name, _value);
		} else if (strcmp(_command, "-h") == 0 || strcmp(_command, "--help") == 0) {
			print_help();
			exit(0);
		}
	}
}

int main(int _argc, char** _argv) {	
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

	parse_arguments(_argc, _argv);

	if(!make_dir_if_not_exists(project_name)) {
		exit(-1);
	}

	char _file_to_copy[MAX_PATH];
	memset(_file_to_copy, 0, MAX_PATH);
	snprintf(_file_to_copy, MAX_PATH, "%s""duck_logo.png", this_file_full_path);

	char _destination_path[MAX_PATH];
	memset(_destination_path, 0, MAX_PATH);
	
#if _WIN32
	snprintf(_destination_path, MAX_PATH, "%s\\""duck_logo.png", project_name);
#else
	snprintf(_destination_path, MAX_PATH, "%s/""duck_logo.png", project_name);
#endif

	if(!copy_file_if_exists(_file_to_copy, _destination_path)) {
		exit(-1);
	}

	char _source_file_path[MAX_PATH];
	memset(_source_file_path, 0, MAX_PATH);

#if _WIN32
	snprintf(_source_file_path, MAX_PATH, "%s\\""%s.c", project_name, project_name);
#else
	snprintf(_source_file_path, MAX_PATH, "%s/""%s.c", project_name, project_name);
#endif

	if(!create_file_if_not_exists(_source_file_path)) {
		exit(-1);
	}

	if(!write_to_file_if_exists(_source_file_path, RDE_CODE_TEMPLATE)) {
		exit(-1);
	}

	rde_log_level(RDE_LOG_LEVEL_INFO, "Project template for '%s' generated successfully", project_name);

	return 0;
}