//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_MATH
#define GDE_PHYSICS_MATH

namespace GDE {

    class PhysicsMath {
        public:
            static bool approximatelyEqual(float _a, float _b) {
                return std::abs( _a - _b ) <= EPSILON;
            }

            static bool biasGreaterThan(float _a, float _b) {
                const float _biasRelative = 0.95f;
                const float _biasAbsolute = 0.01f;
                return _a >= _b * _biasRelative + _a * _biasAbsolute;
            }
    };

}

#endif //GDE_PHYSICS_MATH