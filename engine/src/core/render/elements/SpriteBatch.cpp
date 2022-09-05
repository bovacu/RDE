#include "core/render/elements/SpriteBatch.h"
#include "core/util/Logger.h"
#include "core/graph/components/Components.h"
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
        shaderManager->loadShaderVertexConfig("basic", {
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

        shaderManager->loadShaderVertexConfig("basicText", {
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
        Debug::batch->shaderManager->loadShaderVertexConfig("debug", {
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

    void SpriteBatch::draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform) {
        getBatch(_spriteRenderer, _spriteRenderer.layer, BatchPriority::SpritePriority).addSprite(_spriteRenderer, _transform);
    }

    void SpriteBatch::draw(const TextRenderer& _text, const Transform& _transform) {
        getBatch(_text, 0, BatchPriority::TextPriority).addText(_text, _transform);
    }

    void SpriteBatch::draw(const ParticleSystem& _particleSystem, const Transform& _transform) {

    }

    void SpriteBatch::draw(const NineSliceSprite& _nineSlice, const Transform& _transform) {
        getBatch(_nineSlice, _nineSlice.layer, BatchPriority::SpritePriority).addNinePatchSprite(_nineSlice, _transform);
    }

    void SpriteBatch::Debug::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};
        auto _screenPos0 = Util::worldToScreenCoords(batch->viewport, _p0);
        auto _screenPos1 = Util::worldToScreenCoords(batch->viewport, _p1);

        auto _transformMat0 = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos0.x, _screenPos0.y, 1.f));
        auto _transformMat1 = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos1.x, _screenPos1.y, 1.f));

        auto _scalingFactor = batch->viewport->getScalingFactor();
        if(_scalingFactor != 1) {
            _transformMat0 *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.x, 1.f});
            _transformMat1 *= glm::scale(glm::mat4(1.0f), {_scalingFactor.x, _scalingFactor.x, 1.f});
        }

        vertexDebugBufferLines.emplace_back(_transformMat0 * glm::vec4 {_screenPos0.x, _screenPos0.y, 0.0f, 1.0f}, _colorVec4);
        vertexDebugBufferLines.emplace_back(_transformMat0 * glm::vec4 {_screenPos1.x, _screenPos1.y, 0.0f, 1.0f}, _colorVec4);
    }

    void SpriteBatch::Debug::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        auto _screenPos = Util::worldToScreenCoords(batch->viewport, _position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x,_screenPos.y, 1.f));

        if(_rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });

        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};

        auto _screenSize = Util::worldToScreenSize(batch->viewport, _size);
        // First triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);

        // Second triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
    }

    void SpriteBatch::Debug::drawShape(Shape& _shape) {
        auto _scalingFactor = batch->viewport->getScalingFactor();
        auto _transformMat = glm::translate(glm::mat4(1.f), glm::vec3 (_shape.getPosition().x * _scalingFactor.x, _shape.getPosition().y * _scalingFactor.y, 1.f));

        if(_shape.getRotation() != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_shape.getRotation()), { 0.0f, 0.0f, 1.0f });

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

    void SpriteBatch::flush() {
        for(auto& _batch : batches) {
            auto _shaderID = _batch.shader->getShaderID();
            if (_batch.vertexBuffer.empty() || _batch.indexBuffer.empty() || _batch.textureID < 0 || _shaderID < 0)
                continue;
            
            glUseProgram(_shaderID);

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
            vertices = 0;
        }
    }

    void SpriteBatch::Debug::flushDebug() {
        auto* _shader = batch->shaderManager->getShader("debug");

        glBindVertexArray(_shader->getShaderVAO());
        glUseProgram(_shader->getShaderID());

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

        glBindVertexArray(0);

        vertexDebugBufferGeometrics.clear();
        vertexDebugBufferLines.clear();
    }

    void SpriteBatch::Debug::setDebugLinesThickness(float _thickness) {
        glLineWidth(_thickness);
    }

    SpriteBatch::Batch& SpriteBatch::getBatch(const IRenderizable& _renderer, int _layer, BatchPriority _priority) {
        for(auto& _batch : batches)
            if(_renderer.getTexture() == _batch.textureID && _layer == _batch.layer && _batch.shader->getShaderID() == _renderer.shaderID)
                return _batch;

        LOG_W("Created a new batch")

        Batch _batch;
        _batch.layer = _layer;
       _batch.indexBuffer.reserve(maxIndicesPerDrawCall * 6);
       _batch.vertexBuffer.reserve(maxIndicesPerDrawCall * 6);
        _batch.textureID = _renderer.getTexture();
        _batch.priority = _priority;
        _batch.shader = shaderManager->getShader(_renderer.shaderID);
        _batch.spriteBatch = this;
        batches.push_back(_batch);

        orderBatches();

        return batches.back();
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

    void SpriteBatch::Batch::addSprite(const SpriteRenderer& _spriteRenderer, const Transform& _transform) {

        if(indexBuffer.size() + 6 >= spriteBatch->maxIndicesPerDrawCall)
            spriteBatch->flush();

        if(textureID < 0)
            textureID = _spriteRenderer.texture->getGLTexture();

        auto _transformMat = _transform.modelMatrix;
        auto _screenPos = Util::worldToScreenCoords(spriteBatch->viewport, {_transform.modelMatrix[3][0], _transform.modelMatrix[3][1]});
        _transformMat[3][0] = _screenPos.x;
        _transformMat[3][1] = _screenPos.y;

        Vec2F _textureOrigin = {(float)_spriteRenderer.texture->getRegion().bottomLeftCorner.x, (float)_spriteRenderer.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_spriteRenderer.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_spriteRenderer.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_spriteRenderer.texture->getRegion().size.x, (float)_spriteRenderer.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_spriteRenderer.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_spriteRenderer.texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::worldToScreenSize(spriteBatch->viewport, _textureTileSize);

        glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec4 _color = {(float)_spriteRenderer.color.r / 255.f, (float)_spriteRenderer.color.g/ 255.f,
                            (float)_spriteRenderer.color.b/ 255.f, (float)_spriteRenderer.color.a/ 255.f};

        vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });

        indexBuffer.emplace_back(vertexCount + 0);
        indexBuffer.emplace_back(vertexCount + 1);
        indexBuffer.emplace_back(vertexCount + 2);

        indexBuffer.emplace_back(vertexCount + 2);
        indexBuffer.emplace_back(vertexCount + 3);
        indexBuffer.emplace_back(vertexCount + 0);

        vertexCount += 4;
    }

    void SpriteBatch::Batch::addText(const TextRenderer& _text, const Transform& _transform) {

        if(indexBuffer.size() + _text.getText().size() * 6 >= spriteBatch->maxIndicesPerDrawCall)
            spriteBatch->flush();

        if(textureID < 0) {
            textureID = _text.getFont()->getTexture().getGLTexture();
        }

        auto* _atlas = _text.getFont();
        auto _atlasSize = _atlas->getSize();

        float _x = _transform.getPositionLocal().x - _text.getTextSize().x / 2.f;
        float _y = _transform.getPositionLocal().y - _text.getTextSize().y / 4.f;

        auto* _chars = _atlas->getChars();

        for(char _char : _text.getText()) {
            float x2 = _x + _chars[_char].bitmapPos.x * 1;
            float y2 = -_y - _chars[_char].bitmapPos.y * 1;
            float w = _chars[_char].bitmapSize.x * 1;
            float h = _chars[_char].bitmapSize.y * 1;

            if(_char == '\n') {
                _y -= _chars[_text.getText()[0]].bitmapSize.y * 0.8f;
                _x = _transform.getPositionLocal().x - _text.getTextSize().x / 2.f;
                continue;
            }

            if (w == 0 || h == 0) {
                _x += _chars[_char].advance.x / 2.f;
                continue;
            }

            auto _transformMat = _transform.modelMatrix;
            auto _screenPos = Util::worldToScreenCoords(spriteBatch->viewport, {_transform.modelMatrix[3][0], _transform.modelMatrix[3][1]});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;

            auto _textColor = _text.color;
            glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

            auto _positionInScreen = Util::worldToScreenSize(spriteBatch->viewport, {x2 + _x, y2 + _y});
            auto _sizeInScreen = Util::worldToScreenSize(spriteBatch->viewport, {w, h});

            glm::vec4 _bottomLeftTextureCorner = { _positionInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner = {_positionInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

            glm::vec2 _bottomLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y};
            glm::vec2 _bottomRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x, _chars[_char].offset.y};
            glm::vec2 _topLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};
            glm::vec2 _topRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x,_chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};

            vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner, _bottomLeftTextureCoord, _color });
            vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord, _color });
            vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner, _topRightTextureCoord, _color });
            vertexBuffer.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner, _topLeftTextureCoord, _color });

            _x += _chars[_char].advance.x / 2.f;
            _y += _chars[_char].advance.y;

            indexBuffer.emplace_back(vertexCount + 0);
            indexBuffer.emplace_back(vertexCount + 1);
            indexBuffer.emplace_back(vertexCount + 2);

            indexBuffer.emplace_back(vertexCount + 2);
            indexBuffer.emplace_back(vertexCount + 3);
            indexBuffer.emplace_back(vertexCount + 0);

            vertexCount += 4;
        }
    }

    void SpriteBatch::Batch::addNinePatchSprite(const NineSliceSprite& _nineSlice, const Transform& _transform) {
        if(vertexCount + 9 * 6 >= spriteBatch->maxIndicesPerDrawCall)
            spriteBatch->flush();

        if(textureID < 0)
            textureID = _nineSlice.texture->getGLTexture();

        auto _rectsAmount = *(&_nineSlice.getNineSlice().subRects + 1) - _nineSlice.getNineSlice().subRects;
        for(auto _i = 0; _i < _rectsAmount; _i++) {
            auto& _subTextureRegion = _nineSlice.getNineSlice().subRects[_i];
            auto _transformMat = _transform.modelMatrix;

            float _distortX = 1.f, _distortY = 1.f;
            auto _uiSize = _nineSlice.ninePatchSize;
            auto _spriteSize = _nineSlice.getRegion().size;

            auto _bottomLeftCornerLocal = Vec2F { (float)_subTextureRegion.bottomLeftCorner.x, (float)(_subTextureRegion.bottomLeftCorner.y -
                    _nineSlice.getNineSlice().subRects[0].bottomLeftCorner.y) };
            Vec2F _position = {_transform.getPositionLocal().x - (float)_spriteSize.x / 2.f + (float)_subTextureRegion.bottomLeftCorner.x + (float)_subTextureRegion.size.x / 2.f, _transform.getPositionWorld().y - (float)((float)_spriteSize.y / 2.f - _bottomLeftCornerLocal.y) + (float)_subTextureRegion.size.y / 2.f};

            if((float)_uiSize.x - (float)_spriteSize.x != 0) {
                auto _widthOfCorners = (float) _nineSlice.getNineSlice().subRects[0].size.x + (float) _nineSlice.getNineSlice().subRects[2].size.x;
                auto _totalDiffX = (float)_uiSize.x - _widthOfCorners;
                _distortX = _totalDiffX / ((float)_spriteSize.x - _widthOfCorners);

                auto _halfWidthOfDistortedMiddleRect = _distortX * _transformMat[0][0] * (float) _nineSlice.getNineSlice().subRects[1].size.x / 2.f;
                auto _halfWidthOfOriginalMiddleRect = (float) _nineSlice.getNineSlice().subRects[1].size.x / 2.f;
                if(_i == 0 || _i == 3 || _i == 6) _position.x -= _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
                if(_i == 2 || _i == 5 || _i == 8) _position.x += _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
            }

            if((float)_uiSize.y - (float)_spriteSize.y != 0) {
                auto _heightOfCorners = (float) _nineSlice.getNineSlice().subRects[0].size.y + (float) _nineSlice.getNineSlice().subRects[6].size.y;
                auto _totalDiffY = (float)_uiSize.y - _heightOfCorners;
                _distortY = _totalDiffY / ((float)_spriteSize.y - _heightOfCorners);

                auto _halfHeightOfDistortedMiddleRect = _distortY * _transformMat[1][1] * (float) _nineSlice.getNineSlice().subRects[3].size.y / 2.f;
                auto _halfHeightOfOriginalMiddleRect = (float) _nineSlice.getNineSlice().subRects[3].size.y / 2.f;
                if(_i == 0 || _i == 1 || _i == 2) _position.y -= _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
                if(_i == 6 || _i == 7 || _i == 8) _position.y += _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
            }

            auto _screenPos = Util::worldToScreenCoords(spriteBatch->viewport, {_position.x, _position.y});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;
            if(_i == 1 || _i == 4 || _i == 7) _transformMat[0][0] *= _distortX;
            if(_i == 3 || _i == 4 || _i == 5) _transformMat[1][1] *= _distortY;


            uploadVertices(_transformMat, _nineSlice, _subTextureRegion);
        }

    }

    void SpriteBatch::Batch::uploadVertices(const glm::mat4& _transform, const NineSliceSprite& _nineSlice, const IntRect& _subTexture) {
        Vec2F _textureOrigin = {(float)_subTexture.bottomLeftCorner.x, (float)_subTexture.bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_nineSlice.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_nineSlice.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_subTexture.size.x, (float)_subTexture.size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_nineSlice.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_nineSlice.texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::worldToScreenSize(spriteBatch->viewport, _textureTileSize);

        glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec4 _color = {(float)_nineSlice.color.r / 255.f, (float)_nineSlice.color.g / 255.f,
                            (float)_nineSlice.color.b / 255.f, (float)_nineSlice.color.a / 255.f};

        vertexBuffer.emplace_back(OpenGLVertex {_transform * _bottomLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transform * _bottomRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transform * _topRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });
        vertexBuffer.emplace_back(OpenGLVertex {_transform * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });

        indexBuffer.emplace_back(vertexCount + 0);
        indexBuffer.emplace_back(vertexCount + 1);
        indexBuffer.emplace_back(vertexCount + 2);

        indexBuffer.emplace_back(vertexCount + 2);
        indexBuffer.emplace_back(vertexCount + 3);
        indexBuffer.emplace_back(vertexCount + 0);

        vertexCount += 4;
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
