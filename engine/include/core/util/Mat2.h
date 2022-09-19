//
// Created by borja on 9/16/22.
//

#ifndef GDE_MAT2_H
#define GDE_MAT2_H

#include <cmath>
#include "Vec.h"

namespace GDE {

    #define degreesToRadians(x) x*(3.141592f/180.0f)

    class Mat2 {
        private:
            float values[2][2]{{0.0f, 0.0f}, {0.0f, 0.0f}};

        public:
            Mat2() {
                values[0][0] = 0.0f;
                values[0][1] = 0.0f;
                values[1][0] = 0.0f;
                values[1][1] = 0.0f;
            }

            explicit Mat2(float _degrees) {
                rotate(_degrees);
            }

            Mat2(float _m00, float _m01, float _m10, float _m11) {
                values[0][0] = _m00;
                values[0][1] = _m01;
                values[1][0] = _m10;
                values[1][1] = _m11;
            }

            void rotate(float _degrees) {
                auto _radians = degreesToRadians(_degrees);
                float _cos = std::cos( _radians );
                float _sin = std::sin( _radians );

                values[0][0] = _cos;
                values[0][1] = -_sin;
                values[1][0] = _sin;
                values[1][1] =  _cos;


            }

            [[nodiscard]] Mat2 abs() const {
                return { std::abs( values[0][0] ), std::abs( values[0][1] ), std::abs( values[1][0] ), std::abs( values[1][1] ) };
            }

            [[nodiscard]] Vec2F axisX() const {
                return Vec2F { values[0][0], values[1][0] };
            }

            [[nodiscard]] Vec2F axisY() const {
                return Vec2F { values[0][1], values[1][1] };
            }

            [[nodiscard]] Mat2 transpose() const {
                return { values[0][0], values[1][0], values[0][1], values[1][1] };
            }

            Vec2F operator*( const Vec2F& rhs ) const
            {
                return { values[0][0] * rhs.x + values[0][1] * rhs.y, values[1][0] * rhs.x + values[1][1] * rhs.y };
            }
    };

}

#endif //GDE_MAT2_H
