// Created by borja on 26/12/21.


#ifndef ENGINE2_0_RENDERER_H
#define ENGINE2_0_RENDERER_H

#include "core/util/Util.h"
#include "core/render/elements/SpriteBatch.h"

namespace engine {

    class Renderer {

        private:
            static SpriteBatch batch;

        public:
            static void init(const Vec2I& _screenSize);
            static void beginDraw(const Color& _color);
            static void draw(Texture* _texture, const Vec2I& _position, const Vec2I& _size, const Color& _tintColor = Color::White);
            static void endDraw();
    };
}


#endif //ENGINE2_0_RENDERER_H
