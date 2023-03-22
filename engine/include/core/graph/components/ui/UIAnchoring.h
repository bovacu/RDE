//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_ANCHORING_H
#define RDE_UI_ANCHORING_H

#include "core/graph/components/Transform.h"
#include "core/Enums.h"

namespace RDE {

	struct UIAnchoring;
    struct UIAnchor {
        friend class UIImage;

		uint16_t anchor = RDE_UI_ANCHOR_MIDDLE | RDE_UI_STRETCH_NO_STRETCH;
        Vec2F anchorPosition;
        Vec2F anchorSize;

       	void updateAnchor(UIAnchoring* _anchoring, Transform* _transform);
    };

	struct UIAnchoringConfig {
		RDE_UI_ANCHOR_ anchor = RDE_UI_ANCHOR_MIDDLE;
		RDE_UI_STRETCH_ stretch = RDE_UI_STRETCH_NO_STRETCH;
		Vec2F size { 64.f, 64.f };
	};

	class Manager;

    struct UIAnchoring {
        friend class Canvas;
        friend class Graph;
        friend class UIImage;
        friend class UIText;

        private:
            UIAnchor anchor;
            Vec2F size { 64.f, 64.f };
			Transform* transform;

		public:
			bool dirty = false;

        public:
			RDE_FUNC UIAnchoring(Node* _node, Scene* _scene, const UIAnchoringConfig& _config = {});
			RDE_FUNC UIAnchoring(Node* _node, Scene* _scene, Canvas* _canvas, const UIAnchoringConfig& _config = {});
			RDE_FUNC UIAnchoring(Node* _node, Manager* _manager, Graph* _graph, const UIAnchoringConfig& _config = {});

			RDE_FUNC_ND RDE_UI_ANCHOR_ getAnchor() const;
			RDE_FUNC void setAnchor(RDE_UI_ANCHOR_ _anchor);

			RDE_FUNC_ND RDE_UI_STRETCH_ getStretch() const;
			RDE_FUNC void setStretch(RDE_UI_STRETCH_ _stretch);

			RDE_FUNC_ND Vec2F getSize();
			RDE_FUNC void setSize(const Vec2F& _size);

			RDE_FUNC_ND bool isDirty();
			RDE_FUNC void update();

        private:
            void setUIDirty();
    };

} // RDE

#endif //RDE_UI_ANCHORING_H
