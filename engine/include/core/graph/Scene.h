#pragma once

#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include "core/util/Delta.h"
#include "core/graph/Graph.h"

namespace GDE {

    class Camera;
    class Window;
    class Scene {

        friend class Graph;

        private:
            Graph mainGraph;
            NodeID mainCameraID;
            std::vector<Camera*> cameras;
            Camera* mainCamera = nullptr;

        protected:
            std::string debugName;

        public:
            explicit Scene(const std::string& _debugName = "Scene");
            virtual ~Scene() {  };

            virtual void onInit() {  }
            virtual void onEvent(Event& _event);
            virtual void onUpdate(Delta _dt);
            virtual void onFixedUpdate(Delta _dt);
            void onRender(Delta _dt);
            virtual void onImGuiRender(Delta _dt) {  }
            virtual void onDebugRender(Delta _dt);
            virtual void onEnd() {  }

            Graph* getMainGraph();

            std::vector<Camera*>& getCameras();
            Camera* getMainCamera();
            Camera* addCamera(Window* window);
            void enableCamera(const NodeID& _cameraID, bool _enable);
            void removeCamera(const NodeID& _cameraID);
            void switchMainCamera(const NodeID& _camera);

            [[nodiscard]] const std::string& getName() const { return debugName; }
    };

}

#endif //ENGINE_SCENE_H
