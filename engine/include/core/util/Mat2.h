//
// Created by borja on 9/16/22.
//

#ifndef RDE_MAT2_H
#define RDE_MAT2_H

#include <cmath>
#include "core/util/Vec.h"
#include "core/Core.h"
#include "core/util/Functions.h"

namespace RDE {

    struct Mat2 {
        union {
            struct {
                float m00, m01;
                float m10, m11;
            };

            float m[2][2];
            float v[4];
        };

        public:
			RDE_FUNC Mat2() {
                m[0][0] = 0.0f;
                m[0][1] = 0.0f;
                m[1][0] = 0.0f;
                m[1][1] = 0.0f;
            }

			RDE_FUNC_EXPLICIT Mat2(float _degrees) {
                rotate(_degrees);
            }

			RDE_FUNC Mat2(float _m00, float _m01, float _m10, float _m11) {
                m[0][0] = _m00;
                m[0][1] = _m01;
                m[1][0] = _m10;
                m[1][1] = _m11;
            }

			RDE_FUNC void rotate(float _degrees) {
                auto _radians = Util::Math::degreesToRadians(_degrees);
                float _cos = std::cos( _radians );
                float _sin = std::sin( _radians );

                m00 = _cos; m01 = -_sin;
                m10 = _sin; m11 =  _cos;
            }

			RDE_FUNC_ND Mat2 abs() const {
                return { std::abs( m00 ), std::abs( m01 ), std::abs( m10 ), std::abs( m11 ) };
            }

			RDE_FUNC_ND Vec2F axisX() const {
                return Vec2F { m00, m10 };
            }

			RDE_FUNC_ND Vec2F axisY() const {
                return Vec2F { m01, m11 };
            }

			RDE_FUNC_ND Mat2 transpose() const {
                return { m00, m10, m01, m11 };
            }

			RDE_FUNC Vec2F operator*(const Vec2F& rhs) const {
                return { m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y };
            }

			RDE_FUNC Mat2 operator*(const Mat2& rhs) const {
                return {
                        m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
                        m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
                        m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
                        m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
                };
            }
    };

}

#endif //RDE_MAT2_H
