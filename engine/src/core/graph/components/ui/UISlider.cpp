//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/Node.h"

namespace RDE {

    UISlider::UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config) {
        data.RenderizableInnerData.texture = _config.backgroundBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "panel") : _config.backgroundBarTexture;
        data.RenderizableInnerData.color = _config.backgroundBarColor;
        data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;

        node = _node;

        if(!_node->hasComponent<UIInteractable>()) {
            uiInteractable = _node->addComponent<UIInteractable>();
        }
                                                                                                           
        if(_config.stopFurtherClicks) {
            if(!node->hasComponent<CanvasEventStopper>()) {
                node->addComponent<CanvasEventStopper>();
            }
        } else {
            if (node->hasComponent<CanvasEventStopper>()) {
                node->removeComponent<CanvasEventStopper>();
            }
        }

		anchoring = node->getComponent<UIAnchoring>();	

        uiInteractable->onInnerClicking.bind<&UISlider::onMouseClicked>(this);
        uiInteractable->onInnerClickingReleased.bind<&UISlider::onMouseReleased>(this);

        backgroundBarSprite = node->addComponent<UIImage>(UIImageConfig {
            .size = _config.barSize,
            .texture = _config.backgroundBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal") :
					   _config.backgroundBarTexture,
            .color = _config.backgroundBarColor,
            .imageRenderingType = RDE_IMAGE_RENDERING_TYPE_NINE_SLICE
        });
        backgroundBarSprite->uiInteractable = uiInteractable;
        backgroundBarTransform = node->getTransform();
		backgroundAnchoring = node->getComponent<UIAnchoring>();
        data.RenderizableInnerData.shader = backgroundBarSprite->getShaderID();

        fillBarNode = _graph->createNode("Fill", node);
        fillBarSprite = fillBarNode->addComponent<UIImage>(UIImageConfig {
			.size = backgroundAnchoring->getSize(),
            .texture = _config.fillingBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal") :
                       _config.fillingBarTexture,
            .color = _config.fillingBarColor,
			.imageRenderingType = RDE_IMAGE_RENDERING_TYPE_NINE_SLICE
        });

        fillBarTransform = fillBarNode->getTransform();
		fillBarAnchoring = fillBarNode->getComponent<UIAnchoring>();
		fillBarAnchoring->setAnchor(RDE_UI_ANCHOR_LEFT);
		fillBarAnchoring->setStretch(RDE_UI_STRETCH_FULL_STRETCH);

        handleNode = _graph->createNode("Handle", node);
        handleSprite = handleNode->addComponent<UIImage>(UIImageConfig {
           .texture = _config.handleTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "handle") :
                      _config.handleTexture,
           .color = _config.handleColor
        });
        handleTransform = handleNode->getTransform();
		handleAnchoring = handleNode->getComponent<UIAnchoring>();
		handleAnchoring->setSize({handleAnchoring->getSize().x * 1.5f * _config.barSize.y / handleSprite->getSize().x, handleAnchoring->getSize().y * 1.5f * _config.barSize.y / handleSprite->getSize().y});
        handleTransform->setPosition(backgroundBarTransform->getPosition().x - (_config.barSize.x * 0.5f) + _config.barSize.x * _config.percentageFilled,
                                     handleTransform->getPosition().y);
		handleAnchoring->setAnchor(RDE_UI_ANCHOR_RIGHT);
		handleAnchoring->setStretch(RDE_UI_STRETCH_VERTICAL_STRETCH);

        setFilledPercentage(_config.percentageFilled);
    }

    ENABLED_DEFAULT_IMPL(UISlider)
    SIZE_METHODS_DEFAULT_IMPL(UISlider)
    INTERACTABLE_DEFAULT_IMPL(UISlider)
    FRAMEBUFFER_METHODS_DEFAULT_IMPL(UISlider)

	void UISlider::setFramebuffer(FramebufferID _framebuffer) {
		data.RenderizableInnerData.framebufferToRenderTo = _framebuffer;
		SAFE_POINTER(backgroundBarSprite, setFramebuffer(_framebuffer))
		SAFE_POINTER(fillBarSprite, setFramebuffer(_framebuffer))
		SAFE_POINTER(handleSprite, setFramebuffer(_framebuffer))
	}

    void UISlider::onUpdate(Delta _dt) {
        if(mouseDown) {
            auto _size = Vec2F { getSize().x, getSize().y };
            Vec2F _limits = { backgroundBarTransform->getWorldPosition().x - _size.x * 0.5f,
                                 backgroundBarTransform->getWorldPosition().x + _size.x * 0.5f };
            auto _posX = Util::Math::clampF(node->manager->inputManager.getMousePosWorldPos().x, _limits.v[0], _limits.v[1]);

            auto _distanceFromLowerPoint = _posX - _limits.v[0];
            setFilledPercentage(_distanceFromLowerPoint / getSize().x);
        }
		
		if(mouseDown && node->manager->inputManager.isMouseJustReleased(RDE_MOUSE_BUTTON_LEFT)) {
            mouseDown = false;
        }
    }

    void UISlider::setFilledPercentage(float _percentage) {
        percentageFilled = Util::Math::clampF(_percentage, 0.f, 1.f);

        auto _width = anchoring->getSize().x;
        auto _leftPos = node->getTransform()->getWorldPosition().x - _width * 0.5f;
        handleTransform->setWordlPosition({ _leftPos + percentageFilled * _width, handleTransform->getWorldPosition().y});

        fillBarSprite->setSize({ getSize().x * percentageFilled, getSize().y });
        fillBarTransform->setPosition({handleTransform->getPosition().x - fillBarSprite->getSize().x * 0.5f, fillBarTransform->getPosition().y});
    }

    float UISlider::getFilledPercentage() const {
        return percentageFilled;
    }

	void UISlider::onMouseClicked(RDE_MOUSE_BUTTON_ _mouseCode) {
        mouseDown = true;
    }

	void UISlider::onMouseReleased(RDE_MOUSE_BUTTON_ _mouseCode) {
        mouseDown = false;
    }


    void UISlider::setColor(const Color& _color) {
        SAFE_POINTER(backgroundBarSprite, setColor(_color))
        data.RenderizableInnerData.color = _color;
    }

    Color UISlider::getColor() const {
        return data.RenderizableInnerData.color;
    }


    void UISlider::setLayer(int _layer) {
        SAFE_POINTER(backgroundBarSprite, setLayer(_layer))
        SAFE_POINTER(fillBarSprite, setLayer(_layer))
        SAFE_POINTER(handleSprite, setLayer(_layer))
        data.RenderizableInnerData.layer = _layer;
    }

    int UISlider::getLayer() const {
        return data.RenderizableInnerData.layer;
    }


    void UISlider::setTexture(Texture* _texture) {
        SAFE_POINTER(backgroundBarSprite, setTexture(_texture))
        data.RenderizableInnerData.texture = _texture;
    }

    Texture* UISlider::getTexture() const {
        return data.RenderizableInnerData.texture;
    }


    void UISlider::setOriginOffset(const Vec2F& _offset) {
        SAFE_POINTER(backgroundBarSprite,setOriginOffset(_offset))
        SAFE_POINTER(fillBarSprite,setOriginOffset(_offset))
        SAFE_POINTER(handleSprite,setOriginOffset(_offset))
        data.originOffset = _offset;
    }

    Vec2F UISlider::getOriginOffset() const {
        return data.originOffset;
    }


    void UISlider::setShaderID(ShaderID _shaderID) {
        SAFE_POINTER(backgroundBarSprite, setShaderID(_shaderID))
        data.RenderizableInnerData.shader = _shaderID;
    }

    ShaderID UISlider::getShaderID() const {
        return data.RenderizableInnerData.shader;
    }


    Node* UISlider::getFillBarNode() {
        return fillBarNode;
    }

    Node* UISlider::getHandleNode() {
        return handleNode;
    }

}
