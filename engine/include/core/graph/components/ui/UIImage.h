//
// Created by borja on 10/26/22.
//

#ifndef GDE_UI_IMAGE_H
#define GDE_UI_IMAGE_H


#include "core/graph/components/SpriteRenderer.h"

namespace GDE {

    class UIImage : public SpriteRenderer {
        public:
            UIImage(Node* _node, Scene* _scene, Canvas* _canvas, Texture* _texture);
            UIImage(Node* _node, Manager* _manager, Graph* _graph, Texture* _texture);
            ~UIImage() override {  }

        protected:
            virtual void calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const IViewPort& _viewport) override;
    };

}


#endif //GDE_UI_IMAGE_H
