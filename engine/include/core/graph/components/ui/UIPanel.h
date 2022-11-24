//
// Created by borja on 10/20/22.
//

#ifndef RDE_UI_PANEL_H
#define RDE_UI_PANEL_H

#include "core/graph/components/ui/UI.h"

namespace RDE {

    struct UIPanelConfig : CommonUIConfig {
        Texture* texture = nullptr;
        Vec2F size { -1.f, -1.f };
        Color color = { 22, 29, 34, 255 };
    };

    FORWARD_DECLARE_CLASS(UI9Slice)

    class UIPanel : public UI {
        public:
            UI9Slice* nineSliceSprite = nullptr;

            UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config);

            [[nodiscard]] Vec2F getSize() const override;

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            ~UIPanel() override {  }
    };

}


#endif //RDE_UI_PANEL_H
