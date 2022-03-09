// Created by borja on 15/1/22.

#ifndef ENGINE2_0_TEXT_H
#define ENGINE2_0_TEXT_H

#include "core/util/Util.h"
#include "FontManager.h"
#include "engine/include/core/scene/Components.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/shapes/Shape.h"

namespace engine {

    class Text : public IRenderizable {
        private:
            Font* font;
            std::string innerText;
            Transform transform;
            Vec2F size;
            float spaceBetweenChars;
            float spaceWidth;
            float enterHeight;
            int fontSize;
            ShaderID shaderID;
            Color textColor = Color::White;
            Shape debugShape;

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            ShaderID getShaderID() override;
            void setShader(ShaderID _shaderID) override;

            Texture* getTexture() override;
            void setTexture(Texture* _texture) override;

        public:
            void init(Font* _font, const std::string& _text);

            void setText(const std::string& _text);
            void setFont(Font* _font);

            Font* getFont();
            std::string& getText();
            Vec2F getTextSize();

            void setPosition(const Vec2F& _position);
            void setRotation(float _rotation);
            void setScale(const Vec2F& _scale);

            Vec2F getPosition();
            float getRotation();
            Vec2F getScale();

            void setFontSize(int _fontSize);
            int getFontSize();

            void setSpaceWidth(float _spaceWidth);
            [[nodiscard]] float getSpaceWidth() const;

            float getSpacesBetweenChars() const;
            void setSpacesBetweenChars(float _spaceBetweenChars);

            void setTextColor(const Color& _color);
            Color& getColor();

            Shape& getDebugShape();
    };

}


#endif //ENGINE2_0_TEXT_H
