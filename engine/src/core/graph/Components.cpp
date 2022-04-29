// Created by borja on 11/3/22.

#include "core/graph/Components.h"
#include "core/util/Functions.h"
#include "core/systems/physicsSystem/Physics.h"

namespace GDE {

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        auto _scalingFactor = Engine::get().getScene()->getMainCamera()->getViewport()->getScalingFactor();
        glm::vec3 _scale = {localScale.x * _scalingFactor.x, localScale.y * _scalingFactor.y, 1};
        localModelMatrix = glm::translate(glm::mat4(1.0f), localPosition) * _rot * glm::scale(glm::mat4(1.0f), _scale);
        return localModelMatrix;
    }

    void Transform::update(Graph* _graph) {
        if (parent != NODE_ID_NULL) {
            auto* _parentT = _graph->getComponent<Transform>(parent);
            modelMatrix = _parentT->modelMatrix * localModelMatrix;
        } else
            modelMatrix = getLocalModelMatrix();
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        Util::worldToScreenSize(_x, _y);
        localPosition = glm::vec3 {_x, _y, 0.0f};
        getLocalModelMatrix();
    }

    Vec2F Transform::getPositionLocal() const {
        float _x = localPosition.x, _y = localPosition.y;
        Util::screenToWorldCoords(_x, _y);
        return {_x, _y};
    }

    void Transform::setRotation(float _rotation) {
        localRotation = _rotation;
        getLocalModelMatrix();
    }

    float Transform::getRotationLocal() const {
        return localRotation;
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        localScale = {_x, _y, 1.0f};
        getLocalModelMatrix();
    }

    Vec2F Transform::getScaleLocal() const {
        return {localScale.x, localScale.y};
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        Util::worldToScreenSize(_x, _y);
        localPosition.x += _x;
        localPosition.y += _y;
        getLocalModelMatrix();
    }

    void Transform::rotate(float _amount) {
        localRotation += _amount;
        getLocalModelMatrix();
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        localScale.x += _x;
        localScale.y += _y;
        getLocalModelMatrix();
    }

    Vec2F Transform::getPositionWorld() const {
        float _x = modelMatrix[3][0], _y =  modelMatrix[3][1];
        Util::screenToWorldCoords(_x, _y);
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

    bool Transform::isConstant() {
        return constant;
    }


    /// -------------------------------------------

    TextRenderer::TextRenderer(Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        shaderID = ShaderManager::get().getShader("basicText");
        texture = &font->getTexture();
    }

    TextRenderer::TextRenderer(Font* _font) {
        font = _font;
        shaderID = ShaderManager::get().getShader("basicText");
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
        fontSize = _fontSize;
        recalcTextDimensions(innerText);
        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
    }

    int TextRenderer::getFontSize() {
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

    void TextRenderer::setColor(const Color &_color) {
        color = _color;
    }

//    Shape& TextRenderer::getDebugShape() {
//        return debugShape;
//    }


    ///-------------------------------- BODY ---------------------------


    Body::Body(const BodyConfig& _bodyConfig, const Transform& _transform) {
        updateBodyConfig(_bodyConfig);
        cpBodySetPosition(body, {_transform.getPositionLocal().x, _transform.getPositionLocal().y});
        bodyConfig = _bodyConfig;
    }

    Body::~Body() {
        if(shape != nullptr) cpShapeFree(shape);
        cpBodyDestroy(body);
    }

    Vec2F Body::getPosition() const {
        auto _pos = cpBodyGetPosition(body);
        return {static_cast<float>(_pos.x), static_cast<float>(_pos.y)};
    }

    void Body::updateBodyConfig(const BodyConfig& _bodyConfig) {
        if(body != nullptr) cpBodyFree(body);

        switch (_bodyConfig.bodyType) {
            case DYNAMIC: body = cpSpaceAddBody(Physics::get().getSpace(),
                                                cpBodyNew(0, 0));
                break;
            case STATIC: body = cpSpaceAddBody(Physics::get().getSpace(), cpBodyNewStatic());
                break;
            case KINEMATIC: body = cpSpaceAddBody(Physics::get().getSpace(), cpBodyNewKinematic());
                break;
        }
    }

    void Body::addCollider() {
        switch (bodyConfig.bodyShapeType) {
            case BOX: shape = cpBoxShapeNew(body, bodyConfig.size.x, bodyConfig.size.y, 0);
                break;
            case CIRCLE: shape = cpCircleShapeNew(body, bodyConfig.size.x, {0, 0});
                break;
            default:
                LOG_W("Physics for polygons not yet implemented")
        }

        cpSpaceAddShape(Physics::get().getSpace(), shape);
        cpShapeSetElasticity(shape, bodyConfig.restitution);
        cpShapeSetFriction(shape, bodyConfig.friction);
        cpShapeSetMass(shape, bodyConfig.mass);
    }

    float Body::getRotation() const {
        return (float)(cpBodyGetRotation(body).y * 180.f / M_PI);
    }

    void Body::setCollisionMask(const std::string& _maskName) {
        mask = Physics::get().getCollisionMask(_maskName);
        cpShapeSetCollisionType(shape, mask);
    }

    CollisionMask Body::getCollisionMask() const {
        return mask;
    }

}
