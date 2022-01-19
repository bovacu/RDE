// Created by borja on 15/1/22.

#ifndef ENGINE2_0_TEXT_H
#define ENGINE2_0_TEXT_H

#include "core/util/Util.h"
#include "FontManager.h"
#include "core/systems/Components.h"

namespace engine {

    class Text {
        private:
            FontAtlas* font;
            Transform transform;
            std::string innerText;
            Vec2F size;
            float spaceBetweenChars;
            float spaceWidth;
            float enterHeight;
            int fontSize;
            Color textColor = Color::White;

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            void init(FontAtlas* _font, const std::string& _text);

            void setText(const std::string& _text);
            void setFont(FontAtlas* _font);

            FontAtlas* getFont();
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
    };

}


#endif //ENGINE2_0_TEXT_H
