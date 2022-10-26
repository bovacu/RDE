//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI.h"

namespace GDE {

    UITransform::UITransform() {

    }

    Vec2F UITransform::getPivot() {
        return pivot;
    }

    void UITransform::setPivot(const Vec2F& _pivot) {
        if(pivot == _pivot) return;
        pivot = _pivot;
    }

    UIAnchor UITransform::getAnchor() {
        return anchor;
    }

    void UITransform::setAnchor(UIAnchor _anchor) {
        if(_anchor == anchor) return;
        anchor = _anchor;
        setUIDirty();
    }

    void UITransform::setUIDirty() {
        uiDirty = true;

        for(auto* _t : children) {
            auto _uiTransform = (UITransform*)_t;
            _uiTransform->uiDirty = true;
        }
    }

    std::tuple<glm::mat4, bool> UITransform::localToWorld() {
        auto [_mat, _dirty] = Transform::localToWorld();
        return { _mat, (_dirty || uiDirty) };
    }

}