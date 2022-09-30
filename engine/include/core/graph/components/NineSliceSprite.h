//
// Created by borja on 9/5/22.
//

#ifndef GDE_NINE_SLICE_SPRITE_H
#define GDE_NINE_SLICE_SPRITE_H

#include "core/util/Vec.h"
#include "core/graph/components/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(SpriteRenderer)
    /**
     * @brief Component that is based on the SpriteRenderer, but allows to render UI images with a different size than the original
     * without loosing quality. The setup of this NineSlice should be done via TexturePacker.
     */
    class NineSliceSprite : public UI {
        private:
            SpriteRenderer* spriteRenderer = nullptr;

        public:
            /**
             * @brief This is the size we want the UI to be rendered, may or may not match UI::getSize().
             */
            Vec2F nineSliceSize;

            NineSliceSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture);

            /**
             * @brief Returns the NineSlice data. This is almost sure not to be needed by end-users.
             * @return NineSlice&
             */
            [[nodiscard]] NineSlice& getNineSlice() const;

            [[nodiscard]] Vec2F getSize() const override { return {(float)nineSliceSize.x * transform->getScale().x, (float)nineSliceSize.y *
                                                                                                                     transform->getScale().y}; }

            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) const override;

            ~NineSliceSprite() override {  }
    };

}

#endif //GDE_NINE_SLICE_SPRITE_H
