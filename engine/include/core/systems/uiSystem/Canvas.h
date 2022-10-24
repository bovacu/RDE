//
// Created by borja on 29/05/22.
//

#ifndef GDE_CANVAS_H
#define GDE_CANVAS_H

#include "core/graph/Graph.h"
#include "core/render/Camera.h"
#include "core/render/elements/Batch.h"
#include <stack>

namespace GDE {

    FORWARD_DECLARE_CLASS(UI, UIInteractable)

    struct CanvasElement {
        NodeID nodeID = NODE_ID_NULL;
        IRenderizable* renderizable = nullptr;
        UIInteractable* interactable = nullptr;
        IRenderizable* updatable = nullptr;
        bool cropping = false;
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

            std::stack<NodeID> stencils;
            std::vector<CanvasElement> canvasElementsOrderedList;

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
             * @brief Makes the Canva's Camera viewport type to match with the Scene camera viewport
             */
            void matchMainCameraViewPort();

            /**
             * @brief Handles the event related to the UI elements.
             * @param _eventDispatcher Event dispatcher
             * @param _event Event
             */
            void onEvent(Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event);

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

        private:
            void traverseTree(const NodeID& _nodeID, bool _earlyBreak, void* _data, void (Canvas::*_preFunc)(const NodeID&, bool&, void*), void (Canvas::*_postFunc)(const NodeID&, void*));

            void batchTreeElementPre(CanvasElement* _canvasElement, void* _data);
            void batchTreeElementPost(CanvasElement* _canvasElement, void* _data);

            void onEventTreeElement(CanvasElement* _canvasElement, void* _data);

            void updateTreeElement(CanvasElement* _canvasElement, void* _data);

            void createElementListTreePre(const NodeID& _nodeID, bool& _earlyBreak, void* _data);

            IRenderizable* getRenderizable(const NodeID& _nodeID);
            IRenderizable* getUpdatable(const NodeID& _nodeID);

            void forceRender();

            void onTreeChanged(void* _data);
    };

}

#endif //GDE_CANVAS_H
