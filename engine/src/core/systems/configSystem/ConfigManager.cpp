//
// Created by borja on 15/05/22.
//
#include "core/systems/configSystem/ConfigManager.h"
#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/render/window/Window.h"
#include "core/render/elements/TextureAtlasManager.h"
#include "core/Engine.h"

namespace GDE {

    void ConfigManager::loadGDEConfig(GDEConfig* _config, FileManager& _manager) {

        auto _configPath = "assets/config/desktop.yaml";
        #if ANDROID
        _configPath = "assets/config/android.yaml";
        #endif
        auto _fileHandler = _manager.open(_configPath, FileMode::READ);
        auto _yaml = YAML::Load(_manager.readFullFile(_fileHandler).content);
        _manager.close(_fileHandler);
        auto _configurationNode = _yaml["Configuration"];

        _config->windowData.title = _configurationNode["Title"].as<std::string>(),
        _config->windowData.fullScreen = _configurationNode["Fullscreen"].as<bool>(),
        _config->windowData.vsync = _configurationNode["VSync"].as<bool>(),
        _config->windowData.size = Vec2<unsigned int>{_configurationNode["Resolution"]["Width"].as<unsigned int>(),
                                                      _configurationNode["Resolution"]["Height"].as<unsigned int>()};

        _config->projectData.iconPath = _configurationNode["Icon"].as<std::string>();
        _config->projectData.mainSceneToLoad = _configurationNode["MainScene"].as<std::string>();
    }

    void ConfigManager::loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath) {
        auto _fileHandler = _manager->fileManager.open(_configFilePath, FileMode::READ);
        auto _yaml = YAML::Load(_manager->fileManager.readFullFile(_fileHandler).content);
        _manager->fileManager.close(_fileHandler);
        loadAssets(_scene, _window, _yaml);
        auto _entityMap = createEntities(_scene, _yaml);
        parentingEntities(_entityMap, _scene, _yaml);
        loadCameras(_entityMap, _scene, _window, _yaml["Scene"]);
        loadSprites(_manager, _entityMap, _scene, _yaml["Scene"]);
        loadTextRenderers(_manager, _entityMap, _scene, _yaml["Scene"]);
        loadBodies(_entityMap, _scene, _yaml["Scene"]);
        _scene->preInit(_manager, _window, _yaml);
    }

    void ConfigManager::loadAssets(Scene* _scene, Window* _window, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];

        auto& _texturesNode = _sceneNode["Assets"]["Textures"];
        for (const auto& _texture : _texturesNode) {
            auto _fileHandler = _scene->engine->manager.fileManager.open(_texture["Path"].as<std::string>(), FileMode::READ);
            _scene->engine->manager.textureManager.loadSpriteSheet(YAML::Load(_scene->engine->manager.fileManager.readFullFile(_fileHandler).content));
            _scene->engine->manager.fileManager.close(_fileHandler);
        }

        auto& _fontsNodes = _sceneNode["Assets"]["Fonts"];
        for (const auto& _font : _fontsNodes) {
            for(auto _i = 0; _i < _font["Sizes"].size(); _i++)
                _scene->engine->manager.fontManager.loadFont(_scene->engine->manager.fileManager, _font["Path"].as<std::string>(), _font["Sizes"][_i].as<int>());
        }

        auto& _prefabsNodes = _sceneNode["Assets"]["Prefabs"];
        for (const auto& _prefab : _prefabsNodes) {
            auto _fileHandler = _scene->engine->manager.fileManager.open(APPEND_S("assets/prefabs/", _prefab["Key"].as<std::string>(), ".yaml"), FileMode::READ);
            auto _p = YAML::Load(_scene->engine->manager.fileManager.readFullFile(_fileHandler).content)["Prefab"];
            loadPrefab(_scene, _window, _p);
            _scene->engine->manager.fileManager.close(_fileHandler);
        }
    }

    void ConfigManager::loadPrefab(Scene* _scene, Window* _window, YAML::Node& _yaml) {
        auto _entityID = _scene->getMainGraph()->createNode(_yaml["Base"]["Tag"].as<std::string>());
        _scene->getMainGraph()->setNodeActive(_entityID, false);
        _scene->prefabs[_yaml["Name"].as<std::string>()] = _entityID;

        EntityMap _entitiesMap;
        _entitiesMap[0] = _entityID;

        setBaseComponents(_scene, _entityID, _yaml["Base"]);

        if(_yaml["Camera"].IsDefined()) {
            _yaml["Camera"]["Ref"] = 0;
            loadCamera(_entityID, _scene, _window, _yaml["Camera"]);
        }

        if(_yaml["SpriteRenderer"].IsDefined()) {
            _yaml["SpriteRenderer"]["Ref"] = 0;
            loadSpriteRenderer(_entityID, _scene, _yaml["SpriteRenderer"]);
        }

        if(_yaml["TextRenderer"].IsDefined()) {
            _yaml["TextRenderer"]["Ref"] = 0;
            loadTextRenderer(_entityID, _scene, _yaml["TextRenderer"]);
        }

        if(_yaml["Body"].IsDefined()) {
            _yaml["Body"]["Ref"] = 0;
            loadBody(_entityID, _scene, _yaml["Body"]);
        }
    }

    ConfigManager::EntityMap ConfigManager::createEntities(Scene* _scene, const YAML::Node& _yaml) {
        std::unordered_map<int, NodeID> entitiesMap;
        auto& _sceneNode = _yaml["Scene"];
        auto& _entitiesNodes = _sceneNode["Entities"];

        for (const auto& _entity : _entitiesNodes) {
            if(_entity["Prefab"].IsDefined()) {
                // TODO this _yaml is probably not what we want to send
                instantiatePrefab(_scene, _yaml);
                continue;
            }

            auto _entityID = _scene->getMainGraph()->createNode(_entity["Tag"].as<std::string>());
            setBaseComponents(_scene, _entityID, _entity);
            entitiesMap[_entity["Ref"].as<int>()] = _entityID;
        }

        return entitiesMap;
    }

    void ConfigManager::instantiatePrefab(Scene* _scene, const YAML::Node& _yaml) {

    }

    void ConfigManager::parentingEntities(const ConfigManager::EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];
        auto& _entitiesNodes = _sceneNode["Entities"];

        for (const auto& _entity : _entitiesNodes) {
            if(!_entity["Parent"].IsDefined()) continue;
            _scene->getMainGraph()->setParent(_map.at(_entity["Ref"].as<int>()), _map.at(_entity["Parent"].as<int>()));
        }
    }

    void ConfigManager::loadCameras(const EntityMap& _map, Scene* _scene, Window* _window, const YAML::Node& _yaml) {
        auto& _sceneCameras = _yaml["Cameras"];

        for (const auto& _sceneCamera : _sceneCameras) {
            auto _nodeID = !_sceneCamera["Ref"].IsDefined() ? (NodeID)0 : _map.at(_sceneCamera["Ref"].as<int>());
            loadCamera(_nodeID, _scene, _window, _sceneCamera);
        }
    }

    void ConfigManager::loadSprites(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _spriteRenderersNode = _yaml["SpriteRenderers"];

        for (const auto& _spriteRendererNode : _spriteRenderersNode) {
            loadSpriteRenderer(_map.at(_spriteRendererNode["Ref"].as<int>()), _scene, _spriteRendererNode);
        }
    }

    void ConfigManager::loadTextRenderers(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _textsRendererNodes = _yaml["TextRenderers"];

        for (const auto& _textRendererNode : _textsRendererNodes) {
            loadTextRenderer(_map.at(_textRendererNode["Ref"].as<int>()), _scene, _textRendererNode);
        }
    }

    void ConfigManager::loadBodies(const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _bodiesNodes = _yaml["Bodies"];

        for (const auto& _bodyNode : _bodiesNodes) {
            loadBody(_map.at(_bodyNode["Ref"].as<int>()), _scene, _bodyNode);
        }
    }

    void ConfigManager::setBaseComponents(Scene* _scene, const NodeID& _nodeID, const YAML::Node& _yamlNode) {
        auto* _nodeTransform = _scene->getMainGraph()->getComponent<Transform>(_nodeID);
        auto& _positionNode = _yamlNode["Transform"]["Position"];
        auto& _scaleNode = _yamlNode["Transform"]["Scale"];
        auto& _rotationNode = _yamlNode["Transform"]["Rotation"];

        _nodeTransform->setPosition(_positionNode[0].as<float>(), _positionNode[1].as<float>());
        _nodeTransform->setScale(_scaleNode[0].as<float>(), _scaleNode[1].as<float>());
        _nodeTransform->setRotation(_rotationNode.as<float>());

        if(!_yamlNode["Active"].as<bool>())
            _scene->getMainGraph()->removeComponent<Active>(_nodeID);
    }

    void ConfigManager::loadCamera(const NodeID& _nodeID, Scene* _scene, Window* _window, const YAML::Node& _yaml) {
        auto _isMain = _yaml["IsMain"].as<bool>();
        if(_isMain) {
            _scene->getMainCamera()->setCurrentZoomLevel(_yaml["Zoom"].as<float>());
            _scene->getMainCamera()->setZoomSpeed(_yaml["ZoomSpeed"].as<float>());

            int _viewPortType = _yaml["ViewPortType"].as<int>();
            switch (_viewPortType) {
                case 0 : {
                    auto _virtualRes = _yaml["ViewPortVirtualResolution"].IsDefined() && _yaml["ViewPortVirtualResolution"].size() > 0 ?
                                       Vec2I{_yaml["ViewPortVirtualResolution"][0].as<int>(), _yaml["ViewPortVirtualResolution"][1].as<int>()} :
                                       _window->getWindowSize();

                    auto _deviceRes = _yaml["ViewPortDeviceResolution"].IsDefined() && _yaml["ViewPortDeviceResolution"].size() > 0 ?
                                      Vec2I{_yaml["ViewPortDeviceResolution"][0].as<int>(), _yaml["ViewPortDeviceResolution"][1].as<int>()} :
                                      _window->getWindowSize();

                    _scene->getMainCamera()->setAdaptiveViewport(_virtualRes, _deviceRes);
                    break;
                }
                case 1 : _scene->getMainCamera()->setFreeViewport(_window->getWindowSize()); break;
                default: _scene->getMainCamera()->setFreeViewport(_window->getWindowSize());
            }

            _scene->getMainCamera()->getViewport()->update(_window->getWindowSize());
        } else {
            auto _ownerEntityID = _nodeID;
            auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
            auto* _camera = _scene->getMainGraph()->addComponent<Camera>(_ownerEntityID, _window, _ownerTransform);

            // Set the zoom properties
            _camera->setCurrentZoomLevel(_yaml["Zoom"].as<float>());
            _camera->setZoomSpeed(_yaml["ZoomSpeed"].as<float>());

            // Set the proper viewport
            int _viewPortType = _yaml["ViewPortType"].as<int>();
            switch (_viewPortType) {
                case 0 : _camera->setAdaptiveViewport(_window->getWindowSize(), _window->getWindowSize()); break;
                case 1 : _camera->setFreeViewport(_window->getWindowSize()); break;
                default: _camera->setFreeViewport(_window->getWindowSize());
            }

            _camera->getViewport()->update(_window->getWindowSize());
            _scene->getCameras().push_back(_camera);
        }
    }

    void ConfigManager::loadSpriteRenderer(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml) {
        auto _ownerEntityID = _nodeID;
        auto* _spriteRenderer = _scene->getMainGraph()->addComponent<SpriteRenderer>(_ownerEntityID, _scene);

        _spriteRenderer->setTexture(
                _scene->engine->manager.textureManager.getSubTexture(_yaml["Texture"]["Atlas"].as<std::string>(),
                                                       _yaml["Texture"]["Tile"].as<std::string>()));
        _spriteRenderer->color = Color { _yaml["Color"][0].as<unsigned char>(), _yaml["Color"][1].as<unsigned char>(),
                                         _yaml["Color"][2].as<unsigned char>(), _yaml["Color"][3].as<unsigned char>()};
        _spriteRenderer->layer = _yaml["Layer"].as<int>();
        _spriteRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(_yaml["Shader"].as<std::string>());
    }

    void ConfigManager::loadTextRenderer(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml) {
        auto _ownerEntityID = _nodeID;
        auto* _textRenderer = _scene->getMainGraph()->addComponent<TextRenderer>(_ownerEntityID,
                                                                                 _scene, _scene->engine->manager.fontManager.getDefaultFont(_yaml["Font"]["Name"].as<std::string>()),
                                                                                 _yaml["Text"].as<std::string>());

        _textRenderer->color = Color {_yaml["Color"][0].as<unsigned char>(), _yaml["Color"][1].as<unsigned char>(),
                                      _yaml["Color"][2].as<unsigned char>(), _yaml["Color"][3].as<unsigned char>()};
        _textRenderer->layer = _yaml["Layer"].as<int>();
        _textRenderer->shaderID = _scene->engine->manager.shaderManager.getShader(_yaml["Shader"].as<std::string>());
    }

    void ConfigManager::loadBody(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml) {
        auto _ownerEntityID = _nodeID;

        BodyType _bodyType;
        switch (_yaml["BodyType"].as<int>()) {
            case 0: _bodyType = BodyType::STATIC; break;
            case 1: _bodyType = BodyType::DYNAMIC; break;
            case 2: _bodyType = BodyType::KINEMATIC; break;
            default: _bodyType = BodyType::STATIC;
        }

        BodyShapeType _bodyShapeType;
        switch (_yaml["BodyShape"].as<int>()) {
            case 0: _bodyShapeType = BodyShapeType::BOX; break;
            case 1: _bodyShapeType = BodyShapeType::CIRCLE; break;
            case 2: _bodyShapeType = BodyShapeType::POLYGON; break;
            default: _bodyShapeType = BodyShapeType::BOX;
        }

        BodyConfig _bodyConfig {
                .mass = _yaml["Mass"].as<float>(),
                .size = {_yaml["Size"]["Width"].as<float>(), _yaml["Size"]["Height"].as<float>()},
                .friction = _yaml["Friction"].as<float>(),
                .restitution = _yaml["Restitution"].as<float>(),
                .mask = _yaml["Mask"].as<unsigned long>(),
                .bodyType = _bodyType,
                .bodyShapeType = _bodyShapeType
        };

        auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
        _scene->getMainGraph()->addComponent<Body>(_ownerEntityID, _scene, _bodyConfig, _ownerTransform);
    }

}

