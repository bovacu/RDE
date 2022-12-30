// Created by borja on 31/3/22.

#include "core/graph/components/ParticleSystem.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Functions.h"
#include "core/Engine.h"
#include "core/graph/components/Node.h"

namespace RDE {

    ParticleSystem::ParticleSystem(Node* _node, Scene* _scene, const ParticleSystemConfig& _particleSystemConfig) :
    ParticleSystem(_node, &_scene->engine->manager, _scene->getMainGraph(), _particleSystemConfig) {  }

    ParticleSystem::ParticleSystem(Node* _node, Manager* _manager, Graph* _graph, const ParticleSystemConfig& _particleSystemConfig) {

        RENDERIZABLE_BASIC_PROPERTIES_INITIALIZATION(50, SPRITE_RENDERER_SHADER, BatchPriority::SpritePriority)

        UDelegate<ParticleData()> _allocator;
        _allocator.bind<&ParticleSystem::allocator>(this);
        particleSystemConfig = _particleSystemConfig;
        pool.init(_allocator, particleSystemConfig.dataConfig.numberOfParticles);

        if(particleSystemConfig.callbacksConfig.effectFunction == nullptr) {
            particleSystemConfig.callbacksConfig.effectFunction.bind<&ParticleSystem::defaultEffect>(this);
        }

        if(particleSystemConfig.callbacksConfig.colorInterpolationFunction == nullptr) {
            particleSystemConfig.callbacksConfig.colorInterpolationFunction.bind<&ParticleSystem::defaultColorInterpolationFunction>(this);
        }

        if(particleSystemConfig.dataConfig.texture == nullptr) {
            particleSystemConfig.dataConfig.texture = _manager->textureManager.getSubTexture("defaultAssets", "handle");
        }
    }

    RENDERIZABLE_BASIC_METHODS_IMPL(ParticleSystem, particleSystemConfig.dataConfig.texture->getSize().x, particleSystemConfig.dataConfig.texture->getSize().y)

    void ParticleData::reset(const ParticleSystemConfig& _particleSystemConfig, Transform* _parentTransform) {
        auto _parentPos = _parentTransform->getPosition();
        position = glm::vec3 { _parentPos.x, _parentPos.y, 0.f };
        Random _random;
        velocity = glm::vec2 { _random.randomf(-20, 20), _random.randomf(20, 100) };
        life = _particleSystemConfig.dataConfig.lifeTime;
        acceleration = glm::vec2 { };
        color = _particleSystemConfig.colorGradientConfig.initColor;
    }

    void ParticleSystem::update(Delta _dt) {
        static float _timer = 0.0f;
        for (auto _it = usedParticles.begin(); _it != usedParticles.end(); ++_it) {

            _it->life -= _dt;
            if (_it->life < 0) {
                _it->reset(particleSystemConfig, node->getTransform());
                auto _particle = _it;
                pool.returnElement(&(*_it));
                usedParticles.erase(_it);
                continue;
            }

            if(!isPlaying) continue;

            particleSystemConfig.callbacksConfig.effectFunction(*_it, _dt, particleSystemConfig);
            _it->color = particleSystemConfig.callbacksConfig.colorInterpolationFunction(*_it, _dt, particleSystemConfig);
        }

        if(!isPlaying) return;

        if(_timer > particleSystemConfig.dataConfig.timeToCreateNewParticleMs) {
            ParticleData* _particle = pool.getElement();
            usedParticles.push_back(*_particle);
            _timer = 0.0f;
        }

        _timer += _dt.getMilliseconds();
    }

    ParticleData ParticleSystem::allocator() {
        auto _position = node->getTransform()->getPosition();
        Random _random;

        auto _particle = ParticleData {
                            glm::vec3 { _position.x, _position.y, 0.f },
                            glm::vec2 { _random.randomf(-20, 20), _random.randomf(20, 100) },
                            glm::vec2 {  },
                            particleSystemConfig.colorGradientConfig.initColor,
                            particleSystemConfig.dataConfig.lifeTime
                        };

        return _particle;
    }

    void ParticleSystem::defaultEffect(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config) {
        _particleData.velocity += glm::vec2(_particleData.velocity.x, _particleData.velocity.y) * (float)_dt * 0.5f;
        _particleData.position += glm::vec3 { _particleData.velocity.x, _particleData.velocity.y, 0.f } * (float)_dt;
    }

    void ParticleSystem::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform* _transform, const ViewPort* _viewport) {
        for (auto& _particle : usedParticles) {
            auto _vertexCount = _vertices.size();

            auto [_transformMat, _dirty] = _transform->localToWorld();
            auto _screenPos = Util::Math::worldToScreenCoords(*_viewport, {_particle.position.x, _particle.position.y});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;

            Vec2F _textureOrigin = {(float)particleSystemConfig.dataConfig.texture->getRegion().bottomLeftCorner.x, (float)particleSystemConfig.dataConfig.texture->getRegion().bottomLeftCorner.y};
            Vec2F _textureOriginNorm = {_textureOrigin.x / (float)particleSystemConfig.dataConfig.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)particleSystemConfig.dataConfig.texture->getSpriteSheetSize().y};

            Vec2F _textureTileSize = {(float)particleSystemConfig.dataConfig.texture->getRegion().size.x, (float)particleSystemConfig.dataConfig.texture->getRegion().size.y};
            Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)particleSystemConfig.dataConfig.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)particleSystemConfig.dataConfig.texture->getSpriteSheetSize().y};
            auto _textureTileSizeOnScreen = Util::Math::worldToScreenSize(*_viewport, _textureTileSize);

            glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

            glm::vec4 _color = { (float)_particle.color.r / 255.f, (float)_particle.color.g / 255.f,(float)_particle.color.b / 255.f, (float)_particle.color.a / 255.f };

            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });

            _indices.emplace_back(_vertexCount + 0);
            _indices.emplace_back(_vertexCount + 1);
            _indices.emplace_back(_vertexCount + 2);

            _indices.emplace_back(_vertexCount + 2);
            _indices.emplace_back(_vertexCount + 3);
            _indices.emplace_back(_vertexCount + 0);
        }
    }

    Color ParticleSystem::defaultColorInterpolationFunction(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config) {
        if(_config.colorGradientConfig.endColor == Color::NO_COLOR) return _particleData.color;

        auto _percentage = _particleData.life < _config.dataConfig.lifeTime / 1.25f ? (1 - _particleData.life / _config.dataConfig.lifeTime) : 0.f;
        auto _red   = Util::Math::clamp(_particleData.color.r + (unsigned char )(_percentage * _dt * std::abs(_config.colorGradientConfig.endColor.r - _particleData.color.r)), 0, 255);
        auto _green = Util::Math::clamp(_particleData.color.g + (unsigned char )(_percentage * _dt * std::abs(_config.colorGradientConfig.endColor.g - _particleData.color.g)), 0, 255);
        auto _blue  = Util::Math::clamp(_particleData.color.b + (unsigned char )(_percentage * _dt * std::abs(_config.colorGradientConfig.endColor.b - _particleData.color.b)), 0, 255);
        auto _alpha = Util::Math::clamp(_particleData.color.a + (unsigned char )(_percentage * _dt * std::abs(_config.colorGradientConfig.endColor.a - _particleData.color.a)), 0, 255);
        return Color { (unsigned char)_red, (unsigned char)_green, (unsigned char)_blue, (unsigned char)_alpha };
    }

    void ParticleSystem::play() {
        isPlaying = true;
    }

    void ParticleSystem::pause() {
        isPlaying = false;
    }

    void ParticleSystem::stop() {
        isPlaying = false;
        reset();
    }

    void ParticleSystem::reset() {
        for (auto _it = usedParticles.begin(); _it != usedParticles.end(); ++_it) {
            _it->reset(particleSystemConfig, node->getTransform());
            pool.returnElement(&(*_it));
            usedParticles.erase(_it);
        }
    }
}
