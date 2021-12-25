//
// Created by borja on 9/12/21.
//

#ifndef RESOURCES_GAME_VEC_H
#define RESOURCES_GAME_VEC_H

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
        this->x = _x;
        this->y = _y;
    }

    Vec2<T> operator +(const Vec2<T> _vec) {
        return {this->x + _vec.x, this->y + _vec.y};
    }

    Vec2<T> operator -(const Vec2<T> _vec) {
        return {this->x - _vec.x, this->y - _vec.y};
    }

    float operator *(const Vec2<T> _vec) {
        return this->x * _vec.x + this->y * _vec.y;
    }

    float dotProduct(const Vec2<T>& _p) const {
        return this->x * _p.x + this->y * _p.y;
    }

    float distance(const Vec2<T>& _p) const {
        return std::sqrt((_p.x - this->x) * (_p.x - this->x) + (_p.y - this->y) * (_p.y - this->y));
    }

    float magnitude() const {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    static Vec2<float> normalize(Vec2<T>& _p) {
        float _magnitude = _p.magnitude();
        return Vec2<float>(_p.x / _magnitude, _p.y / _magnitude);
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

#endif //RESOURCES_GAME_VEC_H
