//
// Created by borja on 10/20/22.
//

#ifndef RDE_UI_SLIDER_H
#define RDE_UI_SLIDER_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/Batch.h"

namespace RDE {

    struct UISliderConfig : CommonUIConfig {
        Texture* backgroundBarTexture = nullptr;
        Color backgroundBarColor = Color::White;
        Vec2F barSize = { 256.f, 12.f };

        Texture* fillingBarTexture = nullptr;
        Color fillingBarColor = Color::Green;
        float percentageFilled = 0.25f;

        Texture* handleTexture = nullptr;
        Color handleColor = Color::Disabled;
    };

    class Scene;
    class Canvas;
    class UIImage;

    // TODO (RDE): Check why 2 or more sliders act weird
    class UISlider {
        private:
            bool mouseDown = false;
            UIImage* backgroundBarSprite = nullptr;
            
            UIImage* fillBarSprite = nullptr;
            Node* fillBarNode = nullptr;

            UIImage* handleSprite = nullptr;
            Node* handleNode = nullptr;

            UITransform* backgroundBarTransform = nullptr;
            UITransform* fillBarTransform = nullptr;
            UITransform* handleTransform = nullptr;

            float percentageFilled = 0.f;

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config = {});
            ~UISlider() {  }

            RENDERIZABLE_UI_BASIC_METHODS()

            void onUpdate(Delta _dt);

            void setFilledPercentage(float _percentage);
            float getFilledPercentage() const;

            Node* getFillBarNode();
            Node* getHandleNode();

        private:
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //RDE_UI_SLIDER_H
