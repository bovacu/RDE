// Created by borja on 15/2/22.


#ifndef ENGINE2_0_APPENDER_H
#define ENGINE2_0_APPENDER_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include "core/platform/PlatformHeaderSDL.h"

#define APPEND_S(...) caller(__VA_ARGS__);


template <typename Arg1>
inline std::string append(std::stringstream& _ss, Arg1&& arg1) {
    _ss << arg1;
    return _ss.str();
}

template <typename Arg1, typename... Args>
inline std::string append(std::stringstream& _ss, Arg1&& arg1, Args&&... args) {
    _ss << arg1;
    return append(_ss, args...);
}

template <typename Arg1, typename... Args>
inline std::string caller(Arg1&& arg1, Args&&... args) {
    std::stringstream _ss;
    return append(_ss, arg1, args...);
}

#endif //ENGINE2_0_APPENDER_H
