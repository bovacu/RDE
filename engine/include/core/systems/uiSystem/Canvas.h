//
// Created by borja on 29/05/22.
//

#ifndef RDE_CANVAS_H
#define RDE_CANVAS_H

#include "core/graph/Graph.h"
#include "core/render/Camera.h"
#include "core/render/elements/Batch.h"
#include <stack>

namespace RDE {

    FORWARD_DECLARE_CLASS(UI, UIInteractable)

    struct CanvasElement {
        Node* node = nullptr;
        IRenderizable* renderizable = nullptr;
        UIInteractable* interactable = nullptr;
        IRenderizable* updatable = nullptr;
        int cropping = 0;
    };

    /**
     * @brief This class represents a container for elements related to the UI and it is in charge of render them, update them...
     * A Scene can have multiple canvas.
     */
    class Canvas {
        FRIEND_CLASS(Graph, Scene)

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

            IRenderizable* getRenderizable(Node* _node);
            IRenderizable* getUpdatable(Node* _node);

            void forceRender();

            void postRenderSync();
    };

}

#endif //RDE_CANVAS_H
