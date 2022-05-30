//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/render/Camera.h"

namespace GDE {

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag) {
        auto _cameraID = graph.createNode("CanvasCamera");
        auto* _canvasTransform = graph.getComponent<Transform>(_cameraID);
        camera = graph.addComponent<Camera>(_cameraID, _window, _canvasTransform, _cameraID);
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }


    void Canvas::onRender() {
//        auto& _registry = graph->getNodeContainer();
//        auto _sprites = _registry.group<SpriteRenderer>(entt::get<Transform, Active>);
//        auto _buttons = _registry.group<UIButton>(entt::get<Transform, Active>);
//        auto _texts = _registry.group<UIText>(entt::get<Transform, Active>);



//        graph.onRenderDel();
    }

}