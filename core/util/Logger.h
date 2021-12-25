//
// Created by borja on 3/12/21.
//

#ifndef RESOURCES_GAME_LOGGER_H
#define RESOURCES_GAME_LOGGER_H

#include <iostream>
#include <cstring>
#include <chrono>

#define LOG_I(...) printer("\033[1;m", "\033[0m", __VA_ARGS__);
#define LOG_W(...) printer("\033[1;33m", "\033[0m", __VA_ARGS__);
#define LOG_E(...) printer("\033[1;31m", "\033[0m", __VA_ARGS__);
#define LOG_S(...) printer("\033[1;32m", "\033[0m", __VA_ARGS__);

#define LOG_I_TIME(...) {printTime("\033[1;m", "\033[0m"); printer("\033[1;m", "\033[0m", __VA_ARGS__);}
#define LOG_W_TIME(...) {printTime("\033[1;33m", "\033[0m"); printer("\033[1;33m", "\033[0m", __VA_ARGS__);}
#define LOG_E_TIME(...) {printTime("\033[1;31m", "\033[0m"); printer("\033[1;31m", "\033[0m", __VA_ARGS__);}
#define LOG_S_TIME(...) {printTime("\033[1;32m", "\033[0m"); printer("\033[1;32m", "\033[0m", __VA_ARGS__);}

inline void printTime(const char* _init, const char* _end) {
    std::time_t t = std::time(nullptr);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << _init << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] " << _end;
}

template <typename Arg1>
inline void printer(const char* _init, const char* _end, Arg1&& arg1)
{
    std::cout << _init << arg1 << _end << std::endl;
}

template <typename Arg1, typename... Args>
inline void printer(const char* _init, const char* _end, Arg1&& arg1, Args&&... args)
{
    std::cout << _init << arg1;
    printer(_init, _end, args...);
}

#endif //RESOURCES_GAME_LOGGER_H
