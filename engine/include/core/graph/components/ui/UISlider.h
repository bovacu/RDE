//
// Created by borja on 10/20/22.
//

#ifndef RDE_UI_SLIDER_H
#define RDE_UI_SLIDER_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UITransform.h"

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

    FORWARD_DECLARE_CLASS(UI9Slice, UIImage)

    class UISlider : public UI {
        private:
            bool mouseDown = false;
            UI9Slice* backgroundBarSprite = nullptr;
            UI9Slice* fillBarSprite = nullptr;
            UIImage* handleSprite = nullptr;

            UITransform* backgroundBarTransform = nullptr;
            UITransform* fillBarTransform = nullptr;
            UITransform* handleTransform = nullptr;

            float percentageFilled = 0.f;

        public:
            UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config);
            ~UISlider() {  }

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void onUpdate(Delta _dt) override;

            void setFilledPercentage(float _percentage);
            float getFilledPercentage() const;

        private:
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //RDE_UI_SLIDER_H
