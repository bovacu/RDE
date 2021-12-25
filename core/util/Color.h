// Created by borja on 24/12/21.


#ifndef ENGINE2_0_COLOR_H
#define ENGINE2_0_COLOR_H

struct Color {
    unsigned char r, g, b, a;

    Color() : r(0), g(0), b(0), a(0) {  }
    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) : r(_r), g(_g), b(_b), a(_a) {  }

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
};

#endif //ENGINE2_0_COLOR_H
