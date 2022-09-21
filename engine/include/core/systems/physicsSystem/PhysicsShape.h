//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_SHAPE_H
#define GDE_PHYSICS_SHAPE_H

#include "core/graph/components/Transform.h"
#include "core/util/Mat2.h"
#include "core/render/shapes/DebugShape.h"

namespace GDE {

    struct PhysicsBody;

    #define MAX_VERTICES_PER_POLYGON 32

    /**
     * @warning THIS CANNOT BE ADDED AS A COMPONENT, IT IS PART OF THE PHYSICS_BODY.
     */
    struct PhysicsShape {

        enum Type {
            CIRCLE,
            POLYGON,
            BOX,
            MAX
        };

        PhysicsBody* physicsBody = nullptr;
        Transform* transform = nullptr;
        Vec2F size = {};
        Type type = Type::POLYGON;

        std::vector<Vec2F> vertices;
        std::vector<Vec2F> normals;

        int vertexCount = 0;

        void makeRectangle(const Vec2F& _size);
        void makePolygon(const std::vector<Vec2F>& _vertices);
        void makeCircle(float _radius);
        void rotate(float _degrees);
        Vec2F getSupport(const Vec2F& _dir);
        Mat2 getRotationMatrix();
    };
}


#endif //GDE_PHYSICS_SHAPE_H
