// Created by borja on 11/3/22.

#include "core/graph/Components.h"
#include "core/util/Functions.h"
#include "core/systems/uiSystem/Canvas.h"
#include "engine/include/core/systems/eventSystem/KeyEvent.h"
#include "engine/include/core/systems/eventSystem/JoystickEvent.h"
#include "engine/include/core/systems/eventSystem/MobileEvent.h"

namespace GDE {

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 _scale = {localScale.x, localScale.y, 1};

        localModelMatrix = glm::translate(glm::mat4(1.0f), {localPosition.x, localPosition.y, 0.0f}) * _rot * glm::scale(glm::mat4(1.0f), _scale);
        return localModelMatrix;
    }

    void Transform::update(Graph* _graph) {
        if (parent != NODE_ID_NULL) {
            auto* _parentT = _graph->getComponent<Transform>(parent);
            if(!dirty && !_parentT->dirty) return;

            modelMatrix = _parentT->modelMatrix * getLocalModelMatrix();
        } else {
            modelMatrix = getLocalModelMatrix();
        }

        dirty = false;
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        localPosition = glm::vec3 {_x, _y, 0.0f};
        dirty = true;
    }

    Vec2F Transform::getPositionLocal() const {
        return {localPosition.x, localPosition.y};
    }

    void Transform::setRotation(float _rotation) {
        localRotation = _rotation;
        dirty = true;
    }

    float Transform::getRotationLocal() const {
        return localRotation;
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        localScale = {_x, _y, 1.0f};
        dirty = true;
    }

    Vec2F Transform::getScaleLocal() const {
        return {localScale.x, localScale.y};
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        localPosition.x += _x;
        localPosition.y += _y;
        dirty = true;
    }

    void Transform::rotate(float _amount) {
        localRotation += _amount;
        dirty = true;
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        localScale.x += _x;
        localScale.y += _y;
        dirty = true;
    }

    Vec2F Transform::getPositionWorld() const {
        float _x = modelMatrix[3][0], _y =  modelMatrix[3][1];
        return {_x, _y};
    }

    float Transform::getRotationWorld() const {
        return -1;
    }

    Vec2F Transform::getScaleLWorld() const {
        return { modelMatrix[0][0], modelMatrix[1][1] };
    }

    void Transform::setConstant(bool _constant) {
        constant = _constant;
    }

    bool Transform::isConstant() const {
        return constant;
    }

    Transform::Transform(const NodeID& _nodeId) {

    }


    /// -------------------------------------------

    TextRenderer::TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        shaderID = _scene->engine->manager.shaderManager.getShader("basicText");
        texture = &font->getTexture();
    }

    TextRenderer::TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font) {
        font = _font;
        shaderID = _scene->engine->manager.shaderManager.getShader("basicText");
        texture = &font->getTexture();
    }

    void TextRenderer::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void TextRenderer::setFont(Font* _font) {
        font = _font;
    }

    Font* TextRenderer::getFont() const {
        return font;
    }

    const std::string& TextRenderer::getText() const {
        return innerText;
    }

    void TextRenderer::recalcTextDimensions(const std::string& _text) {
        size.y = -1;
        size.x = 0;

        for(auto _c : _text) {
            auto _char = font->getChars()[_c];

            if(_c == ' ') {
                size.x += _char.advance.x / 2.f;
                continue;
            }

            if(_c == '\n') {
                continue;
            }

            size.x += _char.advance.x / 2.f;
            size.y = std::max(size.y, _char.bitmapSize.y - _char.bearing.y);
        }

//        debugShape.makeSquare(getPositionLocal(), size);
    }

    Vec2F TextRenderer::getTextSize() const {
        return size;
    }

    void TextRenderer::setFontSize(int _fontSize) {
        LOG_W("Set font size not working!!")
//        fontSize = _fontSize;
//        recalcTextDimensions(innerText);
//        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
    }

    int TextRenderer::getFontSize() const {
        return fontSize;
    }

    void TextRenderer::setSpaceWidth(float _spaceWidth) {
        spaceWidth = _spaceWidth;
        recalcTextDimensions(innerText);
    }

    float TextRenderer::getSpaceWidth() const {
        return spaceWidth;
    }

    float TextRenderer::getSpacesBetweenChars() const {
        return spaceBetweenChars;
    }

    void TextRenderer::setSpacesBetweenChars(float _spaceBetweenChars) {
        spaceBetweenChars = _spaceBetweenChars;
        recalcTextDimensions(innerText);
    }

//    Shape& TextRenderer::getDebugShape() {
//        return debugShape;
//    }


    ///-------------------------------- BODY ---------------------------


    Body::Body(const NodeID& _nodeId, const BodyConfig& _config, Transform* _transform) {
        bodyConfig = _config;

        b2dConfig.bodyDefinition.position.Set(_transform->getPositionLocal().x, _transform->getPositionLocal().y);
        b2dConfig.bodyDefinition.angle = _transform->getRotationLocal();
        b2dConfig.bodyDefinition.type = gdeBodyTypeToB2dBodyType(_config.bodyType);
        b2dConfig.body = Physics::get().createBody(b2dConfig.bodyDefinition);

        switch (_config.bodyShapeType) {
            case BOX: b2dConfig.polygonShape.SetAsBox(_config.size.x / 2.f, _config.size.y / 2.f); break;
            case CIRCLE: b2dConfig.circleShape.m_p = {_transform->getPositionLocal().x, _transform->getPositionLocal().y};
                b2dConfig.circleShape.m_radius = _config.size.x;
                break;
            case POLYGON: LOG_W("Polygon shape not yet supported") break;
        }

        b2dConfig.fixtureDef.friction = _config.friction;
        b2dConfig.fixtureDef.density = _config.mass;
        b2dConfig.fixtureDef.restitution = _config.restitution;
        b2dConfig.fixtureDef.shape = &b2dConfig.polygonShape;

        b2dConfig.fixtureDef.filter.categoryBits = _config.mask;

        b2dConfig.fixture = b2dConfig.body->CreateFixture(&b2dConfig.fixtureDef);
        b2dConfig.lastPosition = _transform->getPositionLocal();
        b2dConfig.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    Body::~Body() {  }

    Vec2F Body::getPosition() const {
        auto _pos = b2dConfig.body->GetTransform().p;
        return {_pos.x, _pos.y};
    }

    float Body::getRotation() const {
        return b2dConfig.body->GetTransform().q.GetAngle() * 180.f / (float)M_PI;
    }

    b2BodyType Body::gdeBodyTypeToB2dBodyType(const BodyType& _bodyType) {
        switch (_bodyType) {
            case DYNAMIC: return b2BodyType::b2_dynamicBody;
            case STATIC: return b2BodyType::b2_staticBody;
            case KINEMATIC: return b2BodyType::b2_kinematicBody;
        }

        return b2BodyType::b2_dynamicBody;
    }

    void Body::setApplyGravity(bool applyGravity) const {
        b2dConfig.body->SetGravityScale(applyGravity ? b2dConfig.body->GetGravityScale() : 0);
    }

    bool Body::isApplyingGravity() const {
        return b2dConfig.body->GetGravityScale() != 0;
    }

    void Body::setGravityMultiplier(float _gravityMultiplier) const {
        b2dConfig.body->SetGravityScale(_gravityMultiplier);
    }

    float Body::getGravityMultiplayer() const {
        return b2dConfig.body->GetGravityScale();
    }

    void Body::setSensor(bool _sensor) const {
        b2dConfig.fixture->SetSensor(_sensor);
    }

    bool Body::isSensor() const {
        return b2dConfig.fixture->IsSensor();
    }

    void Body::setSelfCollisionMask(CollisionMask _mask) const {
        auto _filter = reinterpret_cast<b2Filter*>(b2dConfig.body->GetUserData().pointer);
        _filter->categoryBits = _mask;
    }


    // -------------------------------- SPRITE RENDERER


    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Scene* _scene, Texture* _texture) : texture(_texture) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic");
        viewport = _scene->getMainCamera()->getViewport();
    }

    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Scene* _scene) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic");
        viewport = _scene->getMainCamera()->getViewport();
    }

    Vec2F SpriteRenderer::getSize() const {
        return { (float)texture->getSize().x, (float)texture->getSize().y };
    }

    void SpriteRenderer::updateViewport(IViewPort* _viewport) {
        viewport = _viewport;
    }

    std::string SpriteRenderer::getTexturePath() {
        return texture->getPath();
    }

    std::string SpriteRenderer::getTextureName() {
        return Util::getFileNameFromPath(texture->getPath());
    }

    std::string SpriteRenderer::getTextureExtension() {
        return Util::getFileExtension(texture->getPath());
    }



    // -------------------------------------- NinePatchSprite --------------------------------------


    NinePatchSprite::NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic");
        interaction = _canvas->getGraph()->addComponent<UIInteractable>(_nodeID);
    }

    NinePatchSprite::NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic");
        texture = _texture;
        size = _texture->getRegion().size;
        interaction = _canvas->getGraph()->addComponent<UIInteractable>(_nodeID);
    }

    NinePatch& NinePatchSprite::getNinePatch() const {
        return texture->ninePatch;
    }

    Vec2I NinePatchSprite::getSize() const {
        return size;
    }



    static bool isSelectedFoo() { return false; }

    void UIInteractable::onEvent(const NodeID& _nodeID, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas) {
        if(interactionTrigger(_nodeID, _canvas)) {
            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onClick.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                if(mouseStatus == MouseStatus::MouseExited) {
                    mouseStatus = MouseStatus::MouseEntered;
                    if(onMouseEntered.isEmpty()) return;
                    onMouseEntered();
                }
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !onMouseExited.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseScrolledEvent>() && !onScroll.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mse = (MouseScrolledEvent*)&_event;
                onScroll({_mse->getScrollX(), _mse->getScrollY()});
                return;
            }

            if(_eventDispatcher.dispatchEvent<KeyReleasedEvent>() && !onKeyPressed.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _kre = (KeyReleasedEvent*)&_event;
                onKeyPressed(_kre->getKeyCode());
                return;
            }

            if(_eventDispatcher.dispatchEvent<JoystickButtonUpEvent>() && !onGamepadButtonPressed.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _jbue = (JoystickButtonUpEvent*)&_event;
                onGamepadButtonPressed(_jbue->getButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MobileTouchUpEvent>() && !onMobileClick.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mtue = (MobileTouchUpEvent*)&_event;
                onMobileClick(_mtue->getFingerID());
                return;
            }

            return;
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>()) {
            _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
            if(mouseStatus == MouseStatus::MouseEntered) {
                mouseStatus = MouseStatus::MouseExited;
                if(onMouseExited.isEmpty()) return;
                onMouseExited();
            }
        }
    }



    UIButton::UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _buttonTexture, const std::string& _text) {

    }




    UIText::UIText(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const std::string& _text) {

    }



    UIInteractable::UIInteractable(const NodeID& _nodeId) {  }
    CanvasEventStopper::CanvasEventStopper(const NodeID& _nodeId) {  }
    StaticTransform::StaticTransform(const NodeID& _nodeId) {  }
}
