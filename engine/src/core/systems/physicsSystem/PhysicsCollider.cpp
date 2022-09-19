//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsCollider.h"
#include "core/util/Mat2.h"

namespace GDE {

    CollisionCallback Dispatch[PhysicsShape::Type::MAX][PhysicsShape::Type::MAX] =
    {
        {
            circleToCircle, circleToPolygon
        },
        {
            polygonToCircle, polygonToPolygon
        },
    };

    void circleToCircle(PhysicsManifold* m, PhysicsBody* a, PhysicsBody* b) {
        auto* A = a->shape;
        auto* B = b->shape;

        // Calculate translational vector, which is normal
        Vec2F normal = b->position - a->position;

        real dist_sqr = normal.magnitudeSqr();
        real radius = A->size.x + B->size.x;

        // Not in contact
        if(dist_sqr >= radius * radius) {
            m->contact_count = 0;
            return;
        }

        real distance = std::sqrt( dist_sqr );

        m->contact_count = 1;

        if(distance == 0.0f) {
            m->penetration = A->size.x;
            m->normal = Vec2F { 1, 0 };
            m->contacts [0] = a->position;
        } else {
            m->penetration = radius - distance;
            m->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
            m->contacts[0] = m->normal * A->size.x + a->position;
        }
    }

    void circleToPolygon(PhysicsManifold* m, PhysicsBody* a, PhysicsBody* b) {
        auto* A = a->shape; // Circle
        auto* B = b->shape; // Polygon

        m->contact_count = 0;

        // Transform circle center to Polygon model space
        Vec2 center = a->position;
        center = B->u.transpose( ) * (center - b->position);

        // Find edge with minimum penetration
        // Exact concept as using support points in Polygon vs Polygon
        real separation = -FLT_MAX;
        uint32 faceNormal = 0;
        for(uint32 i = 0; i < B->vertexCount; ++i)
        {
            real s = dot( B->normals[i], center - B->vertices[i] );

            if(s > A->size.x)
                return;

            if(s > separation)
            {
                separation = s;
                faceNormal = i;
            }
        }

        // Grab face's vertices
        Vec2 v1 = B->vertices[faceNormal];
        uint32 i2 = faceNormal + 1 < B->vertexCount ? faceNormal + 1 : 0;
        Vec2 v2 = B->vertices[i2];

        // Check to see if center is within polygon
        if(separation < EPSILON)
        {
            m->contact_count = 1;
            m->normal = { -(B->u * B->normals[faceNormal]).x, -(B->u * B->normals[faceNormal]).y };
            m->contacts[0] = m->normal * A->size.x + a->position;
            m->penetration = A->size.x;
            return;
        }

        // Determine which voronoi region of the edge center of circle lies within
        real dot1 = dot( center - v1, v2 - v1 );
        real dot2 = dot( center - v2, v1 - v2 );
        m->penetration = A->size.x - separation;

        // Closest to v1
        if(dot1 <= 0.0f)
        {
            if(distSqr( center, v1 ) > A->size.x * A->size.x)
                return;

            m->contact_count = 1;
            Vec2 n = v1 - center;
            n = B->u * n;
            n.normalize( );
            m->normal = n;
            v1 = B->u * v1 + b->position;
            m->contacts[0] = v1;
        }

            // Closest to v2
        else if(dot2 <= 0.0f)
        {
            if(distSqr( center, v2 ) > A->size.x * A->size.x)
                return;

            m->contact_count = 1;
            Vec2 n = v2 - center;
            v2 = B->u * v2 + b->position;
            m->contacts[0] = v2;
            n = B->u * n;
            n.normalize( );
            m->normal = n;
        }

            // Closest to face
        else
        {
            Vec2 n = B->normals[faceNormal];
            if(dot( center - v1, n ) > A->size.x)
                return;

            n = B->u * n;
            m->normal = { -n.x, -n.y };
            m->contacts[0] = m->normal * A->size.x + a->position;
            m->contact_count = 1;
        }
    }

    void polygonToCircle(PhysicsManifold* m, PhysicsBody* a, PhysicsBody* b) {
        circleToPolygon( m, b, a );
        m->normal = { -m->normal.x, -m->normal.y };
    }

    real FindAxisLeastPenetration( uint32 *faceIndex, PhysicsShape *A, PhysicsShape *B )
    {
        real bestDistance = -FLT_MAX;
        uint32 bestIndex;

        for(uint32 i = 0; i < A->vertexCount; ++i)
        {
            // Retrieve a face normal from A
            Vec2 n = A->normals[i];
            Vec2 nw = A->u * n;

            // Transform face normal into B's model space
            Mat2 buT = B->u.transpose( );
            n = buT * nw;

            // Retrieve support point from B along -n
            Vec2 s = B->getSupport({ -n.x, -n.y });

            // Retrieve vertex on face from A, transform into
            // B's model space
            Vec2F v = A->vertices[i];
            v = A->u * v + A->physicsBody->position;
            v = v - B->physicsBody->position;
            v = buT * v;

            // Compute penetration distance (in B's model space)
            real d = dot( n, s - v );

            // Store greatest distance
            if(d > bestDistance)
            {
                bestDistance = d;
                bestIndex = i;
            }
        }

        *faceIndex = bestIndex;
        return bestDistance;
    }

    void FindIncidentFace( Vec2F *v, PhysicsShape *RefPoly, PhysicsShape *IncPoly, uint32 referenceIndex )
    {
        Vec2 referenceNormal = RefPoly->normals[referenceIndex];

        // Calculate normal in incident's frame of reference
        referenceNormal = RefPoly->u * referenceNormal; // To world space
        referenceNormal = IncPoly->u.transpose( ) * referenceNormal; // To incident's model space

        // Find most anti-normal face on incident polygon
        int32 incidentFace = 0;
        real minDot = FLT_MAX;
        for(int i = 0; i < IncPoly->vertexCount; ++i)
        {
            real _dot = dot(referenceNormal, IncPoly->normals[i] );
            if(_dot < minDot)
            {
                minDot = _dot;
                incidentFace = i;
            }
        }

        // Assign face vertices for incidentFace
        v[0] = IncPoly->u * IncPoly->vertices[incidentFace] + IncPoly->physicsBody->position;
        incidentFace = incidentFace + 1 >= IncPoly->vertexCount ? 0 : incidentFace + 1;
        v[1] = IncPoly->u * IncPoly->vertices[incidentFace] + IncPoly->physicsBody->position;
    }

    int32 Clip(Vec2F n, real c, Vec2F *face )
    {
        uint32 sp = 0;
        Vec2F out[2] = {
                face[0],
                face[1]
        };

        // Retrieve distances from each endpoint to the line
        // d = ax + by - c
        real d1 = dot( n, face[0] ) - c;
        real d2 = dot( n, face[1] ) - c;

        // If negative (behind plane) clip
        if(d1 <= 0.0f) out[sp++] = face[0];
        if(d2 <= 0.0f) out[sp++] = face[1];

        // If the points are on different sides of the plane
        if(d1 * d2 < 0.0f) // less than to ignore -0.0f
        {
            // Push interesection point
            real alpha = d1 / (d1 - d2);
            out[sp] = face[0] + alpha * (face[1] - face[0]);
            ++sp;
        }

        // Assign our new converted values
        face[0] = out[0];
        face[1] = out[1];

        assert( sp != 3 );

        return sp;
    }

    void polygonToPolygon(PhysicsManifold* m, PhysicsBody* a, PhysicsBody* b) {
        auto* A = a->shape;
        auto* B = b->shape;
        m->contact_count = 0;

        // Check for a separating axis with A's face planes
        uint32 faceA;
        real penetrationA = FindAxisLeastPenetration( &faceA, A, B );
        if(penetrationA >= 0.0f)
            return;

        // Check for a separating axis with B's face planes
        uint32 faceB;
        real penetrationB = FindAxisLeastPenetration( &faceB, B, A );
        if(penetrationB >= 0.0f)
            return;

        uint32 referenceIndex;
        bool flip; // Always point from a to b

        PhysicsShape *RefPoly; // Reference
        PhysicsShape *IncPoly; // Incident

        // Determine which shape contains reference face
        if(biasGreaterThan( penetrationA, penetrationB ))
        {
            RefPoly = A;
            IncPoly = B;
            referenceIndex = faceA;
            flip = false;
        }

        else
        {
            RefPoly = B;
            IncPoly = A;
            referenceIndex = faceB;
            flip = true;
        }

        // World space incident face
        Vec2F incidentFace[2];
        FindIncidentFace( incidentFace, RefPoly, IncPoly, referenceIndex );

        //        y
        //        ^  ->n       ^
        //      +---c ------posPlane--
        //  x < | i |\
  //      +---+ c-----negPlane--
        //             \       v
        //              r
        //
        //  r : reference face
        //  i : incident poly
        //  c : clipped point
        //  n : incident normal

        // Setup reference face vertices
        Vec2 v1 = RefPoly->vertices[referenceIndex];
        referenceIndex = referenceIndex + 1 == RefPoly->vertexCount ? 0 : referenceIndex + 1;
        Vec2 v2 = RefPoly->vertices[referenceIndex];

        // Transform vertices to world space
        v1 = RefPoly->u * v1 + RefPoly->physicsBody->position;
        v2 = RefPoly->u * v2 + RefPoly->physicsBody->position;

        // Calculate reference face side normal in world space
        Vec2 sidePlaneNormal = (v2 - v1);
        sidePlaneNormal.normalize( );

        // Orthogonalize
        Vec2 refFaceNormal( sidePlaneNormal.y, -sidePlaneNormal.x );

        // ax + by = c
        // c is distance from origin
        real refC = dot( refFaceNormal, v1 );
        real negSide = -dot( sidePlaneNormal, v1 );
        real posSide =  dot( sidePlaneNormal, v2 );

        // Clip incident face to reference face side planes
        if(Clip( { -sidePlaneNormal.x, -sidePlaneNormal.y }, negSide, incidentFace ) < 2)
            return; // Due to floating point error, possible to not have required points

        if(Clip(  sidePlaneNormal, posSide, incidentFace ) < 2)
            return; // Due to floating point error, possible to not have required points

        // Flip
        m->normal = flip ? Vec2F { -refFaceNormal.x, -refFaceNormal.y } : refFaceNormal;

        // Keep points behind reference face
        uint32 cp = 0; // clipped points behind reference face
        real separation = dot( refFaceNormal, incidentFace[0] ) - refC;
        if(separation <= 0.0f)
        {
            m->contacts[cp] = incidentFace[0];
            m->penetration = -separation;
            ++cp;
        }
        else
            m->penetration = 0;

        separation = dot( refFaceNormal, incidentFace[1] ) - refC;
        if(separation <= 0.0f)
        {
            m->contacts[cp] = incidentFace[1];

            m->penetration += -separation;
            ++cp;

            // Average penetration
            m->penetration /= (real)cp;
        }

        m->contact_count = cp;
    }
}