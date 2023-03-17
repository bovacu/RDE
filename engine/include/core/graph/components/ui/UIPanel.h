//
// Created by borja on 10/20/22.
//

#ifndef RDE_UI_PANEL_H
#define RDE_UI_PANEL_H

#include "core/graph/components/ui/UI.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/ui/UIAnchoring.h"

namespace RDE {

    struct UIPanelConfig : CommonUIConfig {
        Texture* texture = nullptr;
        Vec2F size { -1.f, -1.f };
        Color color = { 22, 29, 34, 255 };
    };

    class UIImage;

    class UIPanel {

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            UIImage* uiImage = nullptr;

            UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config = {});

            RENDERIZABLE_UI_BASIC_METHODS()

            ~UIPanel() {  }
    };

}


#endif //RDE_UI_PANEL_H
