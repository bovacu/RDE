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
                const float k_biasRelative = 0.95f;
                const float k_biasAbsolute = 0.01f;
                return _a >= _b * k_biasRelative + _a * k_biasAbsolute;
            }
    };

}

#endif //GDE_PHYSICS_MATH