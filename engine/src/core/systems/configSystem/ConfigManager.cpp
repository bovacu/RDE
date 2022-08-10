//
// Created by borja on 15/05/22.
//
#include <fstream>
#include "core/systems/configSystem/ConfigManager.h"
#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/render/window/Window.h"
#include "core/render/elements/TextureAtlasManager.h"
#include "core/Engine.h"

namespace GDE {

    void ConfigManager::loadGDEConfig(GDEConfig* _config, FileManager& _manager) {

        auto _configPath = "assets/config/desktop.json";
        #if ANDROID
        _configPath = "assets/config/android.json";
        #endif
        auto _fileHandler = _manager.open(_configPath, FileMode::READ);
        nlohmann::json data = nlohmann::json::parse(_manager.readFullFile(_fileHandler).content);
        _manager.close(_fileHandler);
        _config->windowData.title = data["title"].get<std::string>(),
        _config->windowData.fullScreen = data["fullscreen"].get<bool>(),
        _config->windowData.vsync = data["vsync"].get<bool>(),
        _config->windowData.size = Vec2<unsigned int>{data["resolution"][0].get<unsigned int>(),data["resolution"][1].get<unsigned int>()};
        _config->projectData.iconPath = data["icon"].get<std::string>();
        _config->projectData.mainSceneToLoad = data["main_scene"].get<std::string>();
    }

    void ConfigManager::loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath) {
        auto _fileHandler = _manager->fileManager.open(_configFilePath, FileMode::READ);
        nlohmann::json _sceneJson = nlohmann::json::parse(_manager->fileManager.readFullFile(_fileHandler).content);
        _manager->fileManager.close(_fileHandler);

        loadAssets(_scene, _window, _sceneJson);
        loadNodes(_scene, _window, _sceneJson, createNodes(_scene, _sceneJson));
        _scene->preInit(_manager, _window, _sceneJson);
    }

    void ConfigManager::loadAssets(Scene* _scene, Window* _window, const nlohmann::json& _json) {

        ENGINE_ASSERT(_json.contains("assets"), "Scene MUST have the 'assets' section.")
        auto& _assets = _json["assets"];

        if(_assets.contains("textures")) {
            auto& _texturesNode = _assets["textures"];
            for (const auto& _texture : _texturesNode) {
                auto _fileHandler = _scene->engine->manager.fileManager.open(_texture.get<std::string>(), FileMode::READ);
                _scene->engine->manager.textureManager.loadSpriteSheet(YAML::Load(_scene->engine->manager.fileManager.readFullFile(_fileHandler).content));
                _scene->engine->manager.fileManager.close(_fileHandler);
            }
        }

        if(_assets.contains("fonts")) {
            auto& _fontsNodes = _assets["fonts"];
            for (const auto& _font : _fontsNodes) {
                for(auto _i = 0; _i < _font["sizes"].size(); _i++)
                    _scene->engine->manager.fontManager.loadFont(_scene->engine->manager.fileManager, _font["path"].get<std::string>(), _font["sizes"][_i].get<int>());
            }
        }

        if(_assets.contains("sfx")) {

        }

        if(_assets.contains("music")) {

        }
    }

    std::unordered_map<std::string, NodeID_JsonPair> ConfigManager::createNodes(Scene* _scene, const nlohmann::json& _sceneJson) {
        std::unordered_map<std::string, NodeID_JsonPair> _nodes;

        std::unordered_map<NodeID, std::string> _parentingMap;
        if(_sceneJson.contains("nodes")) {
            auto& _nodesJson = _sceneJson["nodes"];
            int _entityCount = 0;
            for(auto& _node : _nodesJson) {
                if(_node.contains("components") && _node["components"].contains("camera") && _node["components"]["camera"].contains("is_main") && _node["components"]["camera"]["is_main"].get<bool>()) {
                    auto _tag = _node.contains("tag") ? _node["tag"].get<std::string>() : APPEND_S("Entity_", _entityCount);
                    _nodes[_tag] = { _scene->getMainCamera()->ID, _node };
                    _scene->getMainGraph()->getComponent<Tag>(_scene->getMainCamera()->ID)->tag = _tag;
                    _entityCount++;
                    continue;
                }

                auto _tag = _node.contains("tag") ? _node["tag"].get<std::string>() : APPEND_S("Entity_", _entityCount);
                ENGINE_ASSERT(_nodes.find(_tag) == _nodes.end(), "Scene CANNOT have repeated 'tag' for different nodes, it is a unique identifier. Another '", _tag, "' prefab key was already defined.")
                auto _entityID = _scene->getMainGraph()->createNode(_tag);
                _nodes[_tag] = {_entityID, _node };
                _entityCount++;

                if(_node.contains("parent") && !_node["parent"].is_null()) {
                    _parentingMap[_entityID] = _node["parent"];
                }
            }

            for(auto& _child : _parentingMap) {
                _scene->getMainGraph()->setParent(_child.first, _nodes[_child.second].nodeId);
            }
        }

        return _nodes;
    }

    void ConfigManager::loadNodes(Scene* _scene, Window* _window, const nlohmann::json& _sceneJson, const std::unordered_map<std::string, NodeID_JsonPair>& _nodes) {
        if(_sceneJson.contains("nodes")) {
            for(auto& _node : _nodes) {
                auto _entityID = _node.second.nodeId;
                auto& _nodeJson = _node.second.json;

                if(_nodeJson.contains("components")) {
                    auto& _components = _nodeJson["components"];

                    if(!_components.contains("active")) {
                        _scene->getMainGraph()->removeComponent<Active>(_entityID);
                    }

                    ENGINE_ASSERT(_components.contains("transform"), "Every node MUST contain a transform.");
                    loadTransformComponent(_scene, _entityID, _components["transform"]);

                    if(_components.contains("sprite_renderer")) {
                        loadSpriteRendererComponent(_entityID, _scene, _components["sprite_renderer"]);
                    }

                    if(_components.contains("body")) {
                        loadBodyComponent(_entityID, _scene, _components["body"]);
                    }

                    if(_components.contains("text_renderer")) {
                        loadTextRendererComponent(_entityID, _scene, _components["text_renderer"]);
                    }

                    if(_components.contains("camera")) {
                        loadCameraComponent(_entityID, _scene, _window, _components["camera"]);
                    }

                    if(_components.contains("is_prefab") && _components["is_prefab"].get<bool>()) {
                        _scene->getMainGraph()->setNodeActive(_entityID, false);
                        ENGINE_ASSERT(_scene->prefabs.find(_node.first) == _scene->prefabs.end(), "Scene CANNOT have repeated 'tag' for different prefabs. Another '", _node.first, "' prefab key was already defined.")
                        _scene->prefabs[_node.first] = _entityID;
                    }
                }
            }
        }
    }

    void ConfigManager::loadTransformComponent(Scene* _scene, const NodeID& _nodeID, const nlohmann::json& _transformJson) {
        auto* _nodeTransform = _scene->getMainGraph()->getComponent<Transform>(_nodeID);
        if(_transformJson.contains("position")) {
            _nodeTransform->setPosition(_transformJson["position"][0].get<float>(), _transformJson["position"][1].get<float>());
        }

        if(_transformJson.contains("scale")) {
            _nodeTransform->setScale(_transformJson["scale"][0].get<float>(), _transformJson["scale"][1].get<float>());
        }

        if(_transformJson.contains("rotation")) {
            _nodeTransform->setRotation(_transformJson["rotation"].get<float>());
        }
    }

    void ConfigManager::loadSpriteRendererComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _spriteRendererJson) {
        auto _ownerEntityID = _nodeID;
        auto* _spriteRenderer = _scene->getMainGraph()->addComponent<SpriteRenderer>(_ownerEntityID, _scene);

        ENGINE_ASSERT(_spriteRendererJson.contains("texture"), "SpriteRenderer component MUST have section 'texture'.")
        ENGINE_ASSERT(_spriteRendererJson["texture"].contains("atlas"), "SpriteRenderer component MUST have section 'atlas' in section 'texture'.")
        ENGINE_ASSERT(_spriteRendererJson["texture"].contains("tile"), "SpriteRenderer component MUST have section 'tile' in 'texture'.")

        _spriteRenderer->setTexture(
                _scene->engine->manager.textureManager.getSubTexture(_spriteRendererJson["texture"]["atlas"].get<std::string>(),
                                                                     _spriteRendererJson["texture"]["tile"].get<std::string>()));
        if(_spriteRendererJson.contains("color")) {
            auto& _color = _spriteRendererJson["color"];
            _spriteRenderer->color = Color { _color[0].get<unsigned char>(), _color[1].get<unsigned char>(), _color[2].get<unsigned char>(), _color[3].get<unsigned char>()};
        }

        if(_spriteRendererJson.contains("layer")) {
            _spriteRenderer->layer = _spriteRendererJson["layer"].get<int>();
        }

        if(_spriteRendererJson.contains("shader")) {
            _spriteRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(_spriteRendererJson["shader"].get<std::string>());
        }
    }

    void ConfigManager::loadBodyComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _bodyJson) {
        auto _ownerEntityID = _nodeID;

        BodyType _bodyType;
        if(_bodyJson.contains("type")) {
            auto _type = _bodyJson["type"].get<std::string>();
            if(std::equal(_type.begin(), _type.end(), "STATIC")) {
                _bodyType = BodyType::STATIC;
            } else if(std::equal(_type.begin(), _type.end(), "DYNAMIC")) {
                _bodyType = BodyType::DYNAMIC;
            } else if(std::equal(_type.begin(), _type.end(), "KINEMATIC")) {
                _bodyType = BodyType::KINEMATIC;
            } else {
                _bodyType = BodyType::STATIC;
                LOG_W("Tried to load a body with a body type not registered: ", _type)
            }
        } else {
            _bodyType = BodyType::STATIC;
        }

        BodyShapeType _bodyShapeType;
        if(_bodyJson.contains("shape")) {
            auto _shape = _bodyJson["shape"].get<std::string>();
            if(std::equal(_shape.begin(), _shape.end(), "BOX")) {
                _bodyShapeType = BodyShapeType::BOX;
            } else if(std::equal(_shape.begin(), _shape.end(), "CIRCLE")) {
                _bodyShapeType = BodyShapeType::CIRCLE;
            } else if(std::equal(_shape.begin(), _shape.end(), "POLYGON")) {
                _bodyShapeType = BodyShapeType::POLYGON;
            } else {
                _bodyShapeType = BodyShapeType::BOX;
                LOG_W("Tried to load a body with a body shape not registered: ", _shape)
            }
        } else {
            _bodyShapeType = BodyShapeType::BOX;
        }

        ENGINE_ASSERT(_bodyJson.contains("size"), "Body MUST have section 'size'.")
        ENGINE_ASSERT(_bodyJson["size"].size() == 2, "Body MUST have section 'size' with exactly 2 elements.")
        ENGINE_ASSERT(_bodyJson.contains("mask"), "Body MUST have section 'mask'.")

        BodyConfig _bodyConfig {
                .size = {_bodyJson["size"][0].get<float>(), _bodyJson["size"][1].get<float>()},
                .mask = static_cast<CollisionMask>(_bodyJson["mask"].get<int>()),
                .bodyType = _bodyType,
                .bodyShapeType = _bodyShapeType,
        };

        if(_bodyJson.contains("restitution")) {
            _bodyConfig.restitution = _bodyJson["restitution"].get<float>();
        }

        if(_bodyJson.contains("friction")) {
            _bodyConfig.friction = _bodyJson["friction"].get<float>();
        }

        if(_bodyJson.contains("mass")) {
            _bodyConfig.mass = _bodyJson["mass"].get<float>();
        }

        auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
        _scene->getMainGraph()->addComponent<Body>(_ownerEntityID, _scene, _bodyConfig, _ownerTransform);
    }

    void ConfigManager::loadCameraComponent(const NodeID& _nodeID, Scene* _scene, Window* _window, const nlohmann::json& _cameraJson) {
        if(_cameraJson.contains("is_main") && _cameraJson["is_main"].get<bool>()) {

            if(_cameraJson.contains("zoom")) {
                _scene->getMainCamera()->setCurrentZoomLevel(_cameraJson["zoom"].get<float>());
            }

            if(_cameraJson.contains("zoom_speed")) {
                _scene->getMainCamera()->setZoomSpeed(_cameraJson["zoom_speed"].get<float>());
            }

            auto _viewPortType = _cameraJson["viewport"].get<std::string>();
            if(std::equal(_viewPortType.begin(), _viewPortType.end(), "ADAPTIVE")) {

                auto _virtualRes = _cameraJson.contains("view_port_virtual_resolution") && _cameraJson["view_port_virtual_resolution"].size() == 2 ?
                                   Vec2I{_cameraJson["view_port_virtual_resolution"][0].get<int>(), _cameraJson["view_port_virtual_resolution"][1].get<int>()} :
                                   _window->getWindowSize();

                auto _deviceRes = _cameraJson.contains("view_port_device_resolution") && _cameraJson["view_port_device_resolution"].size() == 2 ?
                                  Vec2I{_cameraJson["view_port_device_resolution"][0].get<int>(), _cameraJson["view_port_device_resolution"][1].get<int>()} :
                                  _window->getWindowSize();

                _scene->getMainCamera()->setAdaptiveViewport(_virtualRes, _deviceRes);
            } else if(std::equal(_viewPortType.begin(), _viewPortType.end(), "FREE")) {
                _scene->getMainCamera()->setFreeViewport(_window->getWindowSize());
            } else {
                _scene->getMainCamera()->setFreeViewport(_window->getWindowSize());
                LOG_W("Tried to load a camera viewport with a type not registered: ", _viewPortType)
            }

            _scene->getMainCamera()->getViewport()->update(_window->getWindowSize());
        } else {
            auto _ownerEntityID = _nodeID;
            auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
            auto* _camera = _scene->getMainGraph()->addComponent<Camera>(_ownerEntityID, _window, _ownerTransform);

            if(_cameraJson.contains("zoom")) {
                _camera->setCurrentZoomLevel(_cameraJson["zoom"].get<float>());
            }

            if(_cameraJson.contains("zoom_speed")) {
                _camera->setZoomSpeed(_cameraJson["zoom_speed"].get<float>());
            }

            auto _viewPortType = _cameraJson["viewport"].get<std::string>();
            if(std::equal(_viewPortType.begin(), _viewPortType.end(), "ADAPTIVE")) {

                auto _virtualRes = _cameraJson.contains("view_port_virtual_resolution") && _cameraJson["view_port_virtual_resolution"].size() == 2 ?
                                   Vec2I{_cameraJson["view_port_virtual_resolution"][0].get<int>(), _cameraJson["view_port_virtual_resolution"][1].get<int>()} :
                                   _window->getWindowSize();

                auto _deviceRes = _cameraJson.contains("view_port_device_resolution") && _cameraJson["view_port_device_resolution"].size() == 2 ?
                                  Vec2I{_cameraJson["view_port_device_resolution"][0].get<int>(), _cameraJson["view_port_device_resolution"][1].get<int>()} :
                                  _window->getWindowSize();

                _camera->setAdaptiveViewport(_virtualRes, _deviceRes);
            } else if(std::equal(_viewPortType.begin(), _viewPortType.end(), "FREE")) {
                _camera->setFreeViewport(_window->getWindowSize());
            } else {
                _camera->setFreeViewport(_window->getWindowSize());
                LOG_W("Tried to load a camera viewport with a type not registered: ", _viewPortType)
            }

            _camera->getViewport()->update(_window->getWindowSize());
            _scene->getCameras().push_back(_camera);
        }
    }

    void ConfigManager::loadTextRendererComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _textRendererJson) {
        auto _ownerEntityID = _nodeID;

        ENGINE_ASSERT(_textRendererJson.contains("font"), "TextRenderer MUST have section 'font'.")
        ENGINE_ASSERT(_textRendererJson.contains("text"), "TextRenderer MUST have section 'text'.")

        auto* _textRenderer = _scene->getMainGraph()->addComponent<TextRenderer>(_ownerEntityID,
                                                                                 _scene, _scene->engine->manager.fontManager.getDefaultFont(_textRendererJson["font"].get<std::string>()),
                                                                                 _textRendererJson["text"].get<std::string>());

        if(_textRendererJson.contains("color")) {
            auto& _color = _textRendererJson["color"];
            _textRenderer->color = Color {_color[0].get<unsigned char>(), _color[1].get<unsigned char>(),
                                          _color[2].get<unsigned char>(), _color[3].get<unsigned char>()};
        }

        if(_textRendererJson.contains("layer")) {
            _textRenderer->layer = _textRendererJson["layer"].get<int>();
        }

        if(_textRendererJson.contains("shader")){
            _textRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(_textRendererJson["shader"].get<std::string>());

        }
    }

    void ConfigManager::instantiatePrefab(Scene* _scene, const YAML::Node& _yaml) {

    }

}

