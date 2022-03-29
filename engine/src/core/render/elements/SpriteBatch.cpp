#include "core/render/elements/SpriteBatch.h"
#include "core/util/Logger.h"
#include "core/graph/Components.h"

#if IS_MOBILE()
    #include <GLES3/gl32.h>
#elif IS_DESKTOP()
    #include <glad/glad.h>
#endif

#include "core/util/Functions.h"
#include "core/Engine.h"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace engine {
    void SpriteBatch::init(Window* _window) {
        initDebugVbo();
        initVbo();
        window = _window;
    }

    void SpriteBatch::initVbo() {
        // Get the texture uniform from the shader program.
        texture = nullptr;

        // Setup vertex buffer
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLsizei _structSize = sizeof(Vertex2dUVColor);

        /* Attributes must be in the same order as in Vertex2dUVColor. Parameters are:
         *      layout, numberOfElements(1-4), GLType, normalized(bool), stride(sizeof(Vertex2dUVColor)), offset
         *
         *      The tricky one is offset, element 0 is always 0 bytes, element 1 is element 0 size, element 2 is
         *      element 0 + element 1 size, element 3 is element 0, + element 1 + element 2 size...
        */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _structSize, (void*) nullptr);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, _structSize, (void*)(4 * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, _structSize, (void*)(4 * 3 + 4 * 4));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SpriteBatch::initDebugVbo() {
        glGenBuffers(1, &debugVbo);
        glBindBuffer(GL_ARRAY_BUFFER, debugVbo);
        int _size = 3 * sizeof(float) + 4 * sizeof(float);
        // Position
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, _size, (void*)nullptr);
        // Color
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, _size, (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    SpriteBatch::~SpriteBatch() = default;

    void SpriteBatch::beginDraw(Camera& _camera) {
        viewProjectionMatrix = _camera.getProjectionMatrix() * glm::inverse(_camera.getTransform().modelMatrix);
        aspectRatio = _camera.getAspectRatio();
        scalingFactor = _camera.getViewport()->getScalingFactor();
        for(auto& _batch : batches) {
            _batch.aspectRatio = aspectRatio;
            _batch.scalingFactor = _camera.getViewport()->getScalingFactor();
        }
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& sourceRect, const glm::vec4& color, Texture* _texture, float _rotation) {
//        if (texture != _texture) {
//            flush();
//
//            _texture->incRefCount();
//            if (texture != nullptr)
//                texture->decRefCount();
//
//            texture = _texture;
//        }
//
//        auto _translationMat = glm::translate(glm::mat4(1.f), glm::vec3 (destRect.x, destRect.y, 1.f));
//        auto _rotationMat = glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });
//        auto _scaleMat = glm::scale(glm::mat4(1.0f), { 1, 1, 1.0f });
//        glm::mat4 _transformMat = _translationMat * _rotationMat * _scaleMat;
//
//
//        glm::vec4 _bottomLeftTextureCorner = { -sourceRect.z / 2.f, -sourceRect.w / 2.f, 0.0f, 1.0f };
//        glm::vec4 _bottomRightTextureCorner = {sourceRect.z / 2.f, -sourceRect.w / 2.f, 0.0f, 1.0f };
//        glm::vec4 _topRightTextureCorner = {sourceRect.z / 2.f, sourceRect.w / 2.f, 0.0f, 1.0f };
//        glm::vec4 _topLeftTextureCorner = {-sourceRect.z / 2.f, sourceRect.w / 2.f, 0.0f, 1.0f };
//
//        vertexBuffer.emplace_back(_transformMat * _bottomLeftTextureCorner, glm::vec2(0, 0), color);
//        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.y), color);
//        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(sourceRect.x, sourceRect.w), color);
//        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.y), color);
//        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(sourceRect.x, sourceRect.w), color);
//        vertexBuffer.emplace_back(_transformMat * _topRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.w), color);
    }

    void SpriteBatch::draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform) {
        getBatch(_spriteRenderer, _spriteRenderer.layer, BatchPriority::SpritePriority).addSprite(_spriteRenderer, _transform);
    }

    void SpriteBatch::draw(const TextRenderer& _text, const Transform& _transform) {
        getBatch(_text.spriteRenderer, 0, BatchPriority::TextPriority).addText(_text, _text.spriteRenderer, _transform);
    }

    void SpriteBatch::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};
        auto _screenPos0 = Util::worldToScreenCoords(_p0, window, aspectRatio);
        auto _screenPos1 = Util::worldToScreenCoords(_p1, window, aspectRatio);

        auto _transformMat0 = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos0.x, _screenPos0.y, 1.f));
        auto _transformMat1 = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos1.x, _screenPos1.y, 1.f));

        if(scalingFactor != 1) {
            _transformMat0 *= glm::scale(glm::mat4(1.0f), {scalingFactor.x, scalingFactor.x, 1.f});
            _transformMat1 *= glm::scale(glm::mat4(1.0f), {scalingFactor.x, scalingFactor.x, 1.f});
        }

        vertexDebugBufferLines.emplace_back(_transformMat0 * glm::vec4 {_screenPos0.x, _screenPos0.y, 1.0f, 1.0f}, _colorVec4);
        vertexDebugBufferLines.emplace_back(_transformMat0 * glm::vec4 {_screenPos1.x, _screenPos1.y, 1.0f, 1.0f}, _colorVec4);
    }

    void SpriteBatch::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        auto _screenPos = Util::worldToScreenCoords(_position, window, aspectRatio);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x,_screenPos.y, 1.f));

        if(_rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });

        if(scalingFactor != 1)
            _transformMat *= glm::scale(glm::mat4(1.0f), {scalingFactor.x, scalingFactor.x, 1.f});

        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};

        auto _screenSize = Util::worldToScreenSize(_size, window, aspectRatio);
        // First triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);

        // Second triangle
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, -_screenSize.y, 0.0f, 1.f}, _colorVec4);
        vertexDebugBufferGeometrics.emplace_back(_transformMat * glm::vec4{-_screenSize.x, _screenSize.y, 0.0f, 1.f}, _colorVec4);
    }

    void SpriteBatch::drawShape(Shape& _shape) {
        auto _transformMat = glm::translate(glm::mat4(1.f), glm::vec3 (_shape.getPosition().x * scalingFactor.x, _shape.getPosition().y * scalingFactor.y, 1.f));

        if(_shape.getRotation() != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_shape.getRotation()), { 0.0f, 0.0f, 1.0f });

        if(scalingFactor != 1)
            _transformMat *= glm::scale(glm::mat4(1.0f), {scalingFactor.x, scalingFactor.x, 1.f});

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
            if (_batch.vertexBuffer.empty() || _batch.texture == nullptr || _batch.shaderID < 0)
                continue;

            glUseProgram(_batch.shaderID);
            GLint _location = glGetUniformLocation(_batch.shaderID, "viewProjectionMatrix");
            glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(viewProjectionMatrix)));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _batch.texture->getGLTexture());

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, (long)(sizeof(Vertex2dUVColor) * _batch.vertexBuffer.size()), &_batch.vertexBuffer[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);


            glEnableVertexAttribArray(0); // position
            glEnableVertexAttribArray(1); // color
            glEnableVertexAttribArray(2); // textureCoords

            glDrawArrays(GL_TRIANGLES, 0, (int)_batch.vertexBuffer.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);

            totalTriangles += (int)_batch.vertexBuffer.size() / 3;
            _batch.vertexBuffer.clear();

            drawCalls++;
        }
    }

    void SpriteBatch::flushDebug() {
        ShaderID _id = ShaderManager::get().getShader("debug");
        glUseProgram(_id);
        GLint _location = glGetUniformLocation(_id, "viewProjectionMatrix");
        glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(viewProjectionMatrix)));

        if(!vertexDebugBufferGeometrics.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, debugVbo);
            glBufferData(GL_ARRAY_BUFFER, (long)(sizeof(VertexColorDebug) * vertexDebugBufferGeometrics.size()), &vertexDebugBufferGeometrics[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);

            glDrawArrays(GL_TRIANGLES, 0, (int)vertexDebugBufferGeometrics.size());

            glDisableVertexAttribArray(4);
            glDisableVertexAttribArray(5);
        }

        if(!vertexDebugBufferLines.empty()) {

            glBindBuffer(GL_ARRAY_BUFFER, debugVbo);
            glBufferData(GL_ARRAY_BUFFER, (long) (sizeof(VertexColorDebug) * vertexDebugBufferLines.size()), &vertexDebugBufferLines[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);

            glDrawArrays(GL_LINES, 0, (int) vertexDebugBufferLines.size());

            glDisableVertexAttribArray(4);
            glDisableVertexAttribArray(5);
        }

        vertexDebugBufferGeometrics.clear();
        vertexDebugBufferLines.clear();
    }

    void SpriteBatch::setDebugLinesThickness(float _thickness) {
        glLineWidth(_thickness);
    }

    SpriteBatch::Batch& SpriteBatch::getBatch(const SpriteRenderer& _spriteRenderer, int _layer, BatchPriority _priority) {
        for(auto& _batch : batches)
            if(_spriteRenderer.texture->getGLTexture() == _batch.texture->getGLTexture() && _layer == _batch.layer && _batch.shaderID == _spriteRenderer.shaderID)
                return _batch;

        LOG_W("Created a new batch")

        Batch _batch;
        _batch.window = window;
        _batch.aspectRatio = aspectRatio;
        _batch.layer = _layer;
        _batch.texture = _spriteRenderer.texture;
        _batch.priority = _priority;
        _batch.shaderID = _spriteRenderer.shaderID;
        batches.push_back(_batch);

        orderBatches();

        return batches.back();
    }

    void SpriteBatch::orderBatches() {
        std::sort(batches.begin(), batches.end(), [&](const Batch& a_batch, const Batch& b_batch) {
            if(a_batch.priority == b_batch.priority) {
                if (a_batch.layer == b_batch.layer) {
                    return a_batch.texture < b_batch.texture;
                }

                return a_batch.layer < b_batch.layer;
            }

            return a_batch.priority < b_batch.priority;
        });
    }

    void SpriteBatch::Batch::addSprite(const SpriteRenderer& _spriteRenderer, const Transform& _transform) {
        if(texture == nullptr)
            texture = _spriteRenderer.texture;

        auto _transformMat = _transform.modelMatrix;

        Vec2F _textureOrigin = {(float)_spriteRenderer.texture->getRegion().bottomLeftCorner.x, (float)_spriteRenderer.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_spriteRenderer.texture->getSize().x, _textureOrigin.y / (float)_spriteRenderer.texture->getSize().y};

        Vec2F _textureTileSize = {(float)_spriteRenderer.texture->getRegion().size.x, (float)_spriteRenderer.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_spriteRenderer.texture->getSize().x, _textureTileSize.y / (float)_spriteRenderer.texture->getSize().y};
        auto _textureTileSizeOnScreen = Util::worldToScreenSize(_textureTileSize, window, aspectRatio);

        glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec4 _color = {(float)_spriteRenderer.color.r / 255.f, (float)_spriteRenderer.color.g/ 255.f,
                            (float)_spriteRenderer.color.b/ 255.f, (float)_spriteRenderer.color.a/ 255.f};

        vertexBuffer.emplace_back(_transformMat * _bottomLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2( _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
    }

    void SpriteBatch::Batch::addText(const TextRenderer& _text, const SpriteRenderer& _spriteRenderer, const Transform& _transform) {
        if(texture == nullptr) {
            texture = &_text.getFont()->getTexture();
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

            auto _textColor = _spriteRenderer.color;
            glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

            auto _positionInScreen = Util::worldToScreenSize({x2 + _x, y2 + _y}, window, aspectRatio);
            auto _sizeInScreen = Util::worldToScreenSize({w, h}, window, aspectRatio);

            glm::vec4 _bottomLeftTextureCorner = { _positionInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner = {_positionInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

            glm::vec2 _bottomLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y};
            glm::vec2 _bottomRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x, _chars[_char].offset.y};
            glm::vec2 _topLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};
            glm::vec2 _topRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x,_chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};

            vertexBuffer.emplace_back(_transformMat * _bottomLeftTextureCorner,_bottomLeftTextureCoord, _color);
            vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner,_bottomRightTextureCoord, _color);
            vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner,_topLeftTextureCoord, _color);
            vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner,_bottomRightTextureCoord, _color);
            vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner,_topLeftTextureCoord, _color);
            vertexBuffer.emplace_back(_transformMat * _topRightTextureCorner,_topRightTextureCoord, _color);

            _x += _chars[_char].advance.x / 2.f;
            _y += _chars[_char].advance.y;
        }
    }

    SpriteBatch* SpriteBatch::Debug::batch;

    void SpriteBatch::Debug::drawGrid(const Color& _color) {
        auto _shader = ShaderManager::get().getShader("grid");
        glUseProgram(_shader);

        float _zoom = Engine::get().getMainCamera().getCurrentZoomLevel();
        float _params[4] = {(float)Engine::get().getWindowSize().x, (float)Engine::get().getWindowSize().y, 32.f * (1.f / _zoom), 32.f * (1.f / _zoom)};

        GLint _location = glGetUniformLocation(_shader, "params");
        glUniform4f(_location, _params[0], _params[1], _params[2], _params[3]);

        _location = glGetUniformLocation(_shader, "color");
        glUniform4f(_location, _color.r, _color.g, _color.b, _color.a);

        _location = glGetUniformLocation(_shader, "viewProjectionMatrix");
        glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(batch->viewProjectionMatrix)));

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void SpriteBatch::Debug::init(SpriteBatch* _batch) {
        batch = _batch;
    }
}