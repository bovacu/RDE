#pragma once

#include "core/systems/inputSystem/input/WindowInput.h"
#ifndef RDE_SCENE_H
#define RDE_SCENE_H

#include "nlohmann/json.hpp"
#include "core/util/Delta.h"
#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"

namespace RDE {

    class Camera;
    class Window; 
    class Engine;
    class Canvas;

    /**
     * @brief This class represents what is rendered in the window and all its related systems.
     */
    class Scene {
        friend class Graph;
        friend class ConfigManager;
        friend class WindowInput;

        private:
            /**
             * @see Graph
             */
            Graph mainGraph;

            /**
             * @brief All of the cameras that will render the scene.
             */
            std::vector<Camera*> cameras;

            /**
             * @brief All of the UI canvases that the scene has.
             */
            std::vector<Canvas*> canvases;

            /**
             * @brief Main camera renderer.
             */
            Camera* mainCamera = nullptr;

            /**
             * @brief Map PrefabKey -> NodeID.
             */
            std::unordered_map<std::string, NodeID> prefabs;

        public:
            /**
             * @brief Scene name.
             */
            std::string debugName;

            /**
             * @see Engine
             */
            Engine* engine = nullptr;

        protected:
            /**
             * @brief Callback for the Scene to refresh whatever is needed if the display is changed.
             */
            void onDisplayChanged();

        public:
            explicit Scene(Engine* _engine, const std::string& _debugName = "Scene");
            virtual ~Scene();

            /**
             * @brief This function is called when loading the scene from the configuration, it is used to make a custom loading
             * of your custom components for the internal ECS system.
             * @param _manager gives access to all of the managers of the engine to load anything needed.
             * @param _window the window of the application
             * @param _sceneJson the configuration of the .yaml file used to load the scene
             */
            virtual void preInit(Manager* _manager, Window* _window, const nlohmann::json& _sceneJson) {  }

            /**
             * @brief This function is called only once and when the SceneManager/displayScene is invoked. It is used to
             * initialized any needed values of the scene.
             */
            virtual void onInit() {  }

            /**
             * @brief This function captures all the events that happen inside the application. It captures too mouse, keyboard
             * and gamepads, but it is not recommended to use this input inside this function. The class InputManager is
             * the one you are looking for. This is used to manage window related or screen related events mainly.
             * @param _event the event that was just captured.
             * @see InputManager
             */
            virtual void onEvent(Event& _event);

            /**
             * @brief This function is executed every frame of the application and here we should update logic and take inputs
             * to update the logic and components.
             * @param _dt time that passed between the current frame and the last one.
             */
            virtual void onUpdate(Delta _dt);

            /**
             * @brief This function is called a fixed amount of times per second. By default it is 60 times per second, but it
             * can be modified with Engine/setFixedDelta. This loop is used to handle physics and things that need a
             * consistent frame rate to work properly.
             * @param _dt fixed value initially set at 1/60/
             */
            virtual void onFixedUpdate(Delta _dt);

            /**
             * @brief This function is called as the last update method, after update and fixedUpdate, which is an ideal place
             * to remove elements.
             * @param _dt time that passed between the current frame and the last one.
             */
            virtual void onLateUpdate(Delta _dt);

            /**
             * @brief This function is also called every frame and renders the geometry of the game. It is not virtual because
             * the main rendering pipeline is handled inside the core of the engine. Users don't need to mess with it.
             * @param the time that passed between the current frame and the last one.
             */
            void onRender(Delta _dt);

            /**
             * @brief This function is used to render specifically ImGui elements, nothing else.
             * @param the time that passed between the current frame and the last one.
             */
            virtual void onImGuiRender(Delta _dt) {  }

            /**
             * @brief This function is used to drawBatched debugging lines, squares, circles...
             * @param the time that passed between the current frame and the last one.
             */
            virtual void onDebugRender(Delta _dt);

            /**
             * @brief This function is called when unloading the scene and should be used to release any allocated memory or
             * resources.
             */
            virtual void onEnd() {  }

            /**
             * @brief This function returns the graph system of this specific scene, so we can access any entity and it's components,
             * create new ones or destroy them.
             * @return the graph of the scene.
             */
            Graph* getGraph();

            /**
             * @brief Returns all of the cameras used in the scene.
             * @return std::vector<Camera*>&
             */
            std::vector<Camera*>& getCameras();

            /**
             * @brief Returns the main camera.
             * @return Camera*
             */
            Camera* getMainCamera();

            /**
             * @brief Adds a new camera to the scene and returns it to make any modifications needed.
             * @param window Window
             * @return Camera*
             */
            Camera* addCamera(Window* window);

            /**
             * @brief Activates or deactivates any given camera. If it is disabled, it won't render anything.
             * @param _cameraID Camera to enable or disable
             * @param _enable If the camera gets enabled or disabled
             */
            void enableCamera(Node* _camera, bool _enable);

            /**
             * @brief Destroys a specific camera of the scene.
             * @param _cameraID Camera to be destroyed
             */
            void removeCamera(Node* _camera);

            /**
             * @brief Sets an specific camera as the main one.
             * @param _camera Camera to be set as main
             */
            void switchMainCamera(Node* _camera);

            /**
             * @brief Returns the name of the scene.
             * @return const std::string&
             */
            [[nodiscard]] const std::string& getName() const { return debugName; }

            /**
             * @brief Adds a new Canvas to the scene.
             * @param _canvasTag Name of the Canvas
             * @return Canvas*
             */
            Canvas* addNewCanvas(const std::string& _canvasTag);

            /**
             * @brief Removes a Canvas by its name.
             * @param _canvasTag Canvas name
             */
            void removeCanvas(const std::string& _canvasTag);

            /**
             * @brief Removes a Canvas by its reference.
             * @param _canvas Canvas reference
             */
            void removeCanvas(Canvas* _canvas);

            /**
             * @brief Returns all the canvases of the Scene
             * @return std::vector<Canvas*>&
             */
            std::vector<Canvas*>& getCanvases();

            /**
             * @brief Returns all of the prefabs IDs
             * @return std::vector<NodeID>
             */
            std::vector<NodeID> getPrefabs();

            /**
             * @brief Returns a prefab by its key.
             * @param _prefabKey Prefab key
             * @return NodeID
             */
            NodeID getPrefab(const std::string& _prefabKey);
    };

}

#endif //RDE_SCENE_H
