// Created by borja on 15/2/22.


#ifndef ENGINE2_0_STRING_UTILS_H
#define ENGINE2_0_STRING_UTILS_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include "core/platform/PlatformHeaderSDL.h"

#define APPEND_S(...) caller(__VA_ARGS__);
#define REPLACE_S(_str, _old, _new) replaceAll(_str, _old, _new);

inline void replaceAll( std::string& _str, const std::string& _search, const std::string& _replace) {
    for( size_t pos = 0; ; pos += _replace.length() ) {
        pos = _str.find( _search, pos );
        if( pos == std::string::npos ) break;

        _str.erase( pos, _search.length() );
        _str.insert( pos, _replace );
    }
}

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

#endif //ENGINE2_0_STRING_UTILS_H
