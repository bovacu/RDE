//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_MATH
#define GDE_PHYSICS_MATH

namespace GDE {

//#include <cmath> // abs, sqrt
//#include "core/physics/IEMath.h"
//
//    typedef float real;
//    typedef double real64;
//    typedef signed char	int8;
//    typedef signed short int16;
//    typedef signed int int32;
//    typedef unsigned char uint8;
//    typedef unsigned short uint16;
//    typedef unsigned int uint32;
//#ifdef WIN32 // these types are not standard, only exist in windows
//    typedef __int64 int64;
//typedef unsigned __int64 uint64;
//#endif
//    typedef float f32;
//    typedef double f64;
//
//    inline real dot( const Vec2F& a, const Vec2F& b )
//    {
//        return a.x * b.x + a.y * b.y;
//    }
//
//    inline real distSqr( const Vec2F& a, const Vec2F& b )
//    {
//        Vec2 c = a - b;
//        return dot( c, c );
//    }
//
//    inline Vec2F cross( const Vec2F& v, real a )
//    {
//        return { a * v.y, -a * v.x };
//    }
//
//    inline Vec2F cross( real a, const Vec2F& v )
//    {
//        return { -a * v.y, a * v.x };
//    }
//
//    inline real cross( const Vec2F& a, const Vec2F& b )
//    {
//        return a.x * b.y - a.y * b.x;
//    }
//
//// Comparison with tolerance of EPSILON
//    inline bool equal( real a, real b )
//    {
//        // <= instead of < for NaN comparison safety
//        return std::abs(a - b) <= EPSILON;
//    }
//
//    inline real sqr( real a )
//    {
//        return a * a;
//    }
//
//    inline real random( real l, real h )
//    {
//        real a = (real)rand( );
//        a /= (float)RAND_MAX;
//        a = (h - l) * a + l;
//        return a;
//    }
//
//    inline bool biasGreaterThan( real a, real b ) {
//        const real k_biasRelative = 0.95f;
//        const real k_biasAbsolute = 0.01f;
//        return a >= b * k_biasRelative + a * k_biasAbsolute;
//    }

}

#endif //GDE_PHYSICS_MATH