#include "core/render/elements/SpriteBatch.h"
#include "core/util/Logger.h"
#include "core/util/GLUtil.h"

#if IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#else
    #include "glad/glad.h"
#endif

#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "glm/gtc/type_ptr.hpp"

namespace GDE {

    void SpriteBatch::init(Engine* _engine) {
        shaderManager = &_engine->manager.shaderManager;
        configBasicShader();
    }

    void SpriteBatch::Debug::init(SpriteBatch* _batch) {
        batch = _batch;
        configDebugShader();
    }

    void SpriteBatch::configBasicShader() {
        GLsizei _structSize = sizeof(OpenGLVertex);
        shaderManager->loadShaderVertexConfig(SPRITE_RENDERER_SHADER, {
            VertexConfig {
                0, 3, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
                1, 4, GL_FLOAT, 4 * 3, _structSize
            },
            VertexConfig {
                2, 2, GL_FLOAT, 4 * 3 + 4 * 4, _structSize
            }
        },
        maxIndicesPerDrawCall);

        shaderManager->loadShaderVertexConfig(TEXT_RENDERER_SHADER, {
            VertexConfig {
                  0, 3, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
                  1, 4, GL_FLOAT, 4 * 3, _structSize
            },
            VertexConfig {
                  2, 2, GL_FLOAT, 4 * 3 + 4 * 4, _structSize
            }
        },
        maxIndicesPerDrawCall);
        CHECK_GL_ERROR("SpriteBatch configBasicShader")
    }

    void SpriteBatch::Debug::configDebugShader() {
        GLsizei _structSize = 3 * sizeof(float) + 4 * sizeof(float);
        Debug::batch->shaderManager->loadShaderVertexConfig(DEBUG_SHADER, {
            VertexConfig {
                  0, 3, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
                  1, 4, GL_FLOAT, 3 * sizeof(float), _structSize
            }
        },
          Debug::batch->maxIndicesPerDrawCall);
        CHECK_GL_ERROR("SpriteBatch configDebugShader")
    }

    SpriteBatch::~SpriteBatch() {};

    void SpriteBatch::beginDraw(Camera& _camera, Transform* _cameraTransform) {
        viewProjectionMatrix = _camera.getProjectionMatrix() * glm::inverse(_cameraTransform->modelMatrix);
        viewport = _camera.getViewport();
    }

    void SpriteBatch::Debug::drawPoint(const Vec2F& _position, const Color& _color) {
        if(vertexDebugBufferPoints.size() > 50000)
            flushDebug();

        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};
        auto _screenPos = Util::worldToScreenCoords(*batch->viewport, _position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x, _screenPos.y, 1.f));
        auto _scalingFactor = batch->viewport->getScalingFactor();
        if(_scalingFactor != 1) {
            _transformMat *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.x, 1.f});
        }
        vertexDebugBufferPoints.emplace_back(_transformMat * glm::vec4 {_screenPos.x, _screenPos.y, 0.0f, 1.0f}, _colorVec4);
    }

    void SpriteBatch::Debug::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};
        auto _screenPos0 = Util::worldToScreenCoords(*batch->viewport, _p0);
        auto _screenPos1 = Util::worldToScreenCoords(*batch->viewport, _p1);

        auto _transformMat0 = glm::mat4(1.f);
        auto _transformMat1 = glm::mat4(1.f);

        _transformMat0[3][0] = _screenPos0.x;
        _transformMat0[3][1] = _screenPos0.y;

        _transformMat1[3][0] = _screenPos1.x;
        _transformMat1[3][1] = _screenPos1.y;

        auto _scalingFactor = batch->viewport->getScalingFactor();
        if(_scalingFactor != 1) {
            _transformMat0 *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.y, 1.f});
            _transformMat1 *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.y, 1.f});
        }

        vertexDebugBufferLines.emplace_back(glm::vec3 {_transformMat0[3][0], _transformMat0[3][1], 1.0f}, _colorVec4);
        vertexDebugBufferLines.emplace_back(glm::vec3 {_transformMat1[3][0], _transformMat1[3][1], 1.0f}, _colorVec4);
    }

    void SpriteBatch::Debug::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        auto _screenPos = Util::worldToScreenCoords(*batch->viewport, _position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x,_screenPos.y, 1.f));

        if(_rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });

        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};

        auto _screenSize = Util::worldToScreenSize(*batch->viewport, _size);
        // First triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);

        // Second triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
    }

    void SpriteBatch::Debug::drawShape(DebugShape& _shape) {
        auto _scalingFactor = batch->viewport->getScalingFactor();
        auto _transformMat = glm::translate(glm::mat4(1.f), glm::vec3 (_shape.getPosition().x * _scalingFactor.x, _shape.getPosition().y * _scalingFactor.y, 1.f));

        if(_shape.rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), _shape.rotation, { 0.0f, 0.0f, 1.0f });

        if(_scalingFactor != 1)
            _transformMat *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.x, 1.f});

        glm::vec4 _innerColor = {(float)_shape.getInnerColor().r / 255.f, (float)_shape.getInnerColor().g / 255.f,
                                 (float)_shape.getInnerColor().b/ 255.f, (float)_shape.getInnerColor().a/ 255.f};

        if(_shape.getPoints().size() == 2) {
            drawLine(_shape.getPoints()[0], _shape.getPoints()[1], _shape.getOuterColor());
        } else {
            if(_shape.isInnerShown()) {
                // Making the shape triangles
                for(int _i = 0; _i < _shape.getPoints().size() - 2; _i++) {
                    vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_shape.getPoints()[0].x, _shape.getPoints()[0].y, 0.0f, 1.f}, _innerColor);
                    vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_shape.getPoints()[_i + 1].x, _shape.getPoints()[_i + 1].y, 0.0f, 1.f}, _innerColor);
                    vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_shape.getPoints()[_i + 2].x, _shape.getPoints()[_i + 2].y, 0.0f, 1.f}, _innerColor);
                }
            }

            if(_shape.isOuterShown()) {
                for(int _i = 0; _i < _shape.getPoints().size(); _i++) {
                    int _next = _i + 1;
                    if(_next == _shape.getPoints().size())
                        _next = 0;
                    drawLine(_shape.getPoints()[_i], _shape.getPoints()[_next], _shape.getOuterColor());
                }
            }
        }
    }

    void SpriteBatch::orderBatches() {
        std::sort(batches.begin(), batches.end(), [&](const Batch& a_batch, const Batch& b_batch) {
            if(a_batch.priority == b_batch.priority) {
                if (a_batch.layer == b_batch.layer) {
                    return a_batch.textureID < b_batch.textureID;
                }

                return a_batch.layer < b_batch.layer;
            }

            return a_batch.priority < b_batch.priority;
        });
    }

    Batch& SpriteBatch::getBatch(const IRenderizable* _renderer, int _layer, BatchPriority _priority) {
        for(auto& _batch : batches)
            if(_renderer->getTexture() == _batch.textureID && _layer == _batch.layer && _batch.shader->getShaderID() == _renderer->shaderID && _batch.priority == _renderer->batchPriority)
                return _batch;

        LOG_W("Created a new batch with Texture: ", _renderer->getTexture(), ", Layer: ", _layer, ", Priority: ", _renderer->batchPriority, " and ShaderID: ", _renderer->shaderID)

        Batch _batch;
        _batch.layer = _layer;
        _batch.indexBuffer.reserve(maxIndicesPerDrawCall * 6);
        _batch.vertexBuffer.reserve(maxIndicesPerDrawCall * 6);
        _batch.textureID = _renderer->getTexture();
        _batch.priority = _priority;
        _batch.shader = shaderManager->getShader(_renderer->shaderID);
        _batch.spriteBatch = this;
        batches.push_back(_batch);

        orderBatches();

        return batches.back();
    }

    void SpriteBatch::draw(const IRenderizable* _renderizable, const Transform& _transform) {
        getBatch(_renderizable, _renderizable->layer, _renderizable->batchPriority).draw(_renderizable, _transform);
    }

    void SpriteBatch::flush() {
        for(auto& _batch : batches) {
            auto _shaderID = _batch.shader->getShaderID();
            if (_batch.vertexBuffer.empty() || _batch.indexBuffer.empty() || _batch.textureID < 0 || _shaderID < 0)
                continue;
            
            glUseProgram(_shaderID);

            GLint location = glGetUniformLocation(_shaderID, "viewProjectionMatrix");
            glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(viewProjectionMatrix)));

            glActiveTexture(GL_TEXTURE0);

            glBindVertexArray(_batch.shader->getShaderVAO());

            glBindTexture(GL_TEXTURE_2D, _batch.textureID);

            glBindBuffer(GL_ARRAY_BUFFER, _batch.shader->getShaderVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(_batch.shader->getShaderVertexDataSize() * _batch.vertexBuffer.size()), &_batch.vertexBuffer[0]);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.shader->getShaderIBO());
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (long)(sizeof(uint32_t) * _batch.indexBuffer.size()), &_batch.indexBuffer[0]);

            glDrawElements(GL_TRIANGLES, (int)_batch.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            totalTriangles += (int)_batch.vertexBuffer.size() / 2;
            _batch.vertexBuffer.clear();
            _batch.indexBuffer.clear();
            _batch.vertexCount = 0;

            drawCalls++;
        }
    }

    void SpriteBatch::Debug::flushDebug() {
        auto* _shader = batch->shaderManager->getShader(DEBUG_SHADER);

        glBindVertexArray(_shader->getShaderVAO());
        glUseProgram(_shader->getShaderID());

        GLint location = glGetUniformLocation(_shader->getShaderID(), "viewProjectionMatrix");
        glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(batch->viewProjectionMatrix)));

        if(!vertexDebugBufferGeometrics.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getShaderVBO());
            glBufferData(GL_ARRAY_BUFFER, (long)(_shader->getShaderVertexDataSize() * vertexDebugBufferGeometrics.size()), &vertexDebugBufferGeometrics[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, (int)vertexDebugBufferGeometrics.size());
        }

        if(!vertexDebugBufferLines.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getShaderVBO());
            glBufferData(GL_ARRAY_BUFFER, (long) (_shader->getShaderVertexDataSize() * vertexDebugBufferLines.size()), &vertexDebugBufferLines[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_LINES, 0, (int) vertexDebugBufferLines.size());
        }

        if(!vertexDebugBufferPoints.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getShaderVBO());
            glBufferData(GL_ARRAY_BUFFER, (long) (_shader->getShaderVertexDataSize() * vertexDebugBufferPoints.size()), &vertexDebugBufferPoints[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_POINTS, 0, (int) vertexDebugBufferPoints.size());
        }

        glBindVertexArray(0);

        vertexDebugBufferGeometrics.clear();
        vertexDebugBufferLines.clear();
        vertexDebugBufferPoints.clear();
    }

    void SpriteBatch::Debug::setLinesThickness(float _thickness) {
        glLineWidth(_thickness);
    }

    void SpriteBatch::Debug::setPointSize(float _size) {
        glPointSize(_size);
    }


    SpriteBatch* SpriteBatch::Debug::batch;

    void SpriteBatch::Debug::drawGrid(const Color& _color) {
//        auto _shader = batch->manager->shaderManager.getShader("grid");
//        glUseProgram(_shader);
//
//        float _zoom = Engine::get().manager.sceneManager.getDisplayedScene()->getMainCamera()->getCurrentZoomLevel();
//        float _params[4] = {(float)Engine::get().getWindowSize().x, (float)Engine::get().getWindowSize().y, 32.f * (1.f / _zoom), 32.f * (1.f / _zoom)};
//
//        GLint _location = glGetUniformLocation(_shader, "params");
//        glUniform4f(_location, _params[0], _params[1], _params[2], _params[3]);
//
//        _location = glGetUniformLocation(_shader, "color");
//        glUniform4f(_location, _color.r, _color.g, _color.b, _color.a);
//
//        _location = glGetUniformLocation(_shader, "viewProjectionMatrix");
//        glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(batch->viewProjectionMatrix)));
//
//        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
