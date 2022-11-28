//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIMask.h"

namespace RDE {

    struct OnEventData {
        Engine* engine;
        EventDispatcher* eventDispatcher;
        Event* event;
    };

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag, true) {
        scene = _scene;
        auto _cameraNode = graph.createNode("CanvasCamera");
        camera = _cameraNode->addComponent<Camera>(_window);
        camera->setCameraSize(_window->getWindowSize());
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }





    void Canvas::onEvent(Engine* _engine, Event& _event) {
        EventDispatcher _eventDispatcher(_event);
        OnEventData _data { _engine, &_eventDispatcher, &_event };
        for(auto _it = uiInteractables.rbegin(); _it != uiInteractables.rend(); _it++) {
            _it->interactable->onEvent(_it->node, _data.engine, *_data.eventDispatcher, *_data.event);
        }
    }




    IRenderizable* Canvas::getUpdatable(Node* _node) {
        IRenderizable* _renderizable = nullptr;

        if(_node->hasComponent<UISlider>()) {
            _renderizable = _node->getComponent<UISlider>();
        }

        if(_node->hasComponent<UIInput>()) {
            _renderizable = _node->getComponent<UIInput>();
        }

        if(_renderizable != nullptr && _renderizable->isEnabled()) {
            return _renderizable;
        }

        return nullptr;
    }

    void Canvas::onUpdate(Delta _dt) {
        for(auto& _it : uiUpdatables) {
            _it.updatable->onUpdate(_dt);
        }
    }




    void Canvas::onLateUpdate() {
        if(graph.isRenderizableTreeDirty) {
            graph.isRenderizableTreeDirty = false;
            uiInteractables.clear();
            uiUpdatables.clear();
            uiRenderizables.clear();
            recalculateRenderizableTree(graph.sceneRoot);
        }
    }




    IRenderizable* Canvas::getRenderizable(Node* _node) {
        IRenderizable* _renderizable = nullptr;

        if(_node->hasComponent<UIImage>()) {
            _renderizable = _node->getComponent<UIImage>();
        }

        if(_node->hasComponent<UIText>()) {
            _renderizable = _node->getComponent<UIText>();
        }

        if(_renderizable != nullptr && _renderizable->isEnabled()) {
            return _renderizable;
        }

        return nullptr;
    }

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _data) {
        Batch* _currentBatch = &batches.back();

        if(_canvasElement->cropping > 0) {
            if(_canvasElement->node->hasComponent<UIImage>() && _canvasElement->renderizable != nullptr && _canvasElement->renderizable->isEnabled()) {
                forceRender();
                auto* _mask = _canvasElement->node->getComponent<UIMask>();

                _currentBatch = &batches.back();

                glEnable(GL_STENCIL_TEST);

                glColorMask(GL_FALSE , GL_FALSE , GL_FALSE , GL_FALSE);
                glStencilFunc(GL_ALWAYS , _mask->inverted ? 1 : 2, _mask->inverted ? 0xFF : ~0);
                glStencilOp(GL_REPLACE , GL_REPLACE , GL_REPLACE);

                auto* _renderizable = _canvasElement->node->getComponent<UIImage>();

                if (_currentBatch->shader == nullptr || _renderizable->getTexture() != _currentBatch->textureID || _currentBatch->shader->getShaderID() != _renderizable->shaderID || _currentBatch->indexBuffer.size() + 6 >= maxIndicesPerDrawCall) {
                    Batch _newBatch;
                    _newBatch.shader = scene->engine->manager.shaderManager.getShader(_renderizable->shaderID);
                    _newBatch.textureID = _renderizable->getTexture();
                    batches.emplace_back(_newBatch);
                    _currentBatch = &batches.back();
                }

                _renderizable->drawBatched(_currentBatch->vertexBuffer, _currentBatch->indexBuffer, *_renderizable->node->getTransform(), *camera->getViewport());
                forceRender();

                glColorMask( GL_TRUE , GL_TRUE , GL_TRUE , GL_TRUE);
                glStencilFunc( GL_EQUAL , _mask->inverted ? 0 : 2 , _mask->inverted ? 0xFF : ~0);
                glStencilOp( GL_KEEP , GL_KEEP , GL_KEEP);
            }
        }

        if(_canvasElement->renderizable != nullptr) {
            auto* _renderizable = _canvasElement->renderizable;

            if (_currentBatch->shader == nullptr || _renderizable->getTexture() != _currentBatch->textureID || _currentBatch->shader->getShaderID() != _renderizable->shaderID || _currentBatch->indexBuffer.size() + 6 >= maxIndicesPerDrawCall) {
                Batch _newBatch;
                _newBatch.shader = scene->engine->manager.shaderManager.getShader(_renderizable->shaderID);
                _newBatch.textureID = _renderizable->getTexture();
                batches.emplace_back(_newBatch);
                _currentBatch = &batches.back();
            }

            _renderizable->drawBatched(_currentBatch->vertexBuffer, _currentBatch->indexBuffer, *_renderizable->node->getTransform(), *camera->getViewport());
        }
    }

    void Canvas::batchTreeElementPost(CanvasElement* _canvasElement, void* _data) {
        if(_canvasElement->cropping == 0) {
            forceRender();
            glDisable(GL_STENCIL_TEST);
        }
    }

    void Canvas::forceRender() {
        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, nullptr);
        _renderManager.drawUI(batches);

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

    void Canvas::onRender() {
        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, (Transform*)graph.getComponent<UITransform>(camera->node->getID()));

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);

        // Depending on the element, we will need to restrict the area that can be rendered. In that case and as we
        // precalculate a list in the correct order of rendering, we need to know when to begin the cropping and when to
        // finish it, in this case we create a list of root elements that need to be cropped. We store how many children
        // the root cropped element has, and we simply make a countdown until all children have been drawn, then we apply
        // the crop.
        std::vector<int> _cropList;

        for(auto& _it : uiRenderizables) {
            batchTreeElementPre(&_it, nullptr);

            for(auto& _crop : _cropList) {
                _crop--;
                if(_crop == 0) batchTreeElementPost(&_it, nullptr);
            }

            if(_it.cropping > 0) _cropList.push_back(_it.cropping);
        }

        _renderManager.drawUI(batches);

        postRenderSync();
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();

        auto& _renderManager = graph.scene->engine->manager.renderManager;
       _renderManager.beginDebugDraw(*camera, (Transform*)graph.getComponent<UITransform>(camera->node->getID()));

        _registry.view<UIImage, UITransform, Active>().each([this, &_renderManager](const auto _entity, UIImage& _uiImage, UITransform& _transform, const Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _uiImage.getSize();
            Vec2F _pointSize = {4, 4};

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;
            _pointSize *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);

            _renderManager.drawSquare(_pos, _pointSize, Color::Blue);
            _renderManager.setPointSize(4);
        });

        _registry.view<UICheckbox, UITransform, Active>().each([this, &_renderManager](const NodeID _nodeID, UICheckbox& _checkbox, UITransform& _transform, Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _checkbox.getSize();

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);
        });

        _registry.view<UIText, UITransform, Active>().each([this, &_renderManager](const NodeID _nodeID, UIText& _uiText, UITransform& _transform, Active& _) {
            if(!_uiText.isEnabled()) return;

            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _uiText.getSize();

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);

            Vec2F _pointSize = {4, 4};
            _renderManager.drawSquare(_pos, _pointSize, Color::Yellow);
            _renderManager.setPointSize(4);
        });

        _renderManager.endDebugDraw();
    }

    Camera* Canvas::getCamera() {
        return camera;
    }

    void Canvas::onResize(uint _width, uint _height) {
        ((UITransform*)graph.sceneRoot->getTransform())->setSize({ (float)_width, (float)_height });
        camera->setCameraSize((int)_width, (int)_height);
    }

    void Canvas::setCanvasResolution(const Vec2I& _resolution) {
        camera->getViewport()->virtualResolution = _resolution;
    }

    Vec2I Canvas::getCanvasResolution() {
        return camera->getViewport()->virtualResolution;
    }

    void Canvas::recalculateRenderizableTree(Node* _node) {
        if(!_node->isActive()) {
            return;
        }

        CanvasElement _canvasElement { _node };

        if(graph.getNodeContainer().any_of<UIInteractable>(_node->getID())) {
            _canvasElement.interactable = _node->getComponent<UIInteractable>();
            uiInteractables.push_back(_canvasElement);
        }

        if(graph.getNodeContainer().any_of<UIText, UIButton, UIImage, UISlider>(_node->getID())) {
            _canvasElement.renderizable = getRenderizable(_node);

            if(graph.getNodeContainer().any_of<UIMask>(_node->getID()) && _node->getComponent<UIMask>()->isEnabled()) {
                _canvasElement.cropping = _node->getTransform()->getEnabledChildrenCount();
            }

            if(_canvasElement.renderizable != nullptr) {
                uiRenderizables.push_back(_canvasElement);
            }
        }

        if(graph.getNodeContainer().any_of<UISlider, UIInput>(_node->getID())) {
            _canvasElement.updatable = getUpdatable(_node);

            if(_canvasElement.updatable != nullptr) {
                uiUpdatables.push_back(_canvasElement);
            }
        }

        for(auto* _child : _node->getTransform()->children) {
            recalculateRenderizableTree(_child->node);
        }
    }

    void Canvas::postRenderSync() {
        for(auto* _dirtyTransform : graph.dirtyTransforms) {
            ((UITransform*)_dirtyTransform)->clearDirty();
        }
        graph.dirtyTransforms.clear();
    }

}