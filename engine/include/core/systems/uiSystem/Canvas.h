//
// Created by borja on 29/05/22.
//

#ifndef RDE_CANVAS_H
#define RDE_CANVAS_H

#include "core/graph/Graph.h"
#include "core/render/Camera.h"
#include "core/render/elements/Batch.h"
#include "core/graph/components/Node.h"
#include "core/render/elements/IRenderizable.h"
#include <stack>

namespace RDE {

    class UIInteractable;

    enum UpdatableType {
        UT_NONE        = 0,
        UT_UI_INPUT    = 1,
        UT_UI_SLIDER   = 2
    };

    struct UpdatableData {
        UpdatableType updatableType = UpdatableType::UT_NONE;
        void* updatable = nullptr;
    };

    struct CanvasElement {
        Node* node = nullptr;
        RenderizableInnerDataUI* renderizableInnerData;
        UIInteractable* interactable = nullptr;
        int cropping = 0;
        UpdatableData updatableData;
    };

    /**
     * @brief This class represents a container for elements related to the UI and it is in charge of render them, update them...
     * A Scene can have multiple canvas.
     */
    class Canvas {
        friend class Graph;
        friend class Scene;

        private:
            /**
             * @brief Scene this canvas is attached to.
             */
            Scene* scene;

            /**
             * @brief Camera that renders the Canvas elements.
             */
            Camera* camera = nullptr;

            /**
             * @brief Layer of the canvas.
             */
            int layer = 0;

            /**
             * @brief Inner elements container.
             */
            Graph graph;

            std::vector<Batch> batches;
            bool dirty = true;
            int maxIndicesPerDrawCall = 1000;

            std::vector<CanvasElement> uiRenderizables;
            std::vector<CanvasElement> uiInteractables;
            std::vector<CanvasElement> uiUpdatables;

        public:
            explicit Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag);

            /**
             * @brief Returns the inner container Graph
             * @return Graph*
             */
            Graph* getGraph();

            /**
             * @brief Gets the rendering Camera
             * @return Camera*
             */
            Camera* getCamera();

            /**
             * @brief Handles the event related to the UI elements.
             * @param _eventDispatcher Event dispatcher
             * @param _event Event
             */
            void onEvent(Engine* _engine, Event& _event);

            /**
             * @brief Handles the update of the the UI elements.
             * @param _dt Delta time
             */
            void onUpdate(Delta _dt);

            /**
             * @brief Handles the rendering of the UI elements.
             */
            void onRender();

            /**
             * @brief Handles the debug rendering of the UI elements.
             */
            void onDebugRender();

            void onLateUpdate();

            void onResize(uint _width, uint _height);

            void setCanvasResolution(const Vec2I& _resolution);

            Vec2I getCanvasResolution();

        private:
            void recalculateRenderizableTree(Node* _node);

            void batchTreeElementPre(CanvasElement* _canvasElement, void* _data);
            void batchTreeElementPost(CanvasElement* _canvasElement, void* _data);

            void getRenderizable(Node* _node, CanvasElement* _canvasElement);
            void getUpdatable(Node* _node, CanvasElement* _canvasElement);

            void forceRender();

            void postRenderSync();
    };

}

#endif //RDE_CANVAS_H
