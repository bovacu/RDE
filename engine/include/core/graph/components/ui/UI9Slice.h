//
// Created by borja on 9/5/22.
//

#ifndef GDE_NINE_SLICE_SPRITE_H
#define GDE_NINE_SLICE_SPRITE_H

#include "core/util/Vec.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UIImage.h"

namespace GDE {

    struct UI9SliceConfig : CommonUIConfig {
        Vec2F size = { -1, -1 };
        Texture* texture = nullptr;
        Color color = Color::White;
    };

    FORWARD_DECLARE_CLASS(UIImage, Manager, Graph)
    /**
     * @brief Component that is based on the SpriteRenderer, but allows to render UI images with a different size than the original
     * without loosing quality. The setup of this NineSlice should be done via TexturePacker.
     */
    class UI9Slice : public UI {
        FRIEND_CLASS(UITransform, UIButton, UIPanel, UIInput, UISlider)

        private:
            UIImage* spriteRenderer = nullptr;
            UI9SliceConfig config;
            float dpi = -1;
            OpenGLVertex geometry[36];

        private:
            void calculateGeometry(glm::mat4& _transformMat, Transform& _transform, const IViewPort& _viewport);

        public:
            UI9Slice(Node* _node, Scene* _scene, Canvas* _canvas, Texture* _texture);
            UI9Slice(Node* _node, Manager* _manager, Graph* _graph, Texture* _texture);

            [[nodiscard]] Vec2F getSize() const override;
            void setSize(const Vec2F& _size);

            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) override;
            void drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const IViewPort& _viewport) override;

            void setInteractable(bool _enabled) override;
            bool isInteractable() override;

            ~UI9Slice() override {  }
    };

}

#endif //GDE_NINE_SLICE_SPRITE_H