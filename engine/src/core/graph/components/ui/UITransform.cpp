//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UI9Slice.h"

namespace GDE {

    void UIAnchor::updateAnchor(UITransform* _transform) {
        auto _position = _transform->getPosition();
        auto _parentPosition = _transform->parentTransform->getPosition();
        auto _parentSize = _transform->parentTransform->node->getComponent<UIInteractable>()->sizeOfInteraction;

        switch (anchor) {
            case MIDDLE: {
                anchorPosition = _parentPosition;
                anchorDistances = { _parentPosition.x - _position.x,
                                    _parentPosition.y - _position.y
                };
                break;
            }
            case LEFT: {
                anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f) , _parentPosition.y };
                anchorDistances = { (_parentPosition.x - _parentSize.x * 0.5f) - _position.x,
                                    _parentPosition.y - _position.y
                };
                break;
            }
            case RIGHT: {
                anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f) , _parentPosition.y };
                anchorDistances = { (_parentPosition.x + _parentSize.x * 0.5f) - _position.x,
                                    _parentPosition.y - _position.y
                };
                break;
            }
            case TOP: {
                anchorPosition = { _parentPosition.x, (_parentPosition.y + _parentSize.y * 0.5f) };
                anchorDistances = { _parentPosition.x - _position.x,
                                    (_parentPosition.y + _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case BOTTOM: {
                anchorPosition = { _parentPosition.x, (_parentPosition.y - _parentSize.y * 0.5f) };
                anchorDistances = { _parentPosition.x - _position.x,
                                    (_parentPosition.y - _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case LEFT_BOTTOM: {
                anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
                anchorDistances = { (_parentPosition.x - _parentSize.x * 0.5f) - _position.x,
                                    (_parentPosition.y - _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case LEFT_TOP: {
                anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
                anchorDistances = { (_parentPosition.x - _parentSize.x * 0.5f) - _position.x,
                                    (_parentPosition.y + _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case RIGHT_BOTTOM: {
                anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
                anchorDistances = { (_parentPosition.x + _parentSize.x * 0.5f) - _position.x,
                                    (_parentPosition.y - _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case RIGHT_TOP: {
                anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
                anchorDistances = { (_parentPosition.x + _parentSize.x * 0.5f) - _position.x,
                                    (_parentPosition.y + _parentSize.y * 0.5f) - _position.y
                };
                break;
            }
            case NO_STRETCH:
                break;
            case VERTICAL_STRETCH:
                break;
            case HORIZONTAL_STRETCH:
                break;
            case FULL_STRETCH:
                break;
        }
    }




    UITransform::UITransform() {

    }

    Vec2F UITransform::getPivot() {
        return pivot;
    }

    void UITransform::setPivot(const Vec2F& _pivot) {
        if(pivot == _pivot) return;
        pivot = _pivot;
    }

    Anchor UITransform::getAnchor() {
        return anchor.anchor;
    }

    void UITransform::setAnchor(Anchor _anchor) {
        if(_anchor == anchor.anchor) return;
        anchor.anchor = _anchor;
        anchor.updateAnchor(this);
        setUIDirty();
    }

    void UITransform::setUIDirty() {
        uiDirty = true;

        for(auto* _t : children) {
            auto _uiTransform = (UITransform*)_t;
            _uiTransform->uiDirty = true;
            auto _lastAnchorPos = _uiTransform->anchor.anchorPosition;
            _uiTransform->anchor.updateAnchor(_uiTransform);
            _uiTransform->translate(_uiTransform->anchor.anchorPosition.x - _lastAnchorPos.x, 0);
        }
    }

    std::tuple<glm::mat4, bool> UITransform::localToWorld() {
        auto [_mat, _dirty] = Transform::localToWorld();
        return { _mat, (_dirty || uiDirty) };
    }
}