#include "Shape.h"

namespace engine {

    Polygon::Polygon(const std::vector<Vec2F>& _vertices) : vertices(_vertices) {
        for(int _v = 0; _v < _vertices.size(); _v++) {
            Vec2F _v0, _v1;

            if(_v < _vertices.size() - 1) {
                _v0 = _vertices[_v];
                _v1 = _vertices[_v + 1];
            } else {
                _v0 = _vertices[_v];
                _v1 = _vertices[0];
            }

            const float _dX = _v0.x - _v1.x;
            const float _dY = _v0.y - _v1.y;
            this->angles.emplace_back(std::atan2(_dY, _dX));
        }
    }

    Circle::Circle(const Vec2F& _center, float _radius, int _precision) : center(_center), radius(_radius) {

        const float _angleToAdd = (float)(M_PI / 180.f) * (360.f / 50.f);
        float _angle = 0.f;

        for(int _v = 0; _v < _precision; _v++) {
            float _x = _center.x + _radius * std::cos(_angle);
            float _y = _center.x + _radius * std::sin(_angle);

            this->vertices.emplace_back(_x, _y);

            _angle += _angleToAdd;
        }

        for(int _v = 0; _v < this->vertices.size(); _v++) {
            Vec2F _v0, _v1;

            if(_v < this->vertices.size() - 1) {
                _v0 = this->vertices[_v];
                _v1 = this->vertices[_v + 1];
            } else {
                _v0 = this->vertices[_v];
                _v1 = this->vertices[0];
            }

            const float _dX = _v0.x - _v1.x;
            const float _dY = _v0.y - _v1.y;
            this->angles.emplace_back(std::atan2(_dY, _dX));
        }
    }
}