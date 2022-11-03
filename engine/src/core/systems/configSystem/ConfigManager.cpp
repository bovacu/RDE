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
#include "core/graph/components/Transform.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/util/Functions.h"

namespace GDE {

    void ConfigManager::loadGDEConfig(GDEConfig* _config, FileManager& _manager) {

        auto _configPath = "assets/config/desktop.json";

        #if IS_ANDROID()
        _configPath = "assets/config/android.json";
        #endif

        #if IS_IOS()
        _configPath = "assets/config/ios.json";
        #endif

        auto _fileHandler = _manager.open(_configPath, FileMode::READ);
        nlohmann::json _data = nlohmann::json::parse(_manager.readFullFile(_fileHandler).content);
        _manager.close(_fileHandler);

        _config->windowData.title = _data.contains("title") ? _data["title"].get<std::string>() : "GDEProject";
        _config->windowData.fullScreen = _data.contains("fullscreen") && _data["fullscreen"].get<bool>();
        _config->windowData.vsync = !_data.contains("vsync") || _data["vsync"].get<bool>();

        #if !IS_MOBILE()
        _config->windowData.size = _data.contains("resolution") ? Vec2<unsigned int>{_data["resolution"][0].get<unsigned int>(),_data["resolution"][1].get<unsigned int>()} :
                                   Vec2<unsigned int> {1280, 720};
        #endif

        _config->projectData.iconPath = _data.contains("icon") ? _data["icon"].get<std::string>() : "";
        _config->projectData.resourcesPath = _data.contains("resources_path") ?  _data["resources_path"].get<std::string>() : "";
        _config->projectData.localizationConfig.localizationPath = _data.contains("localization_path") ? _data["localization_path"].get<std::string>() : "";
    }

    void ConfigManager::loadResources(GDEConfig* _gdeConfig, Manager* _manager) {
        if(_gdeConfig->projectData.resourcesPath.empty()) {
            return;
        }

        auto _fileHandler = _manager->fileManager.open(APPEND_S("assets/", _gdeConfig->projectData.resourcesPath), FileMode::READ);

        if(_fileHandler == nullptr) {
            LOG_W("Resources path was set in the config.json, but the file couldn't be loaded!")
            return;
        }

        nlohmann::json _data = nlohmann::json::parse(_manager->fileManager.readFullFile(_fileHandler).content);

        if(_data.empty()) {
            return;
        }

        _manager->fileManager.close(_fileHandler);

        loadAssets(_manager, _data);
    }

    void ConfigManager::loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath) {
        auto _fileHandler = _manager->fileManager.open(_configFilePath, FileMode::READ);

        if(_fileHandler == nullptr) {
            LOG_W("JSON file for scene ", _scene->debugName, " was not found, so a DefaultScene is being created for it.")
            return;
        }

        auto _content = _manager->fileManager.readFullFile(_fileHandler).content;
        nlohmann::json _sceneJson = nlohmann::json::parse(_content);
        _manager->fileManager.close(_fileHandler);

        loadAssets(_manager, _sceneJson);
        loadNodes(_scene, _window, _sceneJson, createNodes(_scene, _sceneJson));
        _scene->preInit(_manager, _window, _sceneJson);
    }

    void ConfigManager::loadAssets(Manager* _manager, const nlohmann::json& _json) {

        ENGINE_ASSERT(_json.contains("assets"), "Scene MUST have the 'assets' section.")
        auto& _assets = _json["assets"];

        if(_assets.contains("textures")) {
            auto& _texturesNode = _assets["textures"];
            for (const auto& _texture : _texturesNode) {
                _manager->textureManager.loadSpriteSheet(APPEND_S("assets/", _texture.get<std::string>()));
            }
        }

        if(_assets.contains("fonts")) {
            auto& _fontsNodes = _assets["fonts"];
            for (const auto& _font : _fontsNodes) {
                for(auto _i = 0; _i < _font["sizes"].size(); _i++)
                    _manager->fontManager.loadFont(_manager->fileManager, APPEND_S("assets/", _font["path"].get<std::string>()), _font["sizes"][_i].get<int>());
            }
        }

        if(_assets.contains("sfx")) {
            auto& _sfxs = _assets["sfx"];
            for (const auto& _sfx : _sfxs) {
                _manager->soundManager.loadSfx(APPEND_S("assets/", _sfx.get<std::string>()));
            }
        }

        if(_assets.contains("music")) {
            auto& _musics = _assets["music"];
            for (const auto& _music : _musics) {
                _manager->soundManager.loadMusic(APPEND_S("assets/", _music.get<std::string>()));
            }
        }
    }

    std::unordered_map<std::string, Node_JsonPair> ConfigManager::createNodes(Scene* _scene, const nlohmann::json& _sceneJson) {
        std::unordered_map<std::string, Node_JsonPair> _nodes;

        std::unordered_map<Node*, std::string> _parentingMap;
        if(_sceneJson.contains("nodes")) {
            auto& _nodesJson = _sceneJson["nodes"];
            int _entityCount = 0;
            for(auto& _node : _nodesJson) {
                if(_node.contains("components") && _node["components"].contains("camera") && _node["components"]["camera"].contains("is_main") && _node["components"]["camera"]["is_main"].get<bool>()) {
                    auto _tag = _node.contains("tag") ? _node["tag"].get<std::string>() : APPEND_S("Entity_", _entityCount);
                    _nodes[_tag] = { _scene->getMainCamera()->node, _node };
                    _scene->getMainGraph()->getComponent<Tag>(_scene->getMainCamera()->node->getID())->tag = _tag;
                    _entityCount++;
                    continue;
                }

                auto _tag = _node.contains("tag") ? _node["tag"].get<std::string>() : APPEND_S("Entity_", _entityCount);
                ENGINE_ASSERT(_nodes.find(_tag) == _nodes.end(), "Scene CANNOT have repeated 'tag' for different nodes, it is a unique identifier. Another '", _tag, "' prefab key was already defined.")
                auto _entityNode = _scene->getMainGraph()->createNode(_tag);
                _nodes[_tag] = {_entityNode, _node };
                _entityCount++;

                if(_node.contains("parent") && !_node["parent"].is_null()) {
                    _parentingMap[_entityNode] = _node["parent"];
                }
            }

            for(auto& _child : _parentingMap) {
                _scene->getMainGraph()->setParent(_child.first, _nodes[_child.second].node);
            }
        }

        return _nodes;
    }

    void ConfigManager::loadNodes(Scene* _scene, Window* _window, const nlohmann::json& _sceneJson, const std::unordered_map<std::string, Node_JsonPair>& _nodes) {
        if(_sceneJson.contains("nodes")) {
            for(auto& _node : _nodes) {
                auto _entity = _node.second.node;
                auto& _nodeJson = _node.second.json;

                if(_nodeJson.contains("components")) {
                    auto& _components = _nodeJson["components"];

                    if(!_components.contains("active")) {
                        _entity->removeComponent<Active>();
                    }

                    ENGINE_ASSERT(_components.contains("transform"), "Every node MUST contain a transform.");
                    loadTransformComponent(_scene, _entity, _components["transform"]);

                    if(_components.contains("sprite_renderer")) {
                        loadSpriteRendererComponent(_entity, _scene, _components["sprite_renderer"]);
                    }

                    if(_components.contains("body")) {
                        loadBodyComponent(_entity, _scene, _components["body"]);
                    }

                    if(_components.contains("text_renderer")) {
                        loadTextRendererComponent(_entity, _scene, _components["text_renderer"]);
                    }

                    if(_components.contains("camera")) {
                        loadCameraComponent(_entity, _scene, _window, _components["camera"]);
                    }

                    if(_components.contains("is_prefab") && _components["is_prefab"].get<bool>()) {
//                        _scene->getMainGraph()->setNodeActive(_scene->getMainGraph()->getComponent<Node>(_entityID), false);
//                        ENGINE_ASSERT(_scene->prefabs.find(_node.first) == _scene->prefabs.end(), "Scene CANNOT have repeated 'tag' for different prefabs. Another '", _node.first, "' prefab key was already defined.")
//                        _scene->prefabs[_node.first] = _entityID;
                        LOG_W("ConfigManager Prefab needs implementation!!")
                    }
                }
            }
        }
    }

    void ConfigManager::loadTransformComponent(Scene* _scene, Node* _node, const nlohmann::json& _transformJson) {
        auto* _nodeTransform = _node->getTransform();
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

    void ConfigManager::loadSpriteRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _spriteRendererJson) {
        auto* _texture = _scene->engine->manager.textureManager.getSubTexture(_spriteRendererJson["texture"]["atlas"].get<std::string>(),
                                                                              _spriteRendererJson["texture"]["tile"].get<std::string>());
        auto* _spriteRenderer = _node->addComponent<SpriteRenderer>(_texture);

        ENGINE_ASSERT(_spriteRendererJson.contains("texture"), "SpriteRenderer component MUST have section 'texture'.")
        ENGINE_ASSERT(_spriteRendererJson["texture"].contains("atlas"), "SpriteRenderer component MUST have section 'atlas' in section 'texture'.")
        ENGINE_ASSERT(_spriteRendererJson["texture"].contains("tile"), "SpriteRenderer component MUST have section 'tile' in 'texture'.")

        if(_spriteRendererJson.contains("color")) {
            auto& _color = _spriteRendererJson["color"];
            _spriteRenderer->color = Color { _color[0].get<unsigned char>(), _color[1].get<unsigned char>(), _color[2].get<unsigned char>(), _color[3].get<unsigned char>()};
        }

        if(_spriteRendererJson.contains("layer")) {
            _spriteRenderer->layer = _spriteRendererJson["layer"].get<int>();
        }

        if(_spriteRendererJson.contains("shader")) {
            _spriteRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(
                    _spriteRendererJson["shader"].get<std::string>())->getShaderID();
        }
    }

    void ConfigManager::loadBodyComponent(Node* _node, Scene* _scene, const nlohmann::json& _bodyJson) {
        BodyConfig _bodyConfig;
        std::vector<ShapeConfig> _shapeConfigs;

        // load body config
        if(_bodyJson.contains("type")) {
            auto _bodyType = _bodyJson["type"].get<std::string>();
            _bodyType = TO_LOWER_S(_bodyType);

            if(std::equal(_bodyType.begin(), _bodyType.end(), "static")) {
                _bodyConfig.physicsBodyType = _bodyConfig.physicsBodyType = PhysicsBodyType::STATIC;
            } else if(std::equal(_bodyType.begin(), _bodyType.end(), "dynamic")) {
                _bodyConfig.physicsBodyType = _bodyConfig.physicsBodyType = PhysicsBodyType::DYNAMIC;
            } else if(std::equal(_bodyType.begin(), _bodyType.end(), "kinematic")) {
                _bodyConfig.physicsBodyType = _bodyConfig.physicsBodyType = PhysicsBodyType::KINEMATIC;
            } else {
                throw std::runtime_error(APPEND_S("Chosen PhysicsBodyType '", _bodyType, "' is not a known type for a PhysicsBody"));
            }
        } else {
            _bodyConfig.physicsBodyType = PhysicsBodyType::DYNAMIC;
        }

        if(_bodyJson.contains("mass")) {
            ENGINE_ASSERT(_bodyJson["mass"].is_number(), "'mass' is a float, so it is a single value, Ex: \"mass\": 5.4")
            _bodyConfig.mass = _bodyJson["mass"].get<float>();
        }

        if(_bodyJson.contains("physicShapes")) {
            for(auto& _shapeJson : _bodyJson["physicShapes"]) {
                ShapeConfig _shapeConfig;

                ENGINE_ASSERT(_shapeJson.contains("type"), "Each PhysicsShape must contain the 'type' field, which is a PhysicsShapeType enum, so possible values are [BOX,CIRCLE,POLYGON,SEGMENT] no case sensitive, Ex: \"type\": \"BOX\"")
                auto _type = _shapeJson["type"].get<std::string>();
                _type = TO_LOWER_S(_type);

                if(_shapeJson.contains("offset")) {
                    ENGINE_ASSERT(_bodyJson["offset"].size() == 2, "'offset' is a Vec2F, so it must contain exactly 2 elements, Ex: \"offset\": [x, y]")
                    _shapeConfig.offset.x = _bodyJson["offset"][0].get<float>();
                    _shapeConfig.offset.y = _bodyJson["offset"][1].get<float>();
                }

                if(std::equal(_type.begin(), _type.end(), "box")) {
                    _shapeConfig.type = PhysicsShapeType::BOX;
                    ENGINE_ASSERT(_shapeJson.contains("size"), "PhysicsShape of type BOX must have field 'size' with a Vec2F: \"size\", Ex: [32, 32]")
                    _shapeConfig.size = { _shapeJson["size"][0].get<float>(), _shapeJson["size"][1].get<float>() };
                } else if(std::equal(_type.begin(), _type.end(), "circle")) {
                    _shapeConfig.type = PhysicsShapeType::CIRCLE;
                    ENGINE_ASSERT(_shapeJson.contains("size"), "PhysicsShape of type CIRCLE must have field 'size' with a Vec2F. X is the radius of the circle, Y is ignored, Ex: \\\"size\\\": [16, 0]\"")
                    _shapeConfig.size = { _shapeJson["size"][0].get<float>(), 0.f };
                } else if(std::equal(_type.begin(), _type.end(), "polygon")) {
                    _shapeConfig.type = PhysicsShapeType::POLYGON;
                    ENGINE_ASSERT(_shapeJson.contains("vertices") && _shapeJson["vertices"].size() >= 3, "PhysicsShape of type POLYGON must have field 'vertices' with an array of Vec2F and a minimum of 3 vertices, Ex: \"vertices\": [{-5,-5}, {5,-5}, {0,5}")
                    for(auto& _vertex : _shapeJson["vertices"]) {
                        _shapeConfig.vertices.emplace_back(_vertex[0].get<float>(), _vertex[1].get<float>());
                    }
                } else if(std::equal(_type.begin(), _type.end(), "segment")) {
                    _shapeConfig.type = PhysicsShapeType::SEGMENT;
                    if(_shapeJson.contains("vertices") && !_shapeJson["vertices"].empty()) {
                        ENGINE_ASSERT(_shapeJson.contains("vertices") && _shapeJson["vertices"].size() == 2, "PhysicsShape of type SEGMENT must have field 'vertices' with an array of exactly 2 Vec2F, Ex: \"vertices\": [{-5,0}, {5,0}")
                        for(auto& _vertex : _shapeJson["vertices"]) {
                            _shapeConfig.vertices.emplace_back(_vertex[0].get<float>(), _vertex[1].get<float>());
                        }
                    } else {
                        ENGINE_ASSERT(_shapeJson.contains("size"), "PhysicsShape of type SEGMENT must have field 'size' with a Vec2F. X is the length of the segment, Y is ignored, Ex: \\\"size\\\": [16, 0]\"")
                        _shapeConfig.size = { _shapeJson["size"].get<float>(), 0.f };
                    }
                } else {
                    throw std::runtime_error(APPEND_S("Chosen PhysicsShapeType '", _type, "' is not a known type for a PhysicsShape"));
                }

                if(_shapeJson.contains("sensor")) {
                    ENGINE_ASSERT(_shapeJson["sensor"].is_boolean(), "'sensor' is a bool, so it is a single value, Ex: \"sensor\": true")
                    _shapeConfig.sensor = _shapeJson["sensor"].get<bool>();
                }

                if(_shapeJson.contains("friction")) {
                    ENGINE_ASSERT(_shapeJson["friction"].is_number(), "'friction' is a float, so it is a single value, Ex: \"friction\": 0.4")
                    _shapeConfig.friction = _shapeJson["friction"].get<float>();
                }

                if(_shapeJson.contains("restitution")) {
                    ENGINE_ASSERT(_shapeJson["restitution"].is_number(), "'restitution' is a float, so it is a single value, Ex: \"restitution\": 0.2")
                    _shapeConfig.restitution = _shapeJson["restitution"].get<float>();
                }

                if(_shapeJson.contains("mask")) {
                    if(_shapeJson["mask"].is_array()) {
                        int _mask = _shapeJson["mask"][0].get<int>();
                        ENGINE_ASSERT(((_mask & (_mask - 1)) == 0) && _mask >= 0 && _mask <= MAX_MASKS - 1, "'mask' must be a 2^n, where 'n' is the value of the mask between [0,32)")
                        _shapeConfig.shapeMaskingConfig.mask = _mask;
                        for(auto _i = 1; _i < _shapeJson["mask"].size(); _i++) {
                            _mask = _shapeJson["mask"][_i].get<int>();
                            ENGINE_ASSERT(((_mask & (_mask - 1)) == 0) && _mask >= 0 && _mask <= MAX_MASKS - 1, "'mask' must be a 2^n, where 'n' is the value of the mask between [0,32)")
                            _shapeConfig.shapeMaskingConfig.mask |= _mask;
                        }
                    } else {
                        int _mask = _shapeJson["mask"].get<int>();
                        ENGINE_ASSERT(((_mask & (_mask - 1)) == 0) && _mask >= 0 && _mask <= MAX_MASKS - 1, "'mask' must be a 2^n, where 'n' is the value of the mask between [0,32)")
                        _shapeConfig.shapeMaskingConfig.mask = _mask;
                    }
                }

                if(_shapeJson.contains("group")) {
                    if(_shapeJson["group"].is_array()) {
                        int _group = _shapeJson["group"][0].get<int>();
                        ENGINE_ASSERT(((_group & (_group - 1)) == 0) && _group >= 0 && _group <= MAX_MASKS - 1, "'group' must be a 2^n, where 'n' is the value of the group between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                        _shapeConfig.shapeMaskingConfig.group = _group;
                        for(auto _i = 1; _i < _shapeJson["group"].size(); _i++) {
                            _group = _shapeJson["group"][_i].get<int>();
                            ENGINE_ASSERT(((_group & (_group - 1)) == 0) && _group >= 0 && _group <= MAX_MASKS - 1, "'group' must be a 2^n, where 'n' is the value of the group between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                            _shapeConfig.shapeMaskingConfig.group |= _group;
                        }
                    } else {
                        int _group = _shapeJson["group"].get<int>();
                        ENGINE_ASSERT(((_group & (_group - 1)) == 0) && _group >= 0 && _group <= MAX_MASKS - 1, "'group' must be a 2^n, where 'n' is the value of the group between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                        _shapeConfig.shapeMaskingConfig.group = _group;
                    }
                }

                if(_shapeJson.contains("toCollideWith")) {
                    if(_shapeJson["toCollideWith"].is_array()) {
                        int _toCollideWith = _shapeJson["toCollideWith"][0].get<int>();
                        ENGINE_ASSERT(((_toCollideWith & (_toCollideWith - 1)) == 0) && _toCollideWith >= 0 && _toCollideWith <= MAX_MASKS - 1, "'toCollideWith' must be a 2^n, where 'n' is the value of the toCollideWith between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                        _shapeConfig.shapeMaskingConfig.toCollideWith = _toCollideWith;
                        for(auto _i = 1; _i < _shapeJson["toCollideWith"].size(); _i++) {
                            _toCollideWith = _shapeJson["toCollideWith"][_i].get<int>();
                            ENGINE_ASSERT(((_toCollideWith & (_toCollideWith - 1)) == 0) && _toCollideWith >= 0 && _toCollideWith <= MAX_MASKS - 1, "'toCollideWith' must be a 2^n, where 'n' is the value of the toCollideWith between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                            _shapeConfig.shapeMaskingConfig.toCollideWith |= _toCollideWith;
                        }
                    } else {
                        int _toCollideWith = _shapeJson["toCollideWith"].get<int>();
                        ENGINE_ASSERT(((_toCollideWith & (_toCollideWith - 1)) == 0) && _toCollideWith >= 0 && _toCollideWith <= MAX_MASKS - 1, "'toCollideWith' must be a 2^n, where 'n' is the value of the toCollideWith between [0,32). This property can be a single int or an array of int, in this last case all values of the array will be joined by bitwise operations")
                        _shapeConfig.shapeMaskingConfig.toCollideWith = _toCollideWith;
                    }
                }

                _shapeConfigs.push_back(_shapeConfig);
            }
        }

        if(_shapeConfigs.empty()) {
            ShapeConfig _shapeConfig;
            _shapeConfigs.push_back(_shapeConfig);
        }

        _bodyConfig.shapeConfig = _shapeConfigs[0];
        auto* _physicsBody = _node->addComponent<PhysicsBody>(_bodyConfig);

        for(auto _i = 1; _i < _shapeConfigs.size(); _i++) {
            _physicsBody->addShape(_shapeConfigs[_i]);
        }
    }

    void ConfigManager::loadCameraComponent(Node* _node, Scene* _scene, Window* _window, const nlohmann::json& _cameraJson) {
        if(_cameraJson.contains("is_main") && _cameraJson["is_main"].get<bool>()) {

            if(_cameraJson.contains("zoom")) {
                _scene->getMainCamera()->setCurrentZoomLevel(_cameraJson["zoom"].get<float>());
            }

            if(_cameraJson.contains("zoom_speed")) {
                _scene->getMainCamera()->setZoomSpeed(_cameraJson["zoom_speed"].get<float>());
            }

            #if IS_MOBILE()
                _scene->getMainCamera()->setFreeViewport({ _window->getWindowSize().x % 2 != 0 ? _window->getWindowSize().x + 1 : _window->getWindowSize().x,
                                                           _window->getWindowSize().y % 2 != 0 ? _window->getWindowSize().y + 1 : _window->getWindowSize().y});
            #else
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
            #endif

            _scene->getMainCamera()->getViewport()->update(_window->getWindowSize());
        } else {
            auto* _ownerTransform = _node->getTransform();
            auto* _camera = _node->addComponent<Camera>(_window);

            if(_cameraJson.contains("zoom")) {
                _camera->setCurrentZoomLevel(_cameraJson["zoom"].get<float>());
            }

            if(_cameraJson.contains("zoom_speed")) {
                _camera->setZoomSpeed(_cameraJson["zoom_speed"].get<float>());
            }

            #if IS_MOBILE()
                _camera->setFreeViewport({ _window->getWindowSize().x % 2 != 0 ? _window->getWindowSize().x + 1 : _window->getWindowSize().x,
                                                       _window->getWindowSize().y % 2 != 0 ? _window->getWindowSize().y + 1 : _window->getWindowSize().y});
            #else
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
            #endif

            _camera->getViewport()->update(_window->getWindowSize());
            LOG_I("Created camera with viewport of size: ", _window->getWindowSize())
            _scene->getCameras().push_back(_camera);
        }
    }

    void ConfigManager::loadTextRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _textRendererJson) {
        ENGINE_ASSERT(_textRendererJson.contains("font"), "TextRenderer MUST have section 'font'.")
        ENGINE_ASSERT(_textRendererJson.contains("text"), "TextRenderer MUST have section 'text'.")

        auto _fontSize = 54;

        if(_textRendererJson.contains("font_size")) {
            _fontSize = _textRendererJson["font_size"].get<int>();
        }

        auto* _textRenderer = _node->addComponent<TextRenderer>(_textRendererJson["text"].get<std::string>(),
                                                                                         _scene->engine->manager.fontManager.
                                                                                         getSpecificFont(_textRendererJson["font"].get<std::string>(), _fontSize));

        if(_textRendererJson.contains("color")) {
            auto& _color = _textRendererJson["color"];
            _textRenderer->color = Color {_color[0].get<unsigned char>(), _color[1].get<unsigned char>(),
                                          _color[2].get<unsigned char>(), _color[3].get<unsigned char>()};
        }

        if(_textRendererJson.contains("layer")) {
            _textRenderer->layer = _textRendererJson["layer"].get<int>();
        }

        if(_textRendererJson.contains("shader")){
            _textRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(_textRendererJson["shader"].get<std::string>())->getShaderID();
        }
    }

    void ConfigManager::instantiatePrefab(Scene* _scene, const nlohmann::json& _json) {

    }

    void ConfigManager::unloadAssets(Scene* _scene, const nlohmann::json& _sceneJson) {
        ENGINE_ASSERT(_sceneJson.contains("assets"), "Scene MUST have the 'assets' section.")
        auto& _assets = _sceneJson["assets"];

        if(_assets.contains("textures")) {
            auto& _texturesNode = _assets["textures"];
            for (const auto& _texture : _texturesNode) {
                _scene->engine->manager.textureManager.unloadAtlas(Util::getFileNameFromPath(_texture.get<std::string>()));
            }
        }

        if(_assets.contains("fonts")) {
            auto& _fontsNodes = _assets["fonts"];
            for (const auto& _font : _fontsNodes) {
                for(auto _i = 0; _i < _font["sizes"].size(); _i++)
                    _scene->engine->manager.fontManager.unloadFullFont(Util::getFileNameFromPath(_font["path"].get<std::string>()));
            }
        }

        if(_assets.contains("sfx")) {
            auto& _sfxs = _assets["sfx"];
            for (const auto& _sfx : _sfxs) {
                _scene->engine->manager.soundManager.unloadSfx(Util::getFileNameFromPath(_sfx.get<std::string>()));
            }
        }

        if(_assets.contains("music")) {
            auto& _musics = _assets["music"];
            for (const auto& _music : _musics) {
                _scene->engine->manager.soundManager.unloadMusic(Util::getFileNameFromPath(_music.get<std::string>()));
            }
        }
    }

    void ConfigManager::unloadScene(Manager* _manager, Scene* _scene, const std::string& _configFilePath) {
        auto _fileHandler = _manager->fileManager.open(_configFilePath, FileMode::READ);

        auto _content = _manager->fileManager.readFullFile(_fileHandler).content;
        nlohmann::json _sceneJson = nlohmann::json::parse(_content);
        _manager->fileManager.close(_fileHandler);

        unloadAssets(_scene, _sceneJson);
    }

}

