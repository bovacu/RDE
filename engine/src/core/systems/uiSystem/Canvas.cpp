//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/render/Camera.h"
#include "core/graph/components/Transform.h"
#include "core/graph/Scene.h"
#include "core/graph/components/UI.h"
#include "core/graph/components/NineSliceSprite.h"
#include "core/render/RenderManager.h"
#include "core/Engine.h"

namespace GDE {

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag) {
        scene = _scene;
        auto _cameraID = graph.createNode("CanvasCamera");
        auto* _canvasTransform = graph.getComponent<Transform>(_cameraID);
        camera = graph.addComponent<Camera>(_cameraID, _window, _canvasTransform);
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }

    void Canvas::onEvent(EventDispatcher& _eventDispatcher, Event& _event) {
        auto& _registry = graph.getNodeContainer();
        auto _interactables = _registry.group<UIInteractable>(entt::get<Active>);

        _interactables.each([&_eventDispatcher, &_event, this](const auto _entity, UIInteractable& _interacable, const Active& _) {
            _interacable.onEvent(_entity, _eventDispatcher, _event, this);
        });

        graph.onEventDel(_registry, _event);
    }

    void Canvas::onUpdate(Delta _dt) {
        auto& _registry = graph.getNodeContainer();
        _registry.view<Transform, Active>(entt::exclude<StaticTransform>).each([&](const auto _entity, Transform& _transform, const Active& _) {
            _transform.update();
        });
    }

    void Canvas::onRender() {
        auto& _registry = graph.getNodeContainer();
        auto _sprites = _registry.group<NineSliceSprite>(entt::get<Transform, Active>);
        auto _texts = _registry.group<TextRenderer>(entt::get<Transform, Active>);

        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, graph.getComponent<Transform>(camera->ID));

            _sprites.each([&_renderManager](const auto _entity, NineSliceSprite& _nineSlice, const Transform& _transform, const Active& _) {
                _renderManager.draw((const IRenderizable*)&_nineSlice, _transform);
            });

            _texts.each([&_renderManager](const auto _entity, TextRenderer& _textRenderer, const Transform& _transform, const Active& _) {
                _renderManager.draw((const IRenderizable*)&_textRenderer, _transform);
            });

        _renderManager.endDraw();

        graph.onRenderDel(_registry);
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();
        auto _sprites = _registry.group<NineSliceSprite>(entt::get<Transform, Active>);

        auto& _renderManager = graph.scene->engine->manager.renderManager;
       _renderManager.beginDebugDraw(*camera, graph.getComponent<Transform>(camera->ID));
        _sprites.each([&_renderManager](const auto _entity, NineSliceSprite& _nineSlice, const Transform& _transform, const Active& _) {
            if(!_nineSlice.getNineSlice().isEnabled()) return;

            auto _rects = _nineSlice.getNineSlice().subRects;
            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[0].bottomLeftCorner.x + (float)_rects[0].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[0].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[0].size.y / 2.f},
                                      {(float)_rects[0].size.x, (float)_rects[0].size.y}, Color::Green);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[1].bottomLeftCorner.x + (float)_rects[1].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[1].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[1].size.y / 2.f},
                                      {(float)_rects[1].size.x, (float)_rects[1].size.y}, Color::Black);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[2].bottomLeftCorner.x + (float)_rects[2].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[2].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[2].size.y / 2.f},
                                      {(float)_rects[2].size.x, (float)_rects[2].size.y}, Color::Blue);
//
            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[3].bottomLeftCorner.x + (float)_rects[3].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[3].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[3].size.y / 2.f},
                                      {(float)_rects[3].size.x, (float)_rects[3].size.y}, Color::Yellow);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[4].bottomLeftCorner.x + (float)_rects[4].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[4].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[4].size.y / 2.f},
                                      {(float)_rects[4].size.x, (float)_rects[4].size.y}, Color::Orange);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[5].bottomLeftCorner.x + (float)_rects[5].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[5].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[5].size.y / 2.f},
                                      {(float)_rects[5].size.x, (float)_rects[5].size.y}, Color::Gray);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[6].bottomLeftCorner.x + (float)_rects[6].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[6].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[6].size.y / 2.f},
                                      {(float)_rects[6].size.x, (float)_rects[6].size.y}, Color::Magenta);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[7].bottomLeftCorner.x + (float)_rects[7].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[7].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[7].size.y / 2.f},
                                      {(float)_rects[7].size.x, (float)_rects[7].size.y}, Color::Purple);

            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[8].bottomLeftCorner.x + (float)_rects[8].size.x / 2.f,
                                       _transform.getPosition().y + (float)(_rects[8].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[8].size.y / 2.f},
                                      {(float)_rects[8].size.x, (float)_rects[8].size.y}, Color::White);

        });
        _renderManager.endDebugDraw();
    }

    Camera* Canvas::getCamera() {
        return camera;
    }

    void Canvas::matchMainCameraViewPort() {
        auto* _mainCameraViewPort = scene->getMainCamera()->getViewport();
        if(dynamic_cast<AdaptiveViewPort*>(_mainCameraViewPort)) {
            camera->setAdaptiveViewport(_mainCameraViewPort->getVirtualResolution(), _mainCameraViewPort->getDeviceResolution());
        } else
            camera->setFreeViewport(_mainCameraViewPort->getDeviceResolution());
    }

}