/*
    Copyright (c) 2013 Randy Gaul http://RandyGaul.net

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
*/

#ifndef IEMATH_H
#define IEMATH_H

// Impulse Engine Math : IEMath

#include <cmath> // abs, sqrt
#include <cassert> // assert
#include <algorithm> // max, min
#include "core/util/Vec.h"

typedef float real;
typedef double real64;
typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#ifdef WIN32 // these types are not standard, only exist in windows
typedef __int64 int64;
typedef unsigned __int64 uint64;
#endif
typedef float f32;
typedef double f64;

const real PI = 3.141592741f;
#ifndef EPSILON
#define EPSILON 0.0001f
#endif

struct Mat2
{
  union
  {
    struct
    {
      real m00, m01;
      real m10, m11;
    };

    real m[2][2];
    real v[4];
  };

  Mat2( ) {}
  Mat2( real radians )
  {
    real c = std::cos( radians );
    real s = std::sin( radians );

    m00 = c; m01 = -s;
    m10 = s; m11 =  c;
  }

  Mat2( real a, real b, real c, real d )
    : m00( a ), m01( b )
    , m10( c ), m11( d )
  {
  }

  void Set( real radians )
  {
    real c = std::cos( radians );
    real s = std::sin( radians );

    m00 = c; m01 = -s;
    m10 = s; m11 =  c;
  }

  Mat2 Abs( void ) const
  {
    return Mat2( std::abs( m00 ), std::abs( m01 ), std::abs( m10 ), std::abs( m11 ) );
  }

  GDE::Vec2F AxisX( void ) const
  {
    return GDE::Vec2F( m00, m10 );
  }

    GDE::Vec2F AxisY( void ) const
  {
    return GDE::Vec2F( m01, m11 );
  }

  Mat2 Transpose( void ) const
  {
    return Mat2( m00, m10, m01, m11 );
  }

  const GDE::Vec2F operator*( const GDE::Vec2F& rhs ) const
  {
    return GDE::Vec2F( m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y );
  }

  const Mat2 operator*( const Mat2& rhs ) const
  {
    // [00 01]  [00 01]
    // [10 11]  [10 11]

    return Mat2(
      m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
      m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
      m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
      m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
    );
  }
};

inline GDE::Vec2F Min( const GDE::Vec2F& a, const GDE::Vec2F& b )
{
  return GDE::Vec2F( std::min( a.x, b.x ), std::min( a.y, b.y ) );
}

inline GDE::Vec2F Max( const GDE::Vec2F& a, const GDE::Vec2F& b )
{
  return GDE::Vec2F( std::max( a.x, b.x ), std::max( a.y, b.y ) );
}

inline real Dot( const GDE::Vec2F& a, const GDE::Vec2F& b )
{
  return a.x * b.x + a.y * b.y;
}

inline real DistSqr( const GDE::Vec2F& a, const GDE::Vec2F& b )
{
    GDE::Vec2F c = a - b;
  return Dot( c, c );
}

inline GDE::Vec2F Cross( const GDE::Vec2F& v, real a )
{
  return GDE::Vec2F( a * v.y, -a * v.x );
}

inline GDE::Vec2F Cross( real a, const GDE::Vec2F& v )
{
  return GDE::Vec2F( -a * v.y, a * v.x );
}

inline real Cross( const GDE::Vec2F& a, const GDE::Vec2F& b )
{
  return a.x * b.y - a.y * b.x;
}

// Comparison with tolerance of EPSILON
inline bool Equal( real a, real b )
{
  // <= instead of < for NaN comparison safety
  return std::abs( a - b ) <= EPSILON;
}

inline real Sqr( real a )
{
  return a * a;
}

inline real Clamp( real min, real max, real a )
{
  if (a < min) return min;
  if (a > max) return max;
  return a;
}

inline int32 Round( real a )
{
  return (int32)(a + 0.5f);
}


inline bool BiasGreaterThan( real a, real b )
{
  const real k_biasRelative = 0.95f;
  const real k_biasAbsolute = 0.01f;
  return a >= b * k_biasRelative + a * k_biasAbsolute;
}

const f32 gravityScale = -5.0f;
const GDE::Vec2F gravity( 0, 10.0f * gravityScale );
const float dt = 1.0f / 60.0f;

#endif // IEMATH_H
