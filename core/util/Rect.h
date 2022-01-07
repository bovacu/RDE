// Created by borja on 6/1/22.


#ifndef ENGINE2_0_RECT_H
#define ENGINE2_0_RECT_H

#include "Vec.h"

namespace engine {

    template<typename T>
    struct Rect {
        T bottomLeftCorner;
        T size;
    };

    typedef Rect<Vec2F> FloatRect;
    typedef Rect<Vec2I> IntRect;

    inline std::ostream& operator<<(std::ostream& _os, const IntRect& _rect){
        _os << "(bottomLeft: [" << _rect.bottomLeftCorner.x << ", " << _rect.bottomLeftCorner.y << "], size: [" << _rect.size.x << ", " << _rect.size.y << ')';
        return _os;
    }


}

#endif //ENGINE2_0_RECT_H
