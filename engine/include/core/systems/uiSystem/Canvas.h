//
// Created by borja on 29/05/22.
//

#ifndef RDE_CANVAS_H
#define RDE_CANVAS_H

#include "core/render/elements/Batch.h"
#include "core/graph/components/Node.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/Enums.h"
#include <stack>

namespace RDE {

    class UIInteractable;

    struct UpdatableData {
		RDE_UI_UPDATABLE_NODE_ updatableType = RDE_UI_UPDATABLE_NODE_NONE;
        void* updatable = nullptr;
    };

    struct CanvasElement {
        Node* node = nullptr;
		UIAnchoring* anchoring;
        RenderizableInnerDataUI* renderizableInnerData;
        UIInteractable* interactable = nullptr;
        int cropping = 0;
        UpdatableData updatableData;
    };

	class Graph;
	class Camera;

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

            std::vector<Batch> batches;
            bool dirty = true;
            int maxIndicesPerDrawCall = 1000;

            std::vector<CanvasElement> uiRenderizables;
            std::vector<CanvasElement> uiInteractables;
            std::vector<CanvasElement> uiUpdatables;

        public:
            /**
             * @brief Returns the inner container Graph
             * @return Graph*
             */
            Graph* graph;

        public:
            explicit Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag);
            ~Canvas();

            /**
             * @brief Gets the rendering Camera
             * @return Camera*
             */
            Camera* getCamera();

            void onResize(uint _width, uint _height);

            void setCanvasResolution(const Vec2I& _resolution);

            Vec2I getCanvasResolution();

        private:
            void batchTreeElementPre(CanvasElement* _canvasElement, void* _data);
            void batchTreeElementPost(CanvasElement* _canvasElement, void* _data);

            void getRenderizable(Node* _node, CanvasElement* _canvasElement);
            void getUpdatable(Node* _node, CanvasElement* _canvasElement);

            void forceRender();

            void postRenderSync();
    };

}

#endif //RDE_CANVAS_H
