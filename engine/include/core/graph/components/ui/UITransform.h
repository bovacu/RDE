//
// Created by borja on 10/26/22.
//

#ifndef GDE_UI_TRANSFORM_H
#define GDE_UI_TRANSFORM_H

#include "core/graph/components/Transform.h"

namespace GDE {

    enum UIAnchor {
        MIDDLE              = 1 << 1,
        LEFT                = 1 << 2,
        RIGHT               = 1 << 3,
        TOP                 = 1 << 4,
        BOTTOM              = 1 << 5,
        LEFT_BOTTOM         = 1 << 6,
        LEFT_TOP            = 1 << 7,
        RIGHT_BOTTOM        = 1 << 8,
        RIGHT_TOP           = 1 << 9,
        NO_STRETCH          = 1 << 10,
        VERTICAL_STRETCH    = 1 << 11,
        HORIZONTAL_STRETCH  = 1 << 12,
        FULL_STRETCH        = 1 << 13
    };

    struct UITransform : public Transform {
        FRIEND_CLASS(Graph)

        private:
            Vec2F pivot { 0.5f, 0.5f };
            UIAnchor anchor;
            bool uiDirty = false;

        public:
            explicit UITransform();

            Vec2F getPivot();
            void setPivot(const Vec2F& _pivot);

            UIAnchor getAnchor();
            void setAnchor(UIAnchor _anchor);

            std::tuple<glm::mat4, bool> localToWorld() override;

        private:
            void setUIDirty();
    };

} // GDE

#endif //GDE_UI_TRANSFORM_H
