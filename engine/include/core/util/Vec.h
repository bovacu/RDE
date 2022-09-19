//
// Created by borja on 9/12/21.
//

#ifndef GDE_VEC_H
#define GDE_VEC_H

#include <ostream>
#include <cmath>
#include "glm/glm.hpp"

namespace GDE {

    template <class T>
    class Vec2 {
        public:
            T x;
            T y;

        public:
            Vec2() {
                x = 0;
                y = 0;
            }

            Vec2(T _x, T _y) {
                x = _x;
                y = _y;
            }

            Vec2<T> operator +(const Vec2<T> _vec) {
                return {x + _vec.x, y + _vec.y};
            }

            Vec2<T> operator +=(const Vec2<T> _vec) {
                return {x + _vec.x, y + _vec.y};
            }

            Vec2<T> operator -(const Vec2<T> _vec) {
                return {x - _vec.x, y - _vec.y};
            }

            Vec2<T> operator -(const Vec2<T> _vec) const {
                return {x - _vec.x, y - _vec.y};
            }

            Vec2<T> operator -=(const Vec2<T> _vec) {
                return {x - _vec.x, y - _vec.y};
            }

            float operator *(const Vec2<T> _vec) {
                return x * _vec.x + y * _vec.y;
            }

            float operator *=(float _scalar) {
                return x * _scalar + y * _scalar;
            }

            [[nodiscard]] float dotProduct(const Vec2<T>& _p) const {
                return x * _p.x + y * _p.y;
            }

            [[nodiscard]] float distance(const Vec2<T>& _p) const {
                return std::sqrt((_p.x - x) * (_p.x - x) + (_p.y - y) * (_p.y - y));
            }

            [[nodiscard]] float magnitude() const {
                return std::sqrt(x * x + y * y);
            }

            [[nodiscard]] float magnitudeSqr() const {
                return x * x + y * y;
            }

            void normalize() {
                float _magnitude = magnitude();
                float _invMagnitude = 1.0f / _magnitude;
                x *= _invMagnitude;
                y *= _invMagnitude;
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
    };

    inline Vec2<float> operator +(const Vec2<int>& _vec, const Vec2<float>& _vec1) {
        return {(float)_vec.x + _vec1.x, (float)_vec.y + _vec1.y};
    }

    inline Vec2<float> operator +(const Vec2<float>& _vec, const Vec2<int>& _vec1) {
        return _vec1 + _vec;
    }

    inline Vec2<float> operator -(const Vec2<int>& _vec, const Vec2<float>& _vec1) {
        return {(float)_vec.x - _vec1.x, (float)_vec.y - _vec1.y};
    }

    inline Vec2<float> operator -(const Vec2<float>& _vec, const Vec2<int>& _vec1) {
        return _vec1 - _vec;
    }

    inline float operator *(const Vec2<float>& _vec, const Vec2<int>& _vec1) {
        return _vec.x * (float)_vec1.x + _vec.y * (float)_vec1.y;
    }

    inline Vec2<float> operator *(const Vec2<float>& _vec, float _scalar) {
        return { _vec.x * _scalar, _vec.y * _scalar };
    }

    inline Vec2<float> operator /(const Vec2<float>& _vec, float _scalar) {
        return { _vec.x / _scalar, _vec.y / _scalar };
    }

    inline Vec2<float> operator *(float _scalar, const Vec2<float>& _vec) {
        return { _vec.x * _scalar, _vec.y * _scalar };
    }

    inline float operator *(const Vec2<int>& _vec, const Vec2<float>& _vec1) {
        return _vec1 * _vec;
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
        return _v0.x != _v1.x || _v0.y == _v1.y;
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
