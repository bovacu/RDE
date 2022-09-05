//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/render/Camera.h"
#include "core/render/Renderer.h"
#include "core/graph/components/Transform.h"

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

        graph.onEventDel(_event);
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
//        auto _buttons = _registry.group<UIButton>(entt::get<Transform, Active>);
//        auto _texts = _registry.group<UIText>(entt::get<Transform, Active>, entt::exclude<UIButton>);

        Renderer::beginDraw(*camera, graph.getComponent<Transform>(camera->ID));
            _sprites.each([](const auto _entity, NineSliceSprite& _nineSlice, const Transform& _transform, const Active& _) {
                Renderer::draw((const IRenderizable*)&_nineSlice, _transform);
            });

//            // TODO add rendering for text in UI
//            _buttons.each([](const auto _entity, UIButton& _button, const Transform& _transform, const Active& _) {
//                Renderer::draw(*_button.background, _transform);
//    //            Renderer::draw(*_button.text, _transform);
//            });
//
//            _texts.each([](const auto _entity, UIText& _text, const Transform& _transform, const Active& _) {
//    //            Renderer::draw(_text, _transform);
//            });
        Renderer::endDraw();

        graph.onRenderDel();
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();
        auto _sprites = _registry.group<NineSliceSprite>(entt::get<Transform, Active>);

        Renderer::beginDebugDraw(*camera, graph.getComponent<Transform>(camera->ID));
        _sprites.each([](const auto _entity, NineSliceSprite& _nineSlice, const Transform& _transform, const Active& _) {
            if(!_nineSlice.getNineSlice().isEnabled()) return;

            auto _rects = _nineSlice.getNineSlice().subRects;
            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[0].bottomLeftCorner.x + (float)_rects[0].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[0].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[0].size.y / 2.f},
                                 {(float)_rects[0].size.x, (float)_rects[0].size.y}, Color::Green);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[1].bottomLeftCorner.x + (float)_rects[1].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[1].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[1].size.y / 2.f},
                                 {(float)_rects[1].size.x, (float)_rects[1].size.y}, Color::Black);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[2].bottomLeftCorner.x + (float)_rects[2].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[2].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[2].size.y / 2.f},
                                 {(float)_rects[2].size.x, (float)_rects[2].size.y}, Color::Blue);
//
            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[3].bottomLeftCorner.x + (float)_rects[3].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[3].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[3].size.y / 2.f},
                                 {(float)_rects[3].size.x, (float)_rects[3].size.y}, Color::Yellow);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[4].bottomLeftCorner.x + (float)_rects[4].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[4].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[4].size.y / 2.f},
                                 {(float)_rects[4].size.x, (float)_rects[4].size.y}, Color::Orange);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[5].bottomLeftCorner.x + (float)_rects[5].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[5].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[5].size.y / 2.f},
                                 {(float)_rects[5].size.x, (float)_rects[5].size.y}, Color::Gray);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[6].bottomLeftCorner.x + (float)_rects[6].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[6].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[6].size.y / 2.f},
                                 {(float)_rects[6].size.x, (float)_rects[6].size.y}, Color::Magenta);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[7].bottomLeftCorner.x + (float)_rects[7].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[7].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[7].size.y / 2.f},
                                 {(float)_rects[7].size.x, (float)_rects[7].size.y}, Color::Purple);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[8].bottomLeftCorner.x + (float)_rects[8].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[8].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[8].size.y / 2.f},
                                 {(float)_rects[8].size.x, (float)_rects[8].size.y}, Color::White);

        });
        Renderer::endDebugDraw();
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