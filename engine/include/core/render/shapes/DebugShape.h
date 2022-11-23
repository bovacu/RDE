// Created by borja on 30/12/21.


#ifndef RDE_DEBUGSHAPE_H
#define RDE_DEBUGSHAPE_H

#include "core/util/Util.h"

namespace RDE {

    /**
     * @brief This class represents a Complex Geometry debugging shape.
     */
    class DebugShape {

        protected:
            /**
             * @brief Color of the inner part of the Shape.
             */
            Color innerColor = Color::Green;

            /**
             * @brief Color of the outline.
             */
            Color outerColor = Color::Blue;

            /**
             * @brief If the inner color is rendered.
             */
            bool showInner = true;

            /**
             * @brief If the outline color is rendered.
             */
            bool showOuter = true;

            /**
             * @brief Points that define the complex shape.
             */
            std::vector<Vec2F> points {};

            /**
             * @brief Center of the complex shape.
             */
            Vec2F center {};

        public:
            /**
             * @brief Angle in degrees.
             */
            float rotation = 0.f;

        public:

            /**
             * @brief Returns the inner color.
             * @return Color
             */
            Color& getInnerColor() {
                return innerColor;
            }

            /**
            * @brief Returns the outline color.
            * @return Color
            */
            Color& getOuterColor() {
                return outerColor;
            }

            /**
            * @brief Returns the shape data.
            * @return std::vector<Vec2F>&
            */
            std::vector<Vec2F>& getPoints() {
                return points;
            }

            /**
             * @brief Adds a new point to the geometry.
             * @param _point New point
             */
            void addPoint(const Vec2F& _point) {
                points.emplace_back(_point);
            }

            /**
             * @brief Removes a point from the geometry.
             * @param _vertex Point to remove
             */
            void removePoint(int _vertex) {
                points.erase(points.begin() + _vertex);
            }

            /**
             * @brief Sets the outline color.
             * @param _color Outline Color
             */
            void setOutlineColor(const Color& _color) {
                outerColor = _color;
            }

            /**
             * @brief Sets the inner color.
             * @param _color Inner color
             */
            void setInnerColor(const Color& _color) {
                innerColor = _color;
            }

            /**
             * @brief Sets if the outline is shown or not.
             * @param _show Enable or disable the color
             */
            void showOutsideColor(bool _show) {
                showOuter = _show;
            }

            /**
             * @brief Sets if the inner is shown or not.
             * @param _show Enable or disable the color
             */
            void showInnerColor(bool _show) {
                showInner = _show;
            }

            /**
             * @brief Sets the position of the complex shape.
             * @param _position New position
             */
            void setPosition(const Vec2F& _position) {
//                transform.setPosition(_position);
            }

            /**
             * @brief Returns the position of the complex shape.
             * @return Vec2F
             */
            Vec2F getPosition() {
//                return transform.getPositionLocal();
                return {};
            }

            /**
             * @brief Sets the rotation of the complex shape.
             * @param _rotation Rotation
             */
            void setRotation(float _rotation) {
                rotation = _rotation;
            }

            /**
             * @brief Gets the rotation of the complex shape.
             * @return float
             */
            float getRotation() {
//                return transform.getRotationLocal();
                return {};
            }

            /**
             * @brief Returns if the inner color is drawn or not.
             * @return bool
             */
            bool isInnerShown() {
                return showInner;
            }

            /**
             * @brief Returns if the outline color is drawn or not.
             * @return bool
             */
            bool isOuterShown() {
                return showOuter;
            }

            /**
             * @brief Makes the complex shape a circle.
             * @param _center Center of the circle
             * @param _radius Radius of the circle
             * @param _precision How pretty the circle should be, the prettier, the more expensive in computing work
             */
            void makeCircle(const Vec2F& _center, float _radius, int _precision = 50) {
                for (int _i = 0; _i < _precision; _i++)   {
                    float _theta = 2.0f * 3.1415926f * float(_i) / float(_precision);
                    float _x = _radius * cosf(_theta);
                    float _y = _radius * sinf(_theta);
                    points.emplace_back(_x + _center.x, _y + _center.y);
                }

                center = _center;
            }

            /**
             * @brief Makes the complex shape a square.
             * @param _position Position of the square
             * @param _size Size of the square
             */
            void makeSquare(const Vec2F& _position, const Vec2F& _size) {
                points.emplace_back(Vec2F{_position.x - _size.x / 2.f, _position.y - _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x + _size.x / 2.f, _position.y - _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x + _size.x / 2.f, _position.y + _size.y / 2.f});
                points.emplace_back(Vec2F{_position.x - _size.x / 2.f, _position.y + _size.y / 2.f});
            }
    };
}

#endif //RDE_DEBUGSHAPE_H
