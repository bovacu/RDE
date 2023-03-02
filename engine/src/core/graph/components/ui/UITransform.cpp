//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/Node.h"

#define ANCHOR_BITS  0b0000001111111110
#define STRETCH_BITS 0b0011110000000000

namespace RDE {

    void UIAnchor::updateAnchor(UITransform* _transform) {
        if (_transform->parentTransform == nullptr) {
            return;
        }

        auto _parentPosition = _transform->parentTransform->getModelMatrixPosition();
        auto _parentSize = ((UITransform*)_transform->parentTransform->node->getTransform())->getSize();

		if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_MIDDLE) {
            anchorPosition = _parentPosition;
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f) , _parentPosition.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f) , _parentPosition.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_TOP) {
            anchorPosition = { _parentPosition.x, (_parentPosition.y + _parentSize.y * 0.5f) };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_BOTTOM) {
            anchorPosition = { _parentPosition.x, (_parentPosition.y - _parentSize.y * 0.5f) };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT_TOP) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT_BOTTOM) {
            anchorPosition = { (_parentPosition.x - _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT_TOP) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y + _parentSize.y * 0.5f) };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT_BOTTOM) {
            anchorPosition = { (_parentPosition.x + _parentSize.x * 0.5f), (_parentPosition.y - _parentSize.y * 0.5f) };
        }


		if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_NO_STRETCH) {
            anchorSize = Vec2F { 0.f, 0.f };
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_HORIZONTAL_STRETCH) {
            anchorSize.x = _parentSize.x;
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_VERTICAL_STRETCH) {
            anchorSize.y = _parentSize.y;
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_FULL_STRETCH) {
            anchorSize = _parentSize;
        }
    }




    UITransform::UITransform(Graph* _graph) : Transform(_graph) {
		anchor.anchor = RDE_UI_ANCHOR_MIDDLE | RDE_UI_STRETCH_NO_STRETCH;
    }

	RDE_UI_ANCHOR_ UITransform::getAnchor() const {
		return (RDE_UI_ANCHOR_)(anchor.anchor & ANCHOR_BITS);
    }

	void UITransform::setAnchor(RDE_UI_ANCHOR_ _anchor) {
        anchor.anchor = _anchor | (anchor.anchor & STRETCH_BITS);
        anchor.updateAnchor(this);
        setUIDirty();
    }

	RDE_UI_STRETCH_ UITransform::getStretch() const {
		return (RDE_UI_STRETCH_)(anchor.anchor & STRETCH_BITS);
    }

	void UITransform::setStretch(RDE_UI_STRETCH_ _stretch) {
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
			if (_sizeDiff != 0) {
				_uiTransform->setSize(_uiTransform->getSize() + _sizeDiff);
				if (((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_RIGHT) == RDE_UI_ANCHOR_RIGHT) || ((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_RIGHT_BOTTOM) == RDE_UI_ANCHOR_RIGHT_BOTTOM) ||
					((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_RIGHT_TOP) == RDE_UI_ANCHOR_RIGHT_TOP)) {
					_sizeDiff.x = -_sizeDiff.x;
				}

				if (((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_TOP) == RDE_UI_ANCHOR_TOP) || ((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_RIGHT_TOP) == RDE_UI_ANCHOR_RIGHT_TOP) ||
					((_uiTransform->anchor.anchor & RDE_UI_ANCHOR_LEFT_TOP) == RDE_UI_ANCHOR_LEFT_TOP)) {
					_sizeDiff.y = -_sizeDiff.y;
				}

				// It is half the size because when a change in size happens, it is applied to both sides
				_uiTransform->translateMatrixModelPosition(_sizeDiff * 0.5f);
			}
        }
    }

	void UITransform::update() {
		Transform::update();
		uiDirty = false;
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