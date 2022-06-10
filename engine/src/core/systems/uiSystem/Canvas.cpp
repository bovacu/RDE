//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/render/Camera.h"
#include "core/render/Renderer.h"

namespace GDE {

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag) {
        scene = _scene;
        auto _cameraID = graph.createNode("CanvasCamera");
        auto* _canvasTransform = graph.getComponent<Transform>(_cameraID);
        camera = graph.addComponent<Camera>(_cameraID, _window, _canvasTransform, _cameraID);
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }

    void Canvas::onUpdate(Delta _dt) {
        auto& _registry = graph.getNodeContainer();
        _registry.view<Transform, Active>(entt::exclude<StaticTransform>).each([&](const auto _entity, Transform& _transform, const Active& _) {
            _transform.update(&graph);
        });
    }

    void Canvas::onRender() {
        auto& _registry = graph.getNodeContainer();
        auto _sprites = _registry.group<NinePatchSprite>(entt::get<Transform, Active>);

        Renderer::beginDraw(*camera, graph.getComponent<Transform>(camera->ID));
        _sprites.each([](const auto _entity, NinePatchSprite& _spriteRenderer, const Transform& _transform, const Active& _) {
            Renderer::draw(_spriteRenderer, _transform);
        });
        Renderer::endDraw();

        graph.onRenderDel();
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();
        auto _sprites = _registry.group<NinePatchSprite>(entt::get<Transform, Active>);

        Renderer::beginDebugDraw(*camera, graph.getComponent<Transform>(camera->ID));
        _sprites.each([](const auto _entity, NinePatchSprite& _ninePatch, const Transform& _transform, const Active& _) {
            if(!_ninePatch.getNinePatch().isEnabled()) return;

            auto _rects = _ninePatch.getNinePatch().subRects;
            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[0].bottomLeftCorner.x + (float)_rects[0].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[0].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[0].size.y / 2.f},
                                 {(float)_rects[0].size.x, (float)_rects[0].size.y}, Color::Green);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[1].bottomLeftCorner.x + (float)_rects[1].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[1].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[1].size.y / 2.f},
                                 {(float)_rects[1].size.x, (float)_rects[1].size.y}, Color::Black);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[2].bottomLeftCorner.x + (float)_rects[2].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[2].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[2].size.y / 2.f},
                                 {(float)_rects[2].size.x, (float)_rects[2].size.y}, Color::Blue);
//
            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[3].bottomLeftCorner.x + (float)_rects[3].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[3].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[3].size.y / 2.f},
                                 {(float)_rects[3].size.x, (float)_rects[3].size.y}, Color::Yellow);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[4].bottomLeftCorner.x + (float)_rects[4].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[4].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[4].size.y / 2.f},
                                 {(float)_rects[4].size.x, (float)_rects[4].size.y}, Color::Orange);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[5].bottomLeftCorner.x + (float)_rects[5].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[5].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[5].size.y / 2.f},
                                 {(float)_rects[5].size.x, (float)_rects[5].size.y}, Color::Gray);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[6].bottomLeftCorner.x + (float)_rects[6].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[6].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[6].size.y / 2.f},
                                 {(float)_rects[6].size.x, (float)_rects[6].size.y}, Color::Magenta);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[7].bottomLeftCorner.x + (float)_rects[7].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[7].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[7].size.y / 2.f},
                                 {(float)_rects[7].size.x, (float)_rects[7].size.y}, Color::Purple);

            Renderer::drawSquare({_transform.getPositionLocal().x - (float)_ninePatch.getRegion().size.x / 2.f + (float)_rects[8].bottomLeftCorner.x + (float)_rects[8].size.x / 2.f, _transform.getPositionLocal().y + (float)(_rects[8].bottomLeftCorner.y - _ninePatch.getRegion().size.y) - (float)_ninePatch.getRegion().size.y / 2.f + (float)_rects[8].size.y / 2.f},
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