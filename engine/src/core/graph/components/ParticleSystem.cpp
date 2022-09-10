// Created by borja on 31/3/22.

#include "core/graph/components/ParticleSystem.h"
#include "core/util/Functions.h"

namespace GDE {

    void ParticleData::reset(const ParticleSystemConfig& _particleSystemConfig, Transform* _parentTransform) {
        auto _parentPos = _parentTransform->getPositionWorld();
        position = glm::vec3 { _parentPos.x, _parentPos.y, 0.f };
        Random _random;
        speed = glm::vec2 { _random.randomf(-20, 20), _random.randomf(20, 100) };
        life = _particleSystemConfig.lifeTime;
        velocity = _particleSystemConfig.velocity;
        acceleration = _particleSystemConfig.acceleration;
        color = _particleSystemConfig.initColor;
    }



    ParticleSystem::ParticleSystem(const GDE::NodeID& _nodeID, Scene* _scene, const ParticleSystemConfig& _particleSystemConfig) {
        transform = _scene->getMainGraph()->getComponent<Transform>(_nodeID);
        UDelegate<ParticleData()> _allocator;
        _allocator.bind<&ParticleSystem::allocator>(this);
        particleSystemConfig = _particleSystemConfig;
        pool.init(_allocator, particleSystemConfig.numberOfParticles);

        if(particleSystemConfig.effectFunction == nullptr) {
            particleSystemConfig.effectFunction.bind<&ParticleSystem::defaultEffect>(this);
        }

        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        IRenderizable::batchPriority = BatchPriority::SpritePriority;

        ENGINE_ASSERT(particleSystemConfig.texture != nullptr, "Tried to use a particle system without specifying a texture!!!")
    }

    void ParticleSystem::update(Delta _dt) {
        static float _timer = 0.0f;

        for (auto _it = usedParticles.begin(); _it != usedParticles.end(); ++_it) {

            _it->life -= _dt;
            if (_it->life < 0) {
                _it->reset(particleSystemConfig, transform);
                pool.returnElement(_it.base());
                usedParticles.erase(_it);
                continue;
            }

            particleSystemConfig.effectFunction(*_it, _dt, particleSystemConfig);
        }

        if(_timer > particleSystemConfig.timeToCreateNewParticleMs) {
            ParticleData* _particle = pool.getElement();
            usedParticles.push_back(*_particle);
            _timer = 0.0f;
        }

        _timer += _dt.GetMilliseconds();
    }

    ParticleData ParticleSystem::allocator() {
        auto _position = transform->getPositionWorld();
        Random _random;
        auto _particle = ParticleData { glm::vec3 { _position.x, _position.y, 0.f }, glm::vec2 { _random.randomf(-20, 20), _random.randomf(20, 100) }, particleSystemConfig.initColor, particleSystemConfig.lifeTime, particleSystemConfig.velocity, particleSystemConfig.acceleration };
        return _particle;
    }

    void ParticleSystem::defaultEffect(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config) {
        _particleData.speed += glm::vec2(_particleData.speed.x, _particleData.speed.y) * (float)_dt * 0.5f;
        _particleData.position += glm::vec3 { _particleData.speed.x, _particleData.speed.y, 0.f } * (float)_dt;
        auto _percentage = _particleData.life < _config.lifeTime / 1.5f ? _particleData.life / _config.lifeTime : 1.f;
        _particleData.color = Color {_particleData.color.r, _particleData.color.g, _particleData.color.b, (unsigned char )(255 * _percentage) };
    }

    void ParticleSystem::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const {
        for (auto& _particle : usedParticles) {
            auto _vertexCount = _vertices.size();

            auto _transformMat = _transform.modelMatrix;
            auto _screenPos = Util::worldToScreenCoords(_viewport, {_particle.position.x, _particle.position.y});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;

            Vec2F _textureOrigin = {(float)particleSystemConfig.texture->getRegion().bottomLeftCorner.x, (float)particleSystemConfig.texture->getRegion().bottomLeftCorner.y};
            Vec2F _textureOriginNorm = {_textureOrigin.x / (float)particleSystemConfig.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)particleSystemConfig.texture->getSpriteSheetSize().y};

            Vec2F _textureTileSize = {(float)particleSystemConfig.texture->getRegion().size.x, (float)particleSystemConfig.texture->getRegion().size.y};
            Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)particleSystemConfig.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)particleSystemConfig.texture->getSpriteSheetSize().y};
            auto _textureTileSizeOnScreen = Util::worldToScreenSize(_viewport, _textureTileSize);

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
}
