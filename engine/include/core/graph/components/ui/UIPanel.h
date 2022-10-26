//
// Created by borja on 10/20/22.
//

#ifndef GDE_UI_PANEL_H
#define GDE_UI_PANEL_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/NineSliceSprite.h"

namespace GDE {

    struct UIPanelConfig : CommonUIConfig {
        Texture* texture = nullptr;
        Vec2F size { 128, 128 };
        Color color = Color::NO_COLOR;
    };

    class UIPanel : public UI {
        private:
            UIPanelConfig config;

        public:
            NineSliceSprite* nineSliceSprite = nullptr;

            UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config);

            [[nodiscard]] Vec2F getSize() const override;

            UIPanelConfig getConfig();
            void setConfig(Manager* _manager, const UIPanelConfig& _config);

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            ~UIPanel() override {  }
    };

}


#endif //GDE_UI_PANEL_H
