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
			UIAnchoring(Node* _node, Scene* _scene, const UIAnchoringConfig& _config = {});
			UIAnchoring(Node* _node, Scene* _scene, Canvas* _canvas, const UIAnchoringConfig& _config = {});
			UIAnchoring(Node* _node, Manager* _manager, Graph* _graph, const UIAnchoringConfig& _config = {});

			[[nodiscard]] RDE_UI_ANCHOR_ getAnchor() const;
			void setAnchor(RDE_UI_ANCHOR_ _anchor);

			[[nodiscard]] RDE_UI_STRETCH_ getStretch() const;
			void setStretch(RDE_UI_STRETCH_ _stretch);

            Vec2F getSize();
            void setSize(const Vec2F& _size);

			bool isDirty();
			void update();

        private:
            void setUIDirty();
    };

} // RDE

#endif //RDE_UI_ANCHORING_H
