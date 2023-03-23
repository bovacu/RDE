//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIAnchoring.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/Node.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"

#define ANCHOR_BITS  0b0000001111111110
#define STRETCH_BITS 0b0011110000000000

namespace RDE {

    void UIAnchor::updateAnchor(UIAnchoring* _anchoring, Transform* _transform) {
        if (_transform->parentTransform == nullptr) {
            return;
        }

        auto _parentPosition = _transform->parentTransform->getModelMatrixPosition();
        auto _parentSize = _transform->parentTransform->node->getComponent<UIAnchoring>()->getSize();

		Vec2F _stretchAdjuster = { 1.f, 1.f };

		if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_NO_STRETCH) {
			anchorSize = Vec2F { 0.f, 0.f };
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_HORIZONTAL_STRETCH) {
			anchorSize.x = _parentSize.x;
			_stretchAdjuster.x = 0.0f;
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_VERTICAL_STRETCH) {
			anchorSize.y = _parentSize.y;
			_stretchAdjuster.y = 0.0f;
		} else if((anchor & STRETCH_BITS) == RDE_UI_STRETCH_FULL_STRETCH) {
			anchorSize = _parentSize;
			_stretchAdjuster.set(0.0f, 0.0f);
		}

		if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_MIDDLE) {
            anchorPosition = _parentPosition;
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT) {
			anchorPosition = { _parentPosition.x - _parentSize.x * 0.5f * _stretchAdjuster.x , _parentPosition.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT) {
			anchorPosition = { _parentPosition.x + _parentSize.x * 0.5f * _stretchAdjuster.x , _parentPosition.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_TOP) {
			anchorPosition = { _parentPosition.x, _parentPosition.y + _parentSize.y * 0.5f * _stretchAdjuster.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_BOTTOM) {
			anchorPosition = { _parentPosition.x, _parentPosition.y - _parentSize.y * 0.5f * _stretchAdjuster.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT_TOP) {
			anchorPosition = { _parentPosition.x - _parentSize.x * 0.5f * _stretchAdjuster.x, _parentPosition.y + _parentSize.y * 0.5f * _stretchAdjuster.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_LEFT_BOTTOM) {
			anchorPosition = { _parentPosition.x - _parentSize.x * 0.5f * _stretchAdjuster.x, _parentPosition.y - _parentSize.y * 0.5f * _stretchAdjuster.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT_TOP) {
			anchorPosition = { _parentPosition.x + _parentSize.x * 0.5f * _stretchAdjuster.x, _parentPosition.y + _parentSize.y * 0.5f * _stretchAdjuster.y };
		} else if((anchor & ANCHOR_BITS) == RDE_UI_ANCHOR_RIGHT_BOTTOM) {
			anchorPosition = { _parentPosition.x + _parentSize.x * 0.5f * _stretchAdjuster.x, _parentPosition.y - _parentSize.y * 0.5f * _stretchAdjuster.y };
        }
    }




	UIAnchoring::UIAnchoring(Node* _node, Scene* _scene, const UIAnchoringConfig& _config) :
		UIAnchoring(_node, &_scene->engine->manager, _scene->graph, _config) {
	
	}
	
	UIAnchoring::UIAnchoring(Node* _node, Scene* _scene, Canvas* _canvas, const UIAnchoringConfig& _config) :
		UIAnchoring(_node, &_scene->engine->manager, _canvas->graph, _config) {
	
	}
	
	UIAnchoring::UIAnchoring(Node* _node, Manager* _manager, Graph* _graph, const UIAnchoringConfig& _config) {
		transform = _node->getTransform();
		anchor.anchor = _config.anchor | _config.stretch;
		setSize(_config.size);
	}

	RDE_UI_ANCHOR_ UIAnchoring::getAnchor() const {
		return (RDE_UI_ANCHOR_)(anchor.anchor & ANCHOR_BITS);
    }

	void UIAnchoring::setAnchor(RDE_UI_ANCHOR_ _anchor) {
        anchor.anchor = _anchor | (anchor.anchor & STRETCH_BITS);
        setUIDirty();
    }

	RDE_UI_STRETCH_ UIAnchoring::getStretch() const {
		return (RDE_UI_STRETCH_)(anchor.anchor & STRETCH_BITS);
    }

	void UIAnchoring::setStretch(RDE_UI_STRETCH_ _stretch) {
        anchor.anchor = _stretch | (anchor.anchor & ANCHOR_BITS);
        setUIDirty();
    }

	void UIAnchoring::setSize(const Vec2F& _size) {
		size = _size;
		setUIDirty();
	}

	Vec2F UIAnchoring::getSize() {
		return size;
	}

	void UIAnchoring::update() {
		anchor.updateAnchor(this, transform);
		dirty = false;
	}

	void UIAnchoring::setUIDirty() {
        dirty = true;
		anchor.updateAnchor(this, transform);

        for(auto* _t : transform->children) {
			auto* _anchoring = _t->node->getComponent<UIAnchoring>();
			_anchoring->dirty = true;
			auto _lastAnchorPos = _anchoring->anchor.anchorPosition;
			auto _lastSize = _anchoring->anchor.anchorSize;

			_anchoring->anchor.updateAnchor(_anchoring, _t);
			auto _sizeDiff = _anchoring->anchor.anchorSize - _lastSize;
			auto _posDiff = _anchoring->anchor.anchorPosition - _lastAnchorPos;
			auto _position = Vec2F { _posDiff.x, _posDiff.y };

			_t->translateMatrixModelPosition(_position);
			_anchoring->setSize(_anchoring->getSize() + _sizeDiff);
        }
    }
}