//
// Created by borja on 10/26/22.
//

#ifndef GDE_UI_TEXT_RENDERER_H
#define GDE_UI_TEXT_RENDERER_H

#include "core/graph/components/TextRenderer.h"

namespace GDE {

    class UIText : private TextRenderer {
        FRIEND_CLASS(Canvas, UIButton, UICheckbox, UIInput)

        public:
            UIText(Node* _node, Scene* _scene, Canvas* _canvas, const std::string& _text, Font* _font = nullptr);
            UIText(Node* _node, Manager* _manager, Graph* _graph, const std::string& _text, Font* _font = nullptr);
            ~UIText() override {  }

            virtual void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) override;
            virtual void drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const IViewPort& _viewport) override;
    };

} // GDE

#endif //GDE_UI_TEXT_RENDERER_H
