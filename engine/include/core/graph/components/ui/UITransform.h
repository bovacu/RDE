//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_TRANSFORM_H
#define RDE_UI_TRANSFORM_H

#include "core/graph/components/Transform.h"
#include "core/Enums.h"

namespace RDE {

    struct UITransform;

    struct UIAnchor {
        friend class UITransform;
        friend class UIImage;

		uint16_t anchor = RDE_UI_ANCHOR_MIDDLE | RDE_UI_STRETCH_NO_STRETCH;
        Vec2F anchorPosition;
        Vec2F anchorSize;

        private:
            void updateAnchor(UITransform* _transform);
    };

    struct UITransform : public Transform {
        friend class Canvas;
        friend class Graph;
        friend class UIImage;
        friend class UIText;

        private:
            UIAnchor anchor;
            Vec2F size { 64.f, 64.f };
            bool uiDirty = false;

        public:
            explicit UITransform(Graph* _graph);

            void clearDirty() override;

			[[nodiscard]] RDE_UI_ANCHOR_ getAnchor() const;
			void setAnchor(RDE_UI_ANCHOR_ _anchor);

			[[nodiscard]] RDE_UI_STRETCH_ getStretch() const;
			void setStretch(RDE_UI_STRETCH_ _stretch);

            Vec2F getSize();
            void setSize(const Vec2F& _size);

            std::tuple<glm::mat4, bool> localToWorld() override;

        private:
            void setUIDirty();
    };

} // RDE

#endif //RDE_UI_TRANSFORM_H
