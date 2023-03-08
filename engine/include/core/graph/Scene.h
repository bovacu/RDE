#ifndef RDE_SCENE_H
#define RDE_SCENE_H

#include "nlohmann/json.hpp"
#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"
#include "core/systems/uiSystem/Canvas.h"

namespace RDE {

    class Camera;
    class Window; 
    class Engine;
    struct RenderizableInnerData;

    /**
     * @brief This class represents what is rendered in the window and all its related systems.
     */
    class Scene {
        friend class Graph;
        friend class ConfigManager;
        friend class WindowInput;
        friend class Engine;
        friend class ImGuiScene;
        friend class SceneManager;

        private:
            /**
             * @brief All of the cameras that will render the scene.
             */
            std::vector<Camera*> cameras;

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

            /**
             * @brief Graph for entities hierarchy.
             * @see Graph
             */
            Graph* graph = nullptr;

            /**
             * @brief All of the UI canvases that the scene has.
             */
            Canvas* canvas = nullptr;

            /**
             * @brief Main camera renderer.
             */
            Camera* mainCamera = nullptr;

        protected:
            /**
             * @brief Callback for the Scene to refresh whatever is needed if the display is changed.
             */
            void onDisplayChanged();

            /**
             * @brief This function captures all the events that happen inside the application. It captures too mouse, keyboard
             * and gamepads, but it is not recommended to use this input inside this function. The class InputManager is
             * the one you are looking for. This is used to manage window related or screen related events mainly.
             * @param _event the event that was just captured.
             * @see InputManager
             */
            void onInnerEvent(Event& _event) {
                onInnerEventUI(_event);
                onInnerEventHierarchy(_event);
                onEvent(_event);
            }

            void onInnerEventHierarchy(Event& _event); 
            void onInnerEventUI(Event& _event);

            /**
             * @brief This function is executed every frame of the application and here we should update logic and take inputs
             * to update the logic and components.
             * @param _dt time that passed between the current frame and the last one.
             */
            void onInnerUpdate(Delta _dt) {
                onInnerUpdateHierarchy(_dt);
                onInnerUpdateUI(_dt);
                onUpdate(_dt);
            }
            void onInnerUpdateHierarchy(Delta _dt); 
            void onInnerUpdateUI(Delta _dt);

            /**
             * @brief This function is called a fixed amount of times per second. By default it is 60 times per second, but it
             * can be modified with Engine/setFixedDelta. This loop is used to handle physics and things that need a
             * consistent frame rate to work properly.
             * @param _dt fixed value initially set at 1/60/
             */
            void onInnerFixedUpdate(Delta _dt) {
                onInnerFixedUpdateHierarchy(_dt);
                onInnerFixedUpdateUI(_dt);
                onFixedUpdate(_dt);
            }
            void onInnerFixedUpdateHierarchy(Delta _dtt); 
            void onInnerFixedUpdateUI(Delta _dt);

            /**
             * @brief This function is called as the last update method, after update and fixedUpdate, which is an ideal place
             * to remove elements.
             * @param _dt time that passed between the current frame and the last one.
             */
            void onInnerLateUpdate(Delta _dt) {
                onInnerLateUpdateHierarchy(_dt);
                onInnerLateUpdateUI(_dt);
                onLateUpdate(_dt);
            }
            void onInnerLateUpdateHierarchy(Delta _dt); 
            void onInnerLateUpdateUI(Delta _dt);
            
            void recalculateRenderizableTree(Node* _node);
            void recalculateRenderizableTreeUI(Node* _node);

            /**
             * @brief This function is also called every frame and renders the geometry of the game. It is not virtual because
             * the main rendering pipeline is handled inside the core of the engine. Users don't need to mess with it.
             * @param _dt the time that passed between the current frame and the last one.
             */
            void onInnerRender(Delta _dt) {
                onInnerRenderHierarchy(_dt); 
                onInnerRenderUI(_dt); 
            }
            void onInnerRenderHierarchy(Delta _dt); 
            void onInnerRenderUI(Delta _dt);

            /**
             * @brief This function is used to drawBatched debugging lines, squares, circles...
             * @param the time that passed between the current frame and the last one.
             */
			void onInnerDebugRender(Delta _dt);
            void onInnerDebugRenderHierarchy(Delta _dt); 
            void onInnerDebugRenderUI(Delta _dt);

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
            virtual void onEvent(Event& _event) {  }

            /**
             * @brief This function is executed every frame of the application and here we should update logic and take inputs
             * to update the logic and components.
             * @param _dt time that passed between the current frame and the last one.
             */
            virtual void onUpdate(Delta _dt) {  }

            /**
             * @brief This function is called a fixed amount of times per second. By default it is 60 times per second, but it
             * can be modified with Engine/setFixedDelta. This loop is used to handle physics and things that need a
             * consistent frame rate to work properly.
             * @param _dt fixed value initially set at 1/60/
             */
            virtual void onFixedUpdate(Delta _dt) {  }

            /**
             * @brief This function is called as the last update method, after update and fixedUpdate, which is an ideal place
             * to remove elements.
             * @param _dt time that passed between the current frame and the last one.
             */
            virtual void onLateUpdate(Delta _dt) {  }

            /**
             * @brief This function is used to render specifically ImGui elements, nothing else.
             * @param the time that passed between the current frame and the last one.
             */
            virtual void onImGuiRender(Delta _dt) {  }

            /**
             * @brief This function is used to drawBatched debugging lines, squares, circles...
             * @param the time that passed between the current frame and the last one.
             */
            virtual void onDebugRender(Delta _dt) {  }

            /**
             * @brief This function is called when unloading the scene and should be used to release any allocated memory or
             * resources.
             */
            virtual void onEnd() {  }

            /**
             * @brief Returns all of the cameras used in the scene.
             * @return std::vector<Camera*>&
             */
            std::vector<Camera*>& getCameras();

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
