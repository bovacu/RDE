//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "core/graph/components/UI.h"

namespace GDE {

    class TextRenderer;
    class Font;
    class NineSliceSprite;
    class UIButton : public UI {
        public:
            TextRenderer* textRenderer;
            NineSliceSprite* nineSliceSprite;

            UIButton(const NodeID& _nodeID, Transform* _transform, Scene* _scene, Canvas* _canvas, Texture* _texture, Font* _font, const std::string& _text);

            /**
             * @brief Returns the NineSlice data. This is almost sure not to be needed by end-users.
             * @return NineSlice&
             */
            [[nodiscard]] NineSlice& getNineSlice() const;

            [[nodiscard]] Vec2F getSize() const override;

            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const override;

            ~UIButton() override {  }
    };

}

#endif //GDE_UI_BUTTON_H
