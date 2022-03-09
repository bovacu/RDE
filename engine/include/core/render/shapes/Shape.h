// Created by borja on 30/12/21.


#ifndef ENGINE2_0_SHAPE_H
#define ENGINE2_0_SHAPE_H

#include "core/util/Util.h"
#include "engine/include/core/scene/Components.h"

namespace engine {
    class Shape {
        protected:
            Color innerColor = Color::Green, outerColor = Color::Green;
            bool showInner = true, showOuter = true;
            std::vector<Vec2F> points {};
            Vec2F center {};
            Transform transform;

        public:
            Color& getInnerColor() {
                return innerColor;
            }

            Color& getOuterColor() {
                return outerColor;
            }

            std::vector<Vec2F>& getPoints() {
                return points;
            }

            void addPoint(const Vec2F& _point) {
                points.emplace_back(_point);
            }

            void removePoint(int _vertex) {
                points.erase(points.begin() + _vertex);
            }

            void setOutlineColor(const Color& _color) {
                outerColor = _color;
            }

            void setInnerColor(const Color& _color) {
                innerColor = _color;
            }

            void showOutsideColor(bool _show) {
                showOuter = _show;
            }

            void showInnerColor(bool _show) {
                showInner = _show;
            }

            Transform& getTransform() {
                return transform;
            }

            void setPosition(const Vec2F& _position) {
                transform.setPosition(_position);
            }

            Vec2F getPosition() {
                return transform.getPosition();
            }

            void setRotation(float _rotation) {
                transform.setRotation(_rotation);
            }

            float getRotation() {
                return transform.getRotation();
            }

            bool isInnerShown() {
                return showInner;
            }

            bool isOuterShown() {
                return showOuter;
            }

            void makeCircle(const Vec2F& _center, float _radius, int _precision = 50) {
                for (int _i = 0; _i < _precision; _i++)   {
                    float _theta = 2.0f * 3.1415926f * float(_i) / float(_precision);
                    float _x = _radius * cosf(_theta);
                    float _y = _radius * sinf(_theta);
                    points.emplace_back(_x + _center.x, _y + _center.y);
                }

                center = _center;
            }

            void makeSquare(const Vec2F& _position, const Vec2F& _size) {
                points.emplace_back(Vec2F{_position.x - _size.x / 2.f, _position.y - _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x + _size.x / 2.f, _position.y - _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x + _size.x / 2.f, _position.y + _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x - _size.x / 2.f, _position.y + _size.y / 2.f});
            }
    };
}

#endif //ENGINE2_0_SHAPE_H
