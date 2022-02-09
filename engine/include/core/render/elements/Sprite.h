// Created by borja on 27/12/21.


#ifndef ENGINE2_0_SPRITE_H
#define ENGINE2_0_SPRITE_H


#include "core/util/Util.h"
#include "Texture.h"
#include "core/systems/Components.h"
#include "IRenderizable.h"

namespace engine {

    class Sprite : public IRenderizable {

        private:
            Transform transform;
            SpriteRenderer renderer;

        public:
            ShaderID getShaderID() override;
            void setShader(ShaderID _shaderID) override;

            Texture* getTexture() override;
            void setTexture(Texture* _texture) override;

        public:
            void init();
            void setPosition(const Vec2F& _position);
            void setScale(const Vec2F& _scale);
            void setRotation(float _rotation);
            void setColor(const Color& _color);

            [[nodiscard]] Vec2F getPosition() const;
            [[nodiscard]] Vec2F getScale() const;
            [[nodiscard]] float getRotation() const;
            [[nodiscard]] Color getColor() const;
            [[nodiscard]] Color getColor();

            void flipX(bool _flip);
            void flipY(bool _flip);

            [[nodiscard]] bool isXFlipped() const;
            [[nodiscard]] bool isYFlipped() const;

            void setLayer(int _layer);
            [[nodiscard]] int getLayer() const;
    };

}


#endif //ENGINE2_0_SPRITE_H
