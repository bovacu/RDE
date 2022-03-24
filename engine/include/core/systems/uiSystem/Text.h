// Created by borja on 15/1/22.

#ifndef ENGINE2_0_TEXT_H
#define ENGINE2_0_TEXT_H

#include "core/util/Util.h"
#include "FontManager.h"
#include "engine/include/core/graph/Components.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/shapes/Shape.h"

namespace engine {

    class Text {
        private:
            Font* font;
            std::string innerText;
            Vec2F size;
            float spaceBetweenChars;
            float spaceWidth;
            float enterHeight;
            int fontSize;
            Shape debugShape;

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            SpriteRenderer spriteRenderer;

        public:
            void init(Font* _font, const std::string& _text);

            void setText(const std::string& _text);
            void setFont(Font* _font);

            [[nodiscard]] Font* getFont() const;
            [[nodiscard]] const std::string& getText() const;
            [[nodiscard]] Vec2F getTextSize() const;

            void setFontSize(int _fontSize);
            int getFontSize();

            void setSpaceWidth(float _spaceWidth);
            [[nodiscard]] float getSpaceWidth() const;

            float getSpacesBetweenChars() const;
            void setSpacesBetweenChars(float _spaceBetweenChars);

            Shape& getDebugShape();
    };

}


#endif //ENGINE2_0_TEXT_H
