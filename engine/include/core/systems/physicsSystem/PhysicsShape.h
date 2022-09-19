//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_SHAPE_H
#define GDE_PHYSICS_SHAPE_H

#include "core/graph/components/Transform.h"
#include "core/util/Mat2.h"
#include "core/render/shapes/DebugShape.h"

namespace GDE {

//    struct PhysicsBody;
//
//    #define MAX_VERTICES_PER_POLYGON 32
//
//    struct PhysicsShape {
//
//        enum Type {
//            CIRCLE,
//            POLYGON,
//            MAX
//        };
//
//        PhysicsBody* physicsBody = nullptr;
//        Transform* transform = nullptr;
//        Vec2F size = {};
//        Type type = Type::POLYGON;
//        Mat2 u;
//
//        std::vector<Vec2F> vertices;
//        std::vector<Vec2F> normals;
//
//        int vertexCount = 0;
//
//        void makeRectangle(const Vec2F& _size);
//        void makePolygon(const std::vector<Vec2F>& _vertices);
//        void makeCircle(float _radius);
//        PhysicsShape* clone();
//
//        void setOrient(float _degrees);
//
//        Vec2F getSupport(const Vec2F& dir);
//
//        DebugShape debugDraw();
//    };


}


#endif //GDE_PHYSICS_SHAPE_H
