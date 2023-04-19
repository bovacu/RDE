#include "core/render/elements/SpriteBatch.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl32.h>
#elif IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#else
    #include "glad/glad.h"
#endif

#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "glm/gtc/type_ptr.hpp"
#include "core/Engine.h"
#include "core/util/Mat2.h"

namespace RDE {

    void SpriteBatch::init(Engine* _engine) {
        engine = _engine;
        configBasicShader();
    }

    void SpriteBatch::Debug::init(SpriteBatch* _batch) {
        batch = _batch;
        configDebugShader();
    }

    void SpriteBatch::configBasicShader() {
        GLsizei _structSize = sizeof(OpenGLVertex);
		Util::GL::checkError("Error BEFORE ConfigBasicShader");
        engine->manager.shaderManager.loadShaderVertexConfig(SPRITE_RENDERER_SHADER, {
            VertexConfig {
                0, 2, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
				1, 4, GL_UNSIGNED_BYTE, sizeof(float) * 2, _structSize
            },
            VertexConfig {
				2, 2, GL_FLOAT, sizeof(float) * 2 + sizeof(unsigned char) * 4, _structSize
            }
        },
		{
			"viewProjectionMatrix"
		},
        maxIndicesPerDrawCall);
		Util::GL::checkError("Error AFTER ConfigBasicShader for sprites");

		Util::Log::info("Size of OpenGLVertex: ", _structSize);

        engine->manager.shaderManager.loadShaderVertexConfig(TEXT_RENDERER_SHADER, {
            VertexConfig {
                0, 2, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
				1, 4, GL_UNSIGNED_BYTE, sizeof(float) * 2, _structSize
            },
            VertexConfig {
				2, 2, GL_FLOAT, sizeof(float) * 2 + sizeof(unsigned char) * 4, _structSize
			}
		},
		{
			"viewProjectionMatrix"
		},
        maxIndicesPerDrawCall);
		Util::GL::checkError("Error AFTER ConfigBasicShader for text");
    }

    void SpriteBatch::Debug::configDebugShader() {
		GLsizei _structSize = sizeof(OpenGLVertexDebug);
        Debug::batch->engine->manager.shaderManager.loadShaderVertexConfig(DEBUG_SHADER, {
            VertexConfig {
                0, 2, GL_FLOAT, 0, _structSize
            },
            VertexConfig {
                1, 4, GL_UNSIGNED_BYTE, 2 * sizeof(float), _structSize
            }
		},
		{
			"viewProjectionMatrix"
		},
        Debug::batch->maxIndicesPerDrawCall);
        Util::GL::checkError("SpriteBatch configDebugShader");
    }

    SpriteBatch::~SpriteBatch() {};

	void SpriteBatch::overwriteRenderingCamera(Camera* _camera) {
		overwriteCamera = _camera;
	}

    void SpriteBatch::beginDraw(Camera* _camera, Transform* _cameraTransform) {
		auto* _cam = overwriteCamera != nullptr ? overwriteCamera : _camera;

		viewProjectionMatrix = _cam->getViewProjectionMatrix();
		viewport = _cam->getViewport();
    }

    void SpriteBatch::Debug::drawPoint(const Vec2F& _position, const Color& _color) {
        if(vertexDebugBufferPoints.size() > 50000)
            flushDebug();

        auto _uint32Color = Util::Math::colorToUint32_t(_color);
        auto _screenPos = Util::Math::worldToScreenCoords(batch->viewport, _position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x, _screenPos.y, 0.f));
		vertexDebugBufferPoints.emplace_back(glm::vec2{_transformMat[3][0], _transformMat[3][1]}, _uint32Color);
    }

    void SpriteBatch::Debug::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        auto _screenPos0 = Util::Math::worldToScreenCoords(batch->viewport, _p0);
        auto _screenPos1 = Util::Math::worldToScreenCoords(batch->viewport, _p1);

        auto _transformMat0 = glm::mat4(1.f);
        auto _transformMat1 = glm::mat4(1.f);

        _transformMat0[3][0] = _screenPos0.x;
        _transformMat0[3][1] = _screenPos0.y;

        _transformMat1[3][0] = _screenPos1.x;
        _transformMat1[3][1] = _screenPos1.y;

        auto _uint32Color = Util::Math::colorToUint32_t(_color);

        vertexDebugBufferLines.emplace_back(glm::vec2 {_transformMat0[3][0], _transformMat0[3][1]}, _uint32Color);
        vertexDebugBufferLines.emplace_back(glm::vec2 {_transformMat1[3][0], _transformMat1[3][1]}, _uint32Color);
    }

    void SpriteBatch::Debug::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        auto _screenPos = Util::Math::worldToScreenCoords(batch->viewport, _position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x,_screenPos.y, 1.f));

        if(_rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });

        auto _uint32Color = Util::Math::colorToUint32_t(_color);

        auto _screenSize = Util::Math::worldToScreenSize(batch->viewport, _size);
        // First triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _uint32Color);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, _screenSize.y, 0.0f, 1.f}, _uint32Color);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _uint32Color);

        // Second triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _uint32Color);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _uint32Color);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _uint32Color);
    }

    void SpriteBatch::Debug::drawShape(DebugShape& _shape) {
        auto _transformMat = glm::translate(glm::mat4(1.f), glm::vec3 (_shape.getPosition().x, _shape.getPosition().y, 1.f));

        auto _innerColor = Util::Math::colorToUint32_t(_shape.getInnerColor());

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
                Mat2 _rotMatrix(1, _transformMat[3][0], 0, _transformMat[3][1]);
                _rotMatrix.rotate(_shape.rotation);
                for(int _i = 0; _i < _shape.getPoints().size(); _i++) {
                    int _next = _i + 1;
                    if(_next == _shape.getPoints().size())
                        _next = 0;
                    drawLine(_rotMatrix * _shape.getPoints()[_i], _rotMatrix * _shape.getPoints()[_next], _shape.getOuterColor());
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

    Batch* SpriteBatch::getBatch(const RenderizableInnerData& _innerData) {
        for(auto& _batch : batches) {
            if (_innerData.texture->getGLTexture()  == _batch.textureID &&
                _innerData.layer                    == _batch.layer &&
                _innerData.shader                   == _batch.shader->getShaderID() &&
                _innerData.batchPriority            == _batch.priority &&
                _batch.vertexBuffer.size()          < maxIndicesPerDrawCall * 4) {
                return &_batch;
            }
        }

        Util::Log::debug("Created a new batch with Texture: ", _innerData.texture->getGLTexture(), ", Layer: ", _innerData.layer, ", Priority: ", _innerData.batchPriority, ", ShaderID: ", _innerData.shader);
        Batch _batch;
        _batch.ID = batches.size();
        _batch.layer = _innerData.layer;
        _batch.vertexBuffer.reserve(maxIndicesPerDrawCall * 4);
        _batch.textureID = _innerData.texture->getGLTexture();
        _batch.priority = _innerData.batchPriority;
        _batch.shader = engine->manager.shaderManager.getShader(_innerData.shader);
        _batch.spriteBatch = this;
        batches.push_back(_batch);

        orderBatches();

        return &batches.back();
    }

    void SpriteBatch::drawSpriteRenderer(RenderizableInnerData& _innerData, Transform* _transform) {
        auto* _batch = getBatch(_innerData);
         if(_batch->textureID < 0) _batch->textureID = _innerData.texture->getGLTexture();
        drawBatchedSpriteRenderer(_innerData, _batch, _transform, viewport);
    }

    void SpriteBatch::drawTextRenderer(RenderizableInnerData& _innerData, Transform* _transform) {
        auto* _batch = getBatch(_innerData);
         if(_batch->textureID < 0) _batch->textureID = _innerData.texture->getGLTexture();
        drawBatchedTextRenderer(_innerData, _batch, _transform, viewport);
    }

    void SpriteBatch::flush() {
        for(auto& _batch : batches) {
            auto _shaderID = _batch.shader->getShaderID();
            if (_batch.vertexBuffer.empty() || _batch.textureID < 0 || _shaderID < 0) {
                Util::Log::warn("Batch: ", _batch.ID, " was skipped, data - VB size: ", _batch.vertexBuffer.size(), ", Texture: ", _batch.textureID, ", Shader: ", _shaderID);
                continue;
            }
            
            glUseProgram(_shaderID);
            Util::GL::checkError("After glUseProgram");

            glUniformMatrix4fv(glGetUniformLocation(_shaderID, "viewProjectionMatrix"), 1, GL_FALSE, GLM_VEC_MAT_TO_POINTER(GLfloat, viewProjectionMatrix));
			Util::GL::checkError("After setUniformValueFloat");

            glActiveTexture(GL_TEXTURE0);
            Util::GL::checkError("After glActiveTexture");

            glBindVertexArray(_batch.shader->getDynamicShaderVAO());
            Util::GL::checkError("After glBindVertexArray");

            glBindTexture(GL_TEXTURE_2D, _batch.textureID);
            Util::GL::checkError("After glBindTexture");

            glBindBuffer(GL_ARRAY_BUFFER, _batch.shader->getDynamicShaderVBO());
            Util::GL::checkError("After glBindBuffer");
            glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(_batch.shader->getShaderVertexDataSize() * _batch.vertexBuffer.size()), &_batch.vertexBuffer[0]);
            Util::GL::checkError("After glBufferSubData");

            glDrawArrays(GL_TRIANGLES, 0, _batch.vertexBuffer.size());
            Util::GL::checkError("After glDrawArrays");

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Util::GL::checkError("After glBindBuffer disable");

            glBindVertexArray(0);
            Util::GL::checkError("After glBindVertexArray disable");

            totalTriangles += (int)_batch.vertexBuffer.size() / 3;
            _batch.vertexBuffer.clear();
            _batch.vertexCount = 0;

            drawCalls++;
        }
    }

	void SpriteBatch::flushUI(std::vector<Batch>& _batches) {
        for(auto& _batch : _batches) {
            auto _shaderID = _batch.shader->getShaderID();
            if (_batch.vertexBuffer.empty() || _batch.textureID < 0 || _shaderID < 0)
                continue;

            glUseProgram(_shaderID);
            Util::GL::checkError("After glUseProgram UI");

            glUniformMatrix4fv(glGetUniformLocation(_shaderID, "viewProjectionMatrix"), 1, GL_FALSE, GLM_VEC_MAT_TO_POINTER(GLfloat, viewProjectionMatrix));
            Util::GL::checkError("After glUniformMatrix4fv UI");

            glActiveTexture(GL_TEXTURE0);
            Util::GL::checkError("After glActiveTexture UI");

            glBindVertexArray(_batch.shader->getDynamicShaderVAO());
            Util::GL::checkError("After glBindVertexArray UI");

            glBindTexture(GL_TEXTURE_2D, _batch.textureID);
            Util::GL::checkError("After glBindTexture UI");

            glBindBuffer(GL_ARRAY_BUFFER, _batch.shader->getDynamicShaderVBO());
            Util::GL::checkError("After glBindBuffer UI");
            glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(_batch.shader->getShaderVertexDataSize() * _batch.vertexBuffer.size()), &_batch.vertexBuffer[0]);
            Util::GL::checkError("After glBufferSubData UI");

            glDrawArrays(GL_TRIANGLES, 0, _batch.vertexBuffer.size());
            Util::GL::checkError("After glDrawArrays UI");

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Util::GL::checkError("After glBindBuffer disable UI");

            glBindVertexArray(0);
            Util::GL::checkError("After glBindVertexArray disable UI");

            totalTriangles += (int)_batch.vertexBuffer.size() / 3;
            _batch.vertexBuffer.clear();
            uiDrawCalls++;
        }
    }

    void SpriteBatch::Debug::flushDebug() {
        auto* _shader = batch->engine->manager.shaderManager.getShader(DEBUG_SHADER);

        glBindVertexArray(_shader->getDynamicShaderVAO());
        glUseProgram(_shader->getShaderID());

		_shader->setUniformValueFloat("viewProjectionMatrix", RDE_UNIFORM_FV_MATRIX_4, GLM_VEC_MAT_TO_POINTER(GLfloat, batch->viewProjectionMatrix));

        if(!vertexDebugBufferGeometrics.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getDynamicShaderVBO());
            glBufferData(GL_ARRAY_BUFFER, (long)(_shader->getShaderVertexDataSize() * vertexDebugBufferGeometrics.size()), &vertexDebugBufferGeometrics[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, (int)vertexDebugBufferGeometrics.size());
        }

        if(!vertexDebugBufferLines.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getDynamicShaderVBO());
            glBufferData(GL_ARRAY_BUFFER, (long) (_shader->getShaderVertexDataSize() * vertexDebugBufferLines.size()), &vertexDebugBufferLines[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_LINES, 0, (int) vertexDebugBufferLines.size());
        }

        if(!vertexDebugBufferPoints.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, _shader->getDynamicShaderVBO());
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
    #if !IS_MOBILE()
        #ifndef __EMSCRIPTEN__
            glPointSize(_size);
        #endif
    #endif
    }


    SpriteBatch* SpriteBatch::Debug::batch;

    void SpriteBatch::Debug::drawGrid(const Color& _color) {
//        auto _shader = batch->manager->shaderManager.getShader("grid");
//        glUseProgram(_shader);
//
//        float _zoom = Engine::get().manager.sceneManager.getDisplayedScene()->mainCamera->getCurrentZoomLevel();
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
