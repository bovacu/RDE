#pragma once

#ifndef ENGINE2_0_SHAPE_H
#define ENGINE2_0_SHAPE_H

#include "core/util/Util.h"

namespace engine {

    class Shape {
        public:
            virtual std::vector<Vec2F>& getVertices()   = 0;
            virtual std::vector<float>& getAngles()     = 0;
    };

    class Polygon : public Shape {
        private:
            std::vector<Vec2F> vertices;
            std::vector<float> angles;

        public:
            explicit Polygon(const std::vector<Vec2F>& _vertices);

            std::vector<Vec2F>& getVertices()  override { return this->vertices; }
            std::vector<float>& getAngles()    override { return this->angles; }
    };

    class Circle : public Shape {
        private:
            std::vector<Vec2F> vertices;
            std::vector<float> angles;
            float radius;
            Vec2F center;

        public:
            Circle(const Vec2F& _center, float _radius, int _precision = 50);

            std::vector<Vec2F>& getVertices()  override { return this->vertices; }
            std::vector<float>& getAngles()    override { return this->angles; }
    };

}

#endif //ENGINE2_0_SHAPE_H
