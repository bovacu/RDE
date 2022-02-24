// Created by borja on 15/2/22.


#ifndef ENGINE2_0_STRING_UTILS_H
#define ENGINE2_0_STRING_UTILS_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include "core/platform/PlatformHeaderSDL.h"
#include <string>
#include <vector>
#include <sstream>

#define SPLIT_S(str, delimiter) split(str, delimiter)
#define SPLIT_S_I(str, delimiter, index) splitGetIndex(str, delimiter, index)
#define APPEND_S(...) caller(__VA_ARGS__)
#define REPLACE_S(_str, _old, _new) replaceAll(_str, _old, _new)

inline std::vector<std::string> split(const std::string& _str, const std::string& _delimiter) {
    size_t pos = 0;
    std::string token;
    std::string _cpyStr = _str;
    std::vector<std::string> _splits;

    while ((pos = _cpyStr.find(_delimiter)) != std::string::npos) {
        token = _cpyStr.substr(0, pos);
        _splits.push_back(token);
        _cpyStr.erase(0, pos + _delimiter.length());
    }

    _splits.push_back(_cpyStr);
    return _splits;
}

inline std::string splitGetIndex(const std::string& _str, const std::string& _delimiter, int _index) {
    return split(_str, _delimiter)[_index];
}

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
