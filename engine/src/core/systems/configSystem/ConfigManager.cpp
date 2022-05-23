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

    const std::string GDE_CONFIG_PATH = "config/project.yaml";

    void ConfigManager::loadGDEConfig(GDEConfig* _config) {
        auto _yaml = YAML::LoadFile(GDE_CONFIG_PATH);
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
        auto _yaml = YAML::LoadFile(_configFilePath);
        loadAssets(_manager, _yaml);
        auto _entityMap = createEntities(_scene, _yaml);
        loadCameras(_entityMap, _scene, _window, _yaml);
        loadSprites(_manager, _entityMap, _scene, _yaml);
        loadTextRenderers(_manager, _entityMap, _scene, _yaml);
        loadBodies(_entityMap, _scene, _yaml);
        _scene->preInit(_manager, _window, _yaml);
    }

    ConfigManager::EntityMap ConfigManager::createEntities(Scene* _scene, const YAML::Node& _yaml) {
        std::unordered_map<int, NodeID> entitiesMap;
        auto& _sceneNode = _yaml["Scene"];
        auto& _entitiesNodes = _sceneNode["Entities"];

        for (const auto& _entity : _entitiesNodes) {
            auto _entityID = _scene->getMainGraph()->createNode(_entity["Tag"].as<std::string>());
            setBaseComponents(_scene, _entityID, _entity);
            entitiesMap[_entity["Ref"].as<int>()] = _entityID;
        }

        return entitiesMap;
    }

    void ConfigManager::loadCameras(const EntityMap& _map, Scene* _scene, Window* _window, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];
        auto& _sceneCameras = _sceneNode["Cameras"];

        for (const auto& _sceneCamera : _sceneCameras) {
            auto _isMain = _sceneCamera["IsMain"].as<bool>();
            if(_isMain) {
                _scene->getMainCamera()->setCurrentZoomLevel(_sceneCamera["Zoom"].as<float>());
                _scene->getMainCamera()->setZoomSpeed(_sceneCamera["ZoomSpeed"].as<float>());

                int _viewPortType = _sceneCamera["ViewPortType"].as<int>();
                switch (_viewPortType) {
                    case 0 : _scene->getMainCamera()->setAdaptiveViewport(_window->getWindowSize(), _window->getWindowSize()); break;
                    case 1 : _scene->getMainCamera()->setFreeViewport(_window); break;
                    default: _scene->getMainCamera()->setFreeViewport(_window);
                }

                _scene->getMainCamera()->getViewport()->update(_window->getWindowSize());
            } else {
                auto _ownerEntityID = _map.at(_sceneCamera["Owner"].as<int>());
                auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
                auto* _camera = _scene->getMainGraph()->addComponent<Camera>(_ownerEntityID, _window, _ownerTransform, _ownerEntityID);

                // Set the zoom properties
                _camera->setCurrentZoomLevel(_sceneCamera["Zoom"].as<float>());
                _camera->setZoomSpeed(_sceneCamera["ZoomSpeed"].as<float>());

                // Set the proper viewport
                int _viewPortType = _sceneCamera["ViewPortType"].as<int>();
                switch (_viewPortType) {
                    case 0 : _camera->setAdaptiveViewport(_window->getWindowSize(), _window->getWindowSize()); break;
                    case 1 : _camera->setFreeViewport(_window); break;
                    default: _camera->setFreeViewport(_window);
                }

                _camera->getViewport()->update(_window->getWindowSize());
                _scene->getCameras().push_back(_camera);
            }
        }
    }

    void ConfigManager::loadSprites(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];
        auto& _spriteRenderersNode = _sceneNode["SpriteRenderers"];

        for (const auto& _spriteRendererNode : _spriteRenderersNode) {
            auto _ownerEntityID = _map.at(_spriteRendererNode["Owner"].as<int>());
            auto* _spriteRenderer = _scene->getMainGraph()->addComponent<SpriteRenderer>(_ownerEntityID, _scene);

            _spriteRenderer->setTexture(_manager->textureManager.getTile(_spriteRendererNode["Texture"]["Atlas"].as<std::string>(),
                                                                          _spriteRendererNode["Texture"]["Tile"].as<std::string>()));
            _spriteRenderer->color = Color { _spriteRendererNode["Color"][0].as<unsigned char>(), _spriteRendererNode["Color"][1].as<unsigned char>(),
                                             _spriteRendererNode["Color"][2].as<unsigned char>(), _spriteRendererNode["Color"][3].as<unsigned char>()};
            _spriteRenderer->layer = _spriteRendererNode["Layer"].as<int>();
            _spriteRenderer->shaderID = _manager->shaderManager.getShader(_spriteRendererNode["Shader"].as<std::string>());
        }
    }

    void ConfigManager::loadTextRenderers(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];
        auto& _textsRendererNodes = _sceneNode["TextRenderers"];

        for (const auto& _textRendererNode : _textsRendererNodes) {
            auto _ownerEntityID = _map.at(_textRendererNode["Owner"].as<int>());
            auto* _textRenderer = _scene->getMainGraph()->addComponent<TextRenderer>(_ownerEntityID,
                         _manager, _manager->fontManager.getDefaultFont(_textRendererNode["Font"]["Name"].as<std::string>()),
                         _textRendererNode["Text"].as<std::string>());

            _textRenderer->color = Color {_textRendererNode["Color"][0].as<unsigned char>(), _textRendererNode["Color"][1].as<unsigned char>(),
                                          _textRendererNode["Color"][2].as<unsigned char>(), _textRendererNode["Color"][3].as<unsigned char>()};
            _textRenderer->layer = _textRendererNode["Layer"].as<int>();
            _textRenderer->shaderID = _manager->shaderManager.getShader(_textRendererNode["Shader"].as<std::string>());
        }
    }

    void ConfigManager::loadBodies(const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];
        auto& _bodiesNodes = _sceneNode["Bodies"];

        for (const auto& _bodyNode : _bodiesNodes) {
            auto _ownerEntityID = _map.at(_bodyNode["Owner"].as<int>());

            BodyType _bodyType;
            switch (_bodyNode["BodyType"].as<int>()) {
                case 0: _bodyType = BodyType::STATIC; break;
                case 1: _bodyType = BodyType::DYNAMIC; break;
                case 2: _bodyType = BodyType::KINEMATIC; break;
                default: _bodyType = BodyType::STATIC;
            }

            BodyShapeType _bodyShapeType;
            switch (_bodyNode["BodyShape"].as<int>()) {
                case 0: _bodyShapeType = BodyShapeType::BOX; break;
                case 1: _bodyShapeType = BodyShapeType::CIRCLE; break;
                case 2: _bodyShapeType = BodyShapeType::POLYGON; break;
                default: _bodyShapeType = BodyShapeType::BOX;
            }

            BodyConfig _bodyConfig {
                .mass = _bodyNode["Mass"].as<float>(),
                .size = {_bodyNode["Size"]["Width"].as<float>(), _bodyNode["Size"]["Height"].as<float>()},
                .friction = _bodyNode["Friction"].as<float>(),
                .restitution = _bodyNode["Restitution"].as<float>(),
                .mask = _bodyNode["Mask"].as<unsigned long>(),
                .bodyType = _bodyType,
                .bodyShapeType = _bodyShapeType
            };

            auto* _ownerTransform = _scene->getMainGraph()->getComponent<Transform>(_ownerEntityID);
            _scene->getMainGraph()->addComponent<Body>(_ownerEntityID, _bodyConfig, _ownerTransform);
        }
    }

    void ConfigManager::loadAssets(Manager* _manager, const YAML::Node& _yaml) {
        auto& _sceneNode = _yaml["Scene"];

        auto& _texturesNode = _sceneNode["Assets"]["Textures"];
        for (const auto& _texture : _texturesNode) {
            _manager->textureManager.loadAtlas(_texture["TileWidth"].as<int>(), _texture["TileHeight"].as<int>(),
                                               _texture["Path"].as<std::string>());
        }

        auto& _fontsNodes = _sceneNode["Assets"]["Fonts"];
        for (const auto& _font : _fontsNodes) {
            for(auto _i = 0; _i < _font["Sizes"].size(); _i++)
                _manager->fontManager.loadFont(_font["Path"].as<std::string>(), _font["Sizes"][_i].as<int>());
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

        _scene->getMainGraph()->getComponent<Active>(_nodeID)->active = _yamlNode["Active"].as<bool>();
    }

}

