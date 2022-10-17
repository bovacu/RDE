//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "core/graph/components/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(TextRenderer, Font, NineSliceSprite)

    struct UIButtonConfig {
        std::string text = "Button Text";
        Texture* texture = nullptr;
        Font* font = nullptr;
        Vec2F size = { 128, 32 };
        Color color = Color::White;
    };

    class UIButton : public UI {
        public:
            TextRenderer* textRenderer;
            NineSliceSprite* nineSliceSprite;

            UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);

            /**
             * @brief Returns the NineSlice data. This is almost sure not to be needed by end-users.
             * @return NineSlice&
             */
            [[nodiscard]] NineSlice& getNineSlice() const;

            [[nodiscard]] Vec2F getSize() const override;

            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) override;

            ~UIButton() override {  }
    };

}

#endif //GDE_UI_BUTTON_H
