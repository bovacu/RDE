//
// Created by borja on 10/20/22.
//

#ifndef GDE_UI_SLIDER_H
#define GDE_UI_SLIDER_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/NineSliceSprite.h"

namespace GDE {

    struct UISliderConfig : CommonUIConfig {
        Texture* backgroundBarTexture = nullptr;
        Color backgroundBarColor = Color::White;
        Vec2F barSize = { 256, 12 };

        Texture* fillingBarTexture = nullptr;
        Color fillingBarColor = Color::Green;
        float percentageFilled = 0.25f;

        Texture* handleTexture = nullptr;
        Color handleColor = Color::Disabled;
    };

    class UISlider : public UI {
        private:
            UISliderConfig config;
            bool mouseDown = false;
            NineSliceSprite* backgroundBarSprite = nullptr;
            NineSliceSprite* fillBarSprite = nullptr;
            UIImage* handleSprite = nullptr;

            Transform* backgroundBarTransform = nullptr;
            Transform* fillBarTransform = nullptr;
            Transform* handleTransform = nullptr;

        public:
            UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config);
            ~UISlider() {  }

            [[nodiscard]] Vec2F getSize() const override;

            UISliderConfig getConfig();
            void setConfig(Manager* _manager, const UISliderConfig& _config);

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            void onUpdate(Delta _dt) override;

            void setFilledPercentage(float _percentage);
            float getFilledPercentage() const;

        private:
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //GDE_UI_SLIDER_H
