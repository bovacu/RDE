//
// Created by borja on 9/16/22.
//

#ifndef RDE_PHYSICS_MATH
#define RDE_PHYSICS_MATH

namespace RDE {

    class PhysicsMath {
        public:
            static bool approximatelyEqual(float _a, float _b) {
                return std::abs( _a - _b ) <= EPSILON;
            }

            static bool approximatelyEqual(float _a, float _b, float _epsilon) {
                return std::abs( _a - _b ) <= _epsilon;
            }

            static bool approximatelyEqual(const Vec2F& _a, const Vec2F &_b) {
                return std::abs( _a.x - _b.x ) <= EPSILON && std::abs(_a.y - _b.y) <= EPSILON;
            }

            static bool approximatelyEqual(const Vec2F& _a, const Vec2F &_b, float _epsilon) {
                return std::abs( _a.x - _b.x ) <= _epsilon && std::abs(_a.y - _b.y) <= _epsilon;
            }

            static bool biasGreaterThan(float _a, float _b) {
                const float _biasRelative = 0.95f;
                const float _biasAbsolute = 0.01f;
                return _a >= _b * _biasRelative + _a * _biasAbsolute;
            }
    };

}

#endif //RDE_PHYSICS_MATH