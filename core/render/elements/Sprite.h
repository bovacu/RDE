// Created by borja on 27/12/21.


#ifndef ENGINE2_0_SPRITE_H
#define ENGINE2_0_SPRITE_H


#include "core/util/Util.h"
#include "Texture.h"
#include "core/systems/Components.h"
#include "core/systems/animationSystem/AnimationSystem.h"

namespace engine {

    class Sprite {

        private:
            Transform transform;
            SpriteRenderer renderer;
            AnimationSystem* animation;

        public:
            void setPosition(const Vec2F& _position);
            void setScale(const Vec2F& _scale);
            void setRotation(float _rotation);
            void setColor(const Color& _color);
            void setTexture(Texture* _texture);

            Vec2F getPosition() const;
            Vec2F getScale() const;
            float getRotation() const;
            Color getColor() const;
            Color getColor();
            Texture& getTexture() const;

            void flipX(bool _flip);
            void flipY(bool _flip);

            bool isXFlipped() const;
            bool isYFlipped() const;

            void addAnimation(AnimationSystem* _animation);

            void update(float _dt);
    };

}


#endif //ENGINE2_0_SPRITE_H
