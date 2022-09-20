/*
    Copyright (c) 2013 Randy Gaul http://RandyGaul.net

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BODY_H
#define BODY_H

#include "IEMath.h"

namespace Physics {

    struct Shape;

// http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-the-core-engine/
    struct Body
    {
        Body(Shape *shape_, const GDE::Vec2F& _position);

        void ApplyForce( const GDE::Vec2F& f )
        {
            force += f;
        }

        void ApplyImpulse( const GDE::Vec2F& impulse, const GDE::Vec2F& contactVector )
        {
            velocity += im * impulse;
            angularVelocity += iI * Cross( contactVector, impulse );
        }

        void SetStatic( void )
        {
            I = 0.0f;
            iI = 0.0f;
            m = 0.0f;
            im = 0.0f;
        }

        void SetOrient( real radians );

        GDE::Vec2F position;
        GDE::Vec2F velocity;

        real angularVelocity;
        real torque;
        real orient; // radians

        GDE::Vec2F force;

        // Set by shape
        real I;  // moment of inertia
        real iI; // inverse inertia
        real m;  // mass
        real im; // inverse masee

        // http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table/
        real staticFriction;
        real dynamicFriction;
        real restitution;

        // Shape interface
        Shape *shape;

        // Store a color in RGB format
        real r, g, b;
    };

}

#endif // BODY_H