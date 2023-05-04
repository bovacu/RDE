// Created by borja on 30/12/21.


#ifndef RDE_DEBUGSHAPE_H
#define RDE_DEBUGSHAPE_H

#include "core/util/Color.h"
#include "core/util/Functions.h"
#include "core/util/Rect.h"
#include "core/util/Vec.h"
#include <float.h>
#include <valarray>
#include <vector>

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
            Vec2F size {};

            void recalculateBoundingBox() {
                if(points.size() < 4) {
                    size = { 0, 0 };
                    return;
                }

                Vec2F _bottomestLeftPoint { FLT_MAX, FLT_MAX };
                Vec2F _topestRightPoint { FLT_MIN, FLT_MIN };

                for(auto& _point : points) {
                    _bottomestLeftPoint.x = _point.x < _bottomestLeftPoint.x ? _point.x : _bottomestLeftPoint.x;
                    _bottomestLeftPoint.y = _point.y < _bottomestLeftPoint.y ? _point.y : _bottomestLeftPoint.y;

                    _topestRightPoint.x = _point.x > _topestRightPoint.x ? _point.x : _topestRightPoint.x;
                    _topestRightPoint.y = _point.y > _topestRightPoint.y ? _point.y : _topestRightPoint.y;
                }

                size = { std::abs(_topestRightPoint.x) + std::abs(_bottomestLeftPoint.x), std::abs(_topestRightPoint.y) + std::abs(_bottomestLeftPoint.y) };
            }

        public:
            /**
             * @brief Angle in degrees.
             */
            float rotation = 0.f;

        public:

            RDE_FUNC_ND Vec2F getSize() const {
                return size;
            }

            RDE_FUNC_ND bool isPointInside(const Vec2F& _shapeOrigin, const Vec2F& _point) {
                return  _point.x >= _shapeOrigin.x - size.x * 0.5f && _point.x <= _shapeOrigin.x + size.x * 0.5f &&
                        _point.y >= _shapeOrigin.y - size.y * 0.5f && _point.y <= _shapeOrigin.y + size.y * 0.5f;
            }

            /**
             * @brief Returns the inner color.
             * @return Color
             */
			RDE_FUNC_ND Color& getInnerColor() {
                return innerColor;
            }

            /**
            * @brief Returns the outline color.
            * @return Color
            */
			RDE_FUNC_ND Color& getOuterColor() {
                return outerColor;
            }

            /**
            * @brief Returns the shape data.
            * @return std::vector<Vec2F>&
            */
			RDE_FUNC_ND std::vector<Vec2F>& getPoints() {
                return points;
            }

            /**
             * @brief Adds a new point to the geometry.
             * @param _point New point
             */
			RDE_FUNC void addPoint(const Vec2F& _point) {
                points.emplace_back(_point);
                recalculateBoundingBox();
            }

            /**
             * @brief Removes a point from the geometry.
             * @param _vertex Point to remove
             */
			RDE_FUNC void removePoint(int _vertex) {
                points.erase(points.begin() + _vertex);
            }

            /**
             * @brief Sets the outline color.
             * @param _color Outline Color
             */
			RDE_FUNC void setOutlineColor(const Color& _color) {
                outerColor = _color;
            }

            /**
             * @brief Sets the inner color.
             * @param _color Inner color
             */
			RDE_FUNC void setInnerColor(const Color& _color) {
                innerColor = _color;
            }

            /**
             * @brief Sets if the outline is shown or not.
             * @param _show Enable or disable the color
             */
			RDE_FUNC void showOutsideColor(bool _show) {
                showOuter = _show;
            }

            /**
             * @brief Sets if the inner is shown or not.
             * @param _show Enable or disable the color
             */
			RDE_FUNC void showInnerColor(bool _show) {
                showInner = _show;
            }

            /**
             * @brief Sets the position of the complex shape.
             * @param _position New position
             */
			RDE_FUNC void setPosition(const Vec2F& _position) {
                center = _position;
                recalculateBoundingBox();
            }

            /**
             * @brief Returns the position of the complex shape.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getPosition() {
//                return transform.getPositionLocal();
                return center;
            }

            /**
             * @brief Sets the rotation of the complex shape.
             * @param _rotation Rotation
             */
			RDE_FUNC void setRotation(float _rotation) {
                rotation = _rotation;
            }

            /**
             * @brief Gets the rotation of the complex shape.
             * @return float
             */
			RDE_FUNC_ND float getRotation() {
//                return transform.getRotationLocal();
                return {};
            }

            /**
             * @brief Returns if the inner color is drawn or not.
             * @return bool
             */
			RDE_FUNC_ND bool isInnerShown() {
                return showInner;
            }

            /**
             * @brief Returns if the outline color is drawn or not.
             * @return bool
             */
			RDE_FUNC_ND bool isOuterShown() {
                return showOuter;
            }

            /**
             * @brief Makes the complex shape a circle.
             * @param _center Center of the circle
             * @param _radius Radius of the circle
             * @param _precision How pretty the circle should be, the prettier, the more expensive in computing work
             */
			RDE_FUNC void makeCircle(const Vec2F& _center, float _radius, int _precision = 50) {
                for (int _i = 0; _i < _precision; _i++)   {
                    float _theta = 2.0f * 3.1415926f * float(_i) / float(_precision);
                    float _x = _radius * cosf(_theta);
                    float _y = _radius * sinf(_theta);
                    points.emplace_back(_x, _y);
                }

                center = _center;
                size = { _radius, _radius };
            }

            /**
             * @brief Makes the complex shape a square.
             * @param _position Position of the square
             * @param _size Size of the square
             */
			RDE_FUNC void makeSquare(const Vec2F& _position, const Vec2F& _size) {
                points.emplace_back(Vec2F{-_size.x / 2.f, -_size.y / 2.f});
                points.emplace_back(Vec2F{ _size.x / 2.f, -_size.y / 2.f});
                points.emplace_back(Vec2F{ _size.x / 2.f,  _size.y / 2.f});
                points.emplace_back(Vec2F{-_size.x / 2.f,  _size.y / 2.f});
				center = _position;
                size = _size;
			}
    };
}

#endif //RDE_DEBUGSHAPE_H
