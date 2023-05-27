//
// Created by borja on 10/20/22.
//

#ifndef RDE_UI_SLIDER_H
#define RDE_UI_SLIDER_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/Batch.h"
#include "core/util/Delta.h"

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

    class UISlider {
        private:
            bool mouseDown = false;
            UIImage* backgroundBarSprite = nullptr;
            
            UIImage* fillBarSprite = nullptr;
            Node* fillBarNode = nullptr;

            UIImage* handleSprite = nullptr;
            Node* handleNode = nullptr;

            Transform* backgroundBarTransform = nullptr;
			UIAnchoring* backgroundAnchoring = nullptr;

			Transform* fillBarTransform = nullptr;
			UIAnchoring* fillBarAnchoring = nullptr;

			Transform* handleTransform = nullptr;
			UIAnchoring* handleAnchoring = nullptr;

            float percentageFilled = 0.f;

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
			RDE_FUNC UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config = {});
			RDE_FUNC ~UISlider() {  }

            RENDERIZABLE_UI_BASIC_METHODS()

			RDE_FUNC void onUpdate(Delta _dt);

			RDE_FUNC void setFilledPercentage(float _percentage);
			RDE_FUNC_ND float getFilledPercentage() const;

			RDE_FUNC_ND Node* getFillBarNode();
			RDE_FUNC_ND Node* getHandleNode();

        private:
			void onMouseClicked(RDE_MOUSE_BUTTON_ _mouseCode);
			void onMouseReleased(RDE_MOUSE_BUTTON_ _mouseCode);
    };

}

#endif //RDE_UI_SLIDER_H
