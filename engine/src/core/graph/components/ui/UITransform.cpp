//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UI9Slice.h"

#define ANCHOR_BITS  0b0000001111111110
#define STRETCH_BITS 0b0011110000000000

namespace GDE {

    void UIAnchor::updateAnchor(UITransform* _transform) {
        auto _parentPosition = _transform->parentTransform->getModelMatrixPosition();
        auto _parentSize = ((UITransform*)_transform->parentTransform->node->getTransform())->getSize();

        if((anchor & ANCHOR_BITS) == Anchor::MIDDLE) {
            anchorPosition = _parentPosition;
        } else if((anchor & ANCHOR_BITS) == Anchor::LEFT) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f) , _parentPosition.y };
        } else if((anchor & ANCHOR_BITS) == Anchor::RIGHT) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f) , _parentPosition.y };
        } else if((anchor & ANCHOR_BITS) == Anchor::TOP) {
            anchorPosition = { _parentPosition.x, (_parentPosition.y + _parentSize.y * 0.5f) };
        } else if((anchor & ANCHOR_BITS) == Anchor::BOTTOM) {
            anchorPosition = { _parentPosition.x, (_parentPosition.y - _parentSize.y * 0.5f) };
        } else if((anchor & ANCHOR_BITS) == Anchor::LEFT_TOP) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
        } else if((anchor & ANCHOR_BITS) == Anchor::LEFT_BOTTOM) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
        } else if((anchor & ANCHOR_BITS) == Anchor::RIGHT_TOP) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
        } else if((anchor & ANCHOR_BITS) == Anchor::RIGHT_BOTTOM) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
        }


        if((anchor & STRETCH_BITS) == Stretch::NO_STRETCH) {
            anchorSize = Vec2F { 0.f, 0.f };
        } else if((anchor & STRETCH_BITS) == Stretch::HORIZONTAL_STRETCH) {
            anchorSize.x = _parentSize.x;
        } else if((anchor & STRETCH_BITS) == Stretch::VERTICAL_STRETCH) {
            anchorSize.y = _parentSize.y;
        } else if((anchor & STRETCH_BITS) == Stretch::FULL_STRETCH) {
            anchorSize = _parentSize;
        }
    }




    UITransform::UITransform() {
        anchor.anchor = Anchor::MIDDLE | Stretch::NO_STRETCH;
    }

    Anchor UITransform::getAnchor() const {
        return (Anchor)(anchor.anchor & ANCHOR_BITS);
    }

    void UITransform::setAnchor(Anchor _anchor) {
        anchor.anchor = _anchor | (anchor.anchor & STRETCH_BITS);
        anchor.updateAnchor(this);
        setUIDirty();
    }

    Stretch UITransform::getStretch() const {
        return (Stretch)(anchor.anchor & STRETCH_BITS);
    }

    void UITransform::setStretch(Stretch _stretch) {
        anchor.anchor = _stretch | (anchor.anchor & ANCHOR_BITS);
        anchor.updateAnchor(this);
        setUIDirty();
    }

    void UITransform::setUIDirty() {
        uiDirty = true;

        for(auto* _t : children) {
            auto _uiTransform = (UITransform*)_t;
            _uiTransform->uiDirty = true;
            auto _lastAnchorPos = _uiTransform->anchor.anchorPosition;
            auto _lastSize = _uiTransform->anchor.anchorSize;

            _uiTransform->anchor.updateAnchor(_uiTransform);
            _uiTransform->translateMatrixModelPosition({_uiTransform->anchor.anchorPosition.x - _lastAnchorPos.x,
                                                        _uiTransform->anchor.anchorPosition.y - _lastAnchorPos.y});

            auto _sizeDiff = _uiTransform->anchor.anchorSize - _lastSize;
            if(_sizeDiff != 0) {
                _uiTransform->setSize(_uiTransform->getSize() + _sizeDiff);
                if(((_uiTransform->anchor.anchor & Anchor::RIGHT) == Anchor::RIGHT) || ((_uiTransform->anchor.anchor & Anchor::RIGHT_BOTTOM) == Anchor::RIGHT_BOTTOM) ||
                        ((_uiTransform->anchor.anchor & Anchor::RIGHT_TOP) == Anchor::RIGHT_TOP)) {
                    _sizeDiff.x = -_sizeDiff.x;
                }

                if(((_uiTransform->anchor.anchor & Anchor::TOP) == Anchor::TOP) || ((_uiTransform->anchor.anchor & Anchor::RIGHT_TOP) == Anchor::RIGHT_TOP) ||
                   ((_uiTransform->anchor.anchor & Anchor::LEFT_TOP) == Anchor::LEFT_TOP)) {
                    _sizeDiff.y = -_sizeDiff.y;
                }
                // It is half the size because when a change in size happens, it is applied to both sides
                _uiTransform->translateMatrixModelPosition(_sizeDiff * 0.5f);
            }
        }
    }

    std::tuple<glm::mat4, bool> UITransform::localToWorld() {
        auto [_mat, _dirty] = Transform::localToWorld();
        if(_dirty) {
            anchor.updateAnchor(this);
        }
        return { _mat, (_dirty || uiDirty) };
    }

    Vec2F UITransform::getSize() {
        return size;
    }

    void UITransform::setSize(const Vec2F& _size) {
        size = _size;
        setUIDirty();
    }
}