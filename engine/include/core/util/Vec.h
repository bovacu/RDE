//
// Created by borja on 9/12/21.
//

#ifndef GDE_VEC_H
#define GDE_VEC_H

#include <ostream>
#include <cmath>
#include "glm/glm.hpp"
#include "core/Core.h"

namespace GDE {

    template <class T>
    struct Vec2 {
        union {
            T m[1][1];
            T v[2];

            struct {
                T x;
                T y;
            };
        };

        public:
            Vec2() {
                x = 0;
                y = 0;
            }

            Vec2(T _x, T _y) {
                x = _x;
                y = _y;
            }

            void set(T _x, T _y) {
                x = _x;
                y = _y;
            }

            void set(const Vec2<T>& _vec) {
                x = _vec.x;
                y = _vec.y;
            }

            [[nodiscard]] float dotProduct(const Vec2<T>& _p) const {
                return x * _p.x + y * _p.y;
            }

            [[nodiscard]] Vec2<T> crossProduct(float _scalar) const {
                return { -_scalar * y, _scalar * x };
            }

            [[nodiscard]] float crossProduct(const Vec2<T> _vec) const {
                return x * _vec.y - y * _vec.x;
            }

            [[nodiscard]] float distance(const Vec2<T>& _p) const {
                return std::sqrt((_p.x - x) * (_p.x - x) + (_p.y - y) * (_p.y - y));
            }

            [[nodiscard]] float distanceSqr(const Vec2<T>& _p) const {
                Vec2<T> c = *this - _p;
                return c.dotProduct(c);
            }

            [[nodiscard]] float magnitude() const {
                return std::sqrt(x * x + y * y);
            }

            [[nodiscard]] float magnitudeSqr() const {
                return x * x + y * y;
            }

            void normalize() {
                float _magnitude = magnitude();

                if(_magnitude > EPSILON) {
                    float _invMagnitude = 1.0f / _magnitude;
                    x *= _invMagnitude;
                    y *= _invMagnitude;
                }
            }

            void rotate(float _degrees) {
                auto _radians = glm::radians(_degrees);
                float _c = std::cos(_radians);
                float _s = std::sin(_radians);

                float _xRotated = x * _c - y * _s;
                float _yRotated = x * _s + y * _c;

                x = _xRotated;
                y = _yRotated;
            }

            bool isInside(const Vec2<T>& _rectCenter, const Vec2<T>& _rectSize) {
                return x >= _rectCenter.x - _rectSize.x / 2.f && x <= _rectCenter.x + _rectSize.x / 2.f &&
                       y >= _rectCenter.y - _rectSize.y / 2.f && y <= _rectCenter.y + _rectSize.y / 2.f;
            }

            Vec2<T> operator+(const Vec2<T>& _rhs) const {
                return Vec2( x + _rhs.x, y + _rhs.y );
            }

            Vec2<T> operator+(float _s) const {
                return Vec2( x + _s, y + _s );
            }

            void operator+=(const Vec2& _rhs) {
                x += _rhs.x;
                y += _rhs.y;
            }



            Vec2<T> operator-(const Vec2<T>& _rhs) const {
                return Vec2( x - _rhs.x, y - _rhs.y );
            }

            Vec2<T> operator-(float _s) const {
                return Vec2( x - _s, y - _s );
            }

            void operator-=(const Vec2<T>& _rhs) {
                x -= _rhs.x;
                y -= _rhs.y;
            }

            Vec2<T> operator -() const {
                return { -x, -y };
            }


            Vec2<T> operator*( float _scalar ) const {
                return Vec2<T>( x * _scalar, y * _scalar );
            }

            void operator *=(float _scalar) {
                x *= _scalar;
                y *= _scalar;
            }


            Vec2<T> operator/( float _scalar ) const {
                return Vec2<T>(x / _scalar, y / _scalar);
            }

            void operator /=(float _scalar) {
                x /= _scalar;
                y /= _scalar;
            }
    };

    inline Vec2<float> operator*(float _scalar, const Vec2<float>& _vec) {
        return { _scalar * _vec.x, _scalar * _vec.y };
    }

    inline Vec2<int> operator*(float _scalar, const Vec2<int>& _vec) {
        return { (int)(_scalar * (float)_vec.x), (int)(_scalar * (float)_vec.y) };
    }

    inline Vec2<double> operator*(float _scalar, const Vec2<double>& _vec) {
        return { _scalar * _vec.x, _scalar * _vec.y };
    }

    inline Vec2<long> operator*(float _scalar, const Vec2<long>& _vec) {
        return { (long)(_scalar * (float)_vec.x), (long)(_scalar * (float)_vec.y) };
    }

    typedef Vec2<int> Vec2I;
    typedef Vec2<float> Vec2F;

    inline std::ostream& operator<<(std::ostream& _os, const Vec2I& _vec){
        _os << '(' << _vec.x << ", " << _vec.y << ')';
        return _os;
    }

    inline std::ostream& operator<<(std::ostream& _os, const Vec2F& _vec){
        _os << '(' << _vec.x << ", " << _vec.y << ')';
        return _os;
    }

    inline bool operator==(const Vec2F & _v0, int _num) {
        return _v0.x == (float)_num && _v0.y == (float)_num;
    }

    inline bool operator==(const Vec2F & _v0, float _num) {
        return _v0.x == _num && _v0.y == _num;
    }

    inline bool operator<(const Vec2F & _v0, float _num) {
        return _v0.x < _num || _v0.y < _num;
    }

    inline bool operator<=(const Vec2F & _v0, float _num) {
        return _v0.x <= _num || _v0.y <= _num;
    }

    inline bool operator>(const Vec2F & _v0, float _num) {
        return _v0.x > _num || _v0.y > _num;
    }

    inline bool operator>=(const Vec2F & _v0, float _num) {
        return _v0.x >= _num || _v0.y >= _num;
    }

    inline bool operator!=(const Vec2F & _v0, const Vec2F & _v1) {
        return _v0.x != _v1.x || _v0.y != _v1.y;
    }

    inline bool operator==(const Vec2I & _v0, int _num) {
        return _v0.x == _num && _v0.y == _num;
    }

    inline bool operator==(const Vec2I & _v0, float _num) {
        return _v0.x == _num && _v0.y == _num;
    }


    inline bool operator!=(const Vec2F & _v0, int _num) {
        return !(_v0 == (float)_num);
    }

    inline bool operator!=(const Vec2F & _v0, float _num) {
        return !(_v0 == _num);
    }

    inline bool operator!=(const Vec2I & _v0, int _num) {
        return !(_v0 == _num);
    }

    inline bool operator!=(const Vec2I & _v0, float _num) {
        return !(_v0 == _num);
    }

}

#endif //GDE_VEC_H
