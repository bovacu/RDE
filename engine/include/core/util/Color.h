// Created by borja on 24/12/21.


#ifndef RDE_COLOR_H
#define RDE_COLOR_H

#include <ostream>
#include "core/Core.h"

#if defined(_WIN32) || __EMSCRIPTEN__
    typedef unsigned int uint;
#endif

namespace RDE {

    struct Color {
        unsigned char r, g, b, a;

		RDE_FUNC Color() : r(0), g(0), b(0), a(0) {  }
		RDE_FUNC Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) : r(_r), g(_g), b(_b), a(_a) {  }

        static const Color White;
        static const Color Black;
        static const Color Blue;
        static const Color Red;
        static const Color Yellow;
        static const Color Magenta;
        static const Color Orange;
        static const Color Green;
        static const Color Brown;
        static const Color Purple;
        static const Color Gray;
        static const Color Transparent;
        static const Color Disabled;
        static const Color Placeholder;
        static const Color NO_COLOR;
    };

	RDE_FUNC inline std::ostream& operator<<(std::ostream& _os, const Color& _color){
        _os << '(' << (uint)_color.r << ", " << (uint)_color.g << ", " << (uint)_color.b << ", " << (uint)_color.a << ')';
        return _os;
    }

	RDE_FUNC inline bool operator==(const Color& _c0, const Color& _c1) {
        return _c0.r == _c1.r && _c0.g == _c1.g && _c0.b == _c1.b && _c0.a == _c1.a;
    }

}

#endif //RDE_COLOR_H
