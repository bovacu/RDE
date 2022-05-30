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
        auto _sprites = _registry.group<SpriteRenderer>(entt::get<Transform, Active>);

        Renderer::beginDraw(*camera, graph.getComponent<Transform>(camera->ID));
        _sprites.each([](const auto _entity, SpriteRenderer& _spriteRenderer, const Transform& _transform, const Active& _) {
            Renderer::draw(_spriteRenderer, _transform);
        });
        Renderer::endDraw();

        graph.onRenderDel();
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