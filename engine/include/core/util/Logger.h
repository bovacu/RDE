//
// Created by borja on 3/12/21.
//

#ifndef RDE_LOGGER_H
#define RDE_LOGGER_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include "core/platform/PlatformHeaderSDL.h"

#if IS_WINDOWS()
#include <ctime>
#endif

#define LOG_I(...) printer("\033[1;m", "\033[0m", __VA_ARGS__);
#define LOG_W(...) printer("\033[1;33m", "\033[0m", __VA_ARGS__);
#define LOG_E(...) printer("\033[1;31m", "\033[0m", __VA_ARGS__);
#define LOG_S(...) printer("\033[1;32m", "\033[0m", __VA_ARGS__);
#define LOG_DEBUG(...) printerd("\033[1;m", "\033[0m", __VA_ARGS__);

#define LOG_I_TIME(...) {printTime("\033[1;m", "\033[0m"); printer("\033[1;m", "\033[0m", __VA_ARGS__);}
#define LOG_W_TIME(...) {printTime("\033[1;33m", "\033[0m"); printer("\033[1;33m", "\033[0m", __VA_ARGS__);}
#define LOG_E_TIME(...) {printTime("\033[1;31m", "\033[0m"); printer("\033[1;31m", "\033[0m", __VA_ARGS__);}
#define LOG_S_TIME(...) {printTime("\033[1;32m", "\033[0m"); printer("\033[1;32m", "\033[0m", __VA_ARGS__);}
#define LOG_DEBUG_TIME(...) {printTimed("\033[1;m", "\033[0m"); printerd("\033[1;m", "\033[0m", __VA_ARGS__);}

inline void printTime(const char* _init, const char* _end) {
    std::time_t t = std::time(nullptr);   // get time now

    #if IS_WINDOWS()
    std::tm _now {};
    localtime_s(&_now, &t);
    std::tm* now = &_now;
    #else
    std::tm* now = std::localtime(&t);
    #endif

    #if IS_DESKTOP()
    std::cout << _init << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] " << _end;
    #endif
}

template <typename Arg1>
inline void printer(const char* _init, const char* _end, Arg1&& arg1) {
#if IS_MOBILE()
    std::stringstream _stream;
    _stream << arg1;

    if(strcmp("\033[1;m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    } else if (strcmp("\033[1;33m", _init) == 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;31m", _init) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;32m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    }
#elif IS_DESKTOP()
    std::cout << _init << arg1 << _end << std::endl;
#endif
}


template <typename Arg1, typename... Args>
inline void printer(const char* _init, const char* _end, Arg1&& arg1, Args&&... args) {
#if IS_MOBILE()
    std::stringstream _stream;
    _stream << arg1;

    using expander = int[];
    (void)expander{0, (void(_stream << args), 0)...};

    if(strcmp("\033[1;m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    } else if (strcmp("\033[1;33m", _init) == 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;31m", _init) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;32m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    }
#elif IS_DESKTOP()
    std::cout << _init << arg1;
    printer(_init, _end, args...);
#endif
}



inline void printTimed(const char* _init, const char* _end) {
#ifdef ENGINE_DEBUG
    std::time_t t = std::time(nullptr);   // get time now

    #if IS_WINDOWS()
        std::tm _now {};
            localtime_s(&_now, &t);
            std::tm* now = &_now;
    #else
        std::tm* now = std::localtime(&t);
    #endif

    #if IS_DESKTOP()
        std::cout << _init << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] " << _end;
    #endif
#endif
}

template <typename Arg1>
inline void printerd(const char* _init, const char* _end, Arg1&& arg1) {
#ifdef ENGINE_DEBUG
#if IS_MOBILE()
    std::stringstream _stream;
    _stream << arg1;

    if(strcmp("\033[1;m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    } else if (strcmp("\033[1;33m", _init) == 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;31m", _init) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;32m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    }
#elif IS_DESKTOP()
    std::cout << _init << arg1 << _end << std::endl;
#endif
#endif
}


template <typename Arg1, typename... Args>
inline void printerd(const char* _init, const char* _end, Arg1&& arg1, Args&&... args) {
#ifdef ENGINE_DEBUG
#if IS_MOBILE()
    std::stringstream _stream;
    _stream << arg1;

    using expander = int[];
    (void)expander{0, (void(_stream << args), 0)...};

    if(strcmp("\033[1;m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    } else if (strcmp("\033[1;33m", _init) == 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;31m", _init) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
    } else if (strcmp("\033[1;32m", _init) == 0) {
        SDL_Log("%s", _stream.str().c_str());
    }
#elif IS_DESKTOP()
    std::cout << _init << arg1;
    printer(_init, _end, args...);
#endif
#endif
}

#endif //RDE_LOGGER_H
