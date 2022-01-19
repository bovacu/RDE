#include "SpriteBatch.h"
#include "core/util/Logger.h"
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>

namespace engine {
    void SpriteBatch::init(Window* _window) {
        initDebugShader();
        initDebugVbo();

        initShader();
        initVbo();

        window = _window;
    }

    void SpriteBatch::initShader() {
        basicShaderID = basicShader.loadFromFiles("assets/vertex.glsl", "assets/fragment.glsl");
        shadersDict[RenderShader::BASIC] = basicShaderID;
    }

    void SpriteBatch::initDebugShader() {
        debugShaderID = debugShader.loadFromFiles("assets/debugVertex.glsl", "assets/debugFragment.glsl");
        shadersDict[RenderShader::DEBUG] = debugShaderID;
    }

    void SpriteBatch::initVbo() {
        // Get the texture uniform from the shader program.
        textureUniform = glGetUniformLocation(basicShaderID, "tex");
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

    SpriteBatch::~SpriteBatch() {
        LOG_S("Cleaning up SpriteBatch")
        glDeleteProgram(basicShaderID);
        glDeleteProgram(debugShaderID);
    }

    void SpriteBatch::beginDraw(Camera& _camera, RenderShader _renderShader) {
        GLuint _shaderID = shadersDict[_renderShader];
        glUseProgram(_shaderID);
        GLint _location = glGetUniformLocation(_shaderID, "viewProjectionMatrix");
        auto _viewProjection = _camera.getProjectionMatrix() * glm::inverse(_camera.getTransform().transformMatrix);
        glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(_viewProjection)));
        aspectRatio = _camera.getAspectRatio();
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& sourceRect, const glm::vec4& color, Texture* _texture, float _rotation) {
        if (texture != _texture) {
            flush();

            _texture->incRefCount();
            if (texture != nullptr)
                texture->decRefCount();

            texture = _texture;
        }

        auto _translationMat = glm::translate(glm::mat4(1.f), glm::vec3 (destRect.x, destRect.y, 1.f));
        auto _rotationMat = glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });
        auto _scaleMat = glm::scale(glm::mat4(1.0f), { 1, 1, 1.0f });
        glm::mat4 _transformMat = _translationMat * _rotationMat * _scaleMat;


        glm::vec4 _bottomLeftTextureCorner = { -sourceRect.z / 2.f, -sourceRect.w / 2.f, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {sourceRect.z / 2.f, -sourceRect.w / 2.f, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {sourceRect.z / 2.f, sourceRect.w / 2.f, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-sourceRect.z / 2.f, sourceRect.w / 2.f, 0.0f, 1.0f };

        vertexBuffer.emplace_back(_transformMat * _bottomLeftTextureCorner, glm::vec2(0, 0), color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.y), color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(sourceRect.x, sourceRect.w), color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.y), color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner, glm::vec2(sourceRect.x, sourceRect.w), color);
        vertexBuffer.emplace_back(_transformMat * _topRightTextureCorner, glm::vec2(sourceRect.z, sourceRect.w), color);
    }

    void SpriteBatch::draw(const Sprite& _sprite) {
        if(texture == nullptr) {
            texture = &_sprite.getTexture();
        }

        if (texture->getGLTexture() != _sprite.getTexture().getGLTexture()) {
            flush();
            texture = &_sprite.getTexture();
        }

        auto _pos = worldToScreenCoords(_sprite.getPosition());
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_pos.x,_pos.y, 1.f));

        if(_sprite.getRotation() != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_sprite.getRotation()), { 0.0f, 0.0f, 1.0f });

        if(_sprite.getScale() != 1)
            _transformMat *= glm::scale(glm::mat4(1.0f), { _sprite.getScale().x, _sprite.getScale().y, 1.0f });


        Vec2F _textureOrigin = {(float)_sprite.getTexture().getRegion().bottomLeftCorner.x, (float)_sprite.getTexture().getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_sprite.getTexture().getSize().x, _textureOrigin.y / (float)_sprite.getTexture().getSize().y};

        Vec2F _textureTileSize = {(float)_sprite.getTexture().getRegion().size.x, (float)_sprite.getTexture().getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_sprite.getTexture().getSize().x, _textureTileSize.y / (float)_sprite.getTexture().getSize().y};
        auto _textureTileSizeOnScreen = worldToScreenSize(_textureTileSize);

        glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec4 _color = {(float)_sprite.getColor().r / 255.f, (float)_sprite.getColor().g/ 255.f,
                            (float)_sprite.getColor().b/ 255.f, (float)_sprite.getColor().a/ 255.f};

        vertexBuffer.emplace_back(_transformMat * _bottomLeftTextureCorner,glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner,glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner,glm::vec2(_textureOriginNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _bottomRightTextureCorner,glm::vec2( _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topLeftTextureCorner,glm::vec2(_textureOriginNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
        vertexBuffer.emplace_back(_transformMat * _topRightTextureCorner,glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x,  _textureOriginNorm.y + _textureTileSizeNorm.y), _color);
    }

    void SpriteBatch::draw(Text& _text) {
        if(texture == nullptr) {
            texture = _text.getFont()->texture;
        }

        if (texture->getGLTexture() != _text.getFont()->texture->openGLTextureID) {
            flush();
            texture = _text.getFont()->texture;
        }

        auto* _atlas = _text.getFont();
        float _x = _text.getPosition().x, _y = _text.getPosition().y;
        for(char _char : _text.getText()) {
            float x2 = _x + _atlas->characters[_char].bitmapPos.x * 1;
            float y2 = -_y - _atlas->characters[_char].bitmapPos.y * 1;
            float w = _atlas->characters[_char].bitmapSize.x * 1;
            float h = _atlas->characters[_char].bitmapSize.y * 1;

            _x += _atlas->characters[_char].advance.x * 1;
            _y += _atlas->characters[_char].advance.y * 1;

            /* Skip glyphs that have no pixels */
            if (w == 0 || h == 0) continue;

            auto _pos = worldToScreenCoords({_x, _y});
            auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_pos.x,_pos.y, 1.f));

            if(_text.getRotation() != 0)
                _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_text.getRotation()), { 0.0f, 0.0f, 1.0f });

            if(_text.getScale() != 1)
                _transformMat *= glm::scale(glm::mat4(1.0f), { _text.getScale().x, _text.getScale().y, 1.0f });

            glm::vec4 _color = {(float)Color::White.r / 255.f, (float)Color::White.g/ 255.f,
                                (float)Color::White.b/ 255.f, (float)Color::White.a/ 255.f};

            auto _coords = worldToScreenSize({x2, -y2});
            Vec2F _textCoords ={_atlas->characters[_char].offset.x, _atlas->characters[_char].offset.y};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                  glm::vec2(_textCoords.x, _textCoords.y), _color);


            _coords = worldToScreenSize({x2 + w, -y2});
            _textCoords = {_atlas->characters[_char].offset.x + _atlas->characters[_char].bitmapSize.x / (float)_atlas->width, _atlas->characters[_char].offset.y};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                  glm::vec2(_textCoords.x, _textCoords.y), _color);


            _coords = worldToScreenSize({x2, -y2 - h});
            _textCoords = {_atlas->characters[_char].offset.x, _atlas->characters[_char].offset.y + _atlas->characters[_char].bitmapSize.y / (float)_atlas->height};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                  glm::vec2(_textCoords.x, _textCoords.y), _color);


            _coords = worldToScreenSize({x2 + w, -y2});
            _textCoords = {_atlas->characters[_char].offset.x + _atlas->characters[_char].bitmapSize.x / (float)_atlas->width, _atlas->characters[_char].offset.y};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                  glm::vec2(_textCoords.x, _textCoords.y), _color);


            _coords = worldToScreenSize({x2, -y2 - h});
            _textCoords ={_atlas->characters[_char].offset.x, _atlas->characters[_char].offset.y + _atlas->characters[_char].bitmapSize.y / (float)_atlas->height};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                  glm::vec2(_textCoords.x, _textCoords.y), _color);


            _coords = worldToScreenSize({x2 + w, -y2 - h});
            _textCoords = {_atlas->characters[_char].offset.x + _atlas->characters[_char].bitmapSize.x / (float)_atlas->width,_atlas->characters[_char].offset.y + _atlas->characters[_char].bitmapSize.y / (float)_atlas->height};
            vertexBuffer.emplace_back(_transformMat * glm::vec4{_coords.x, _coords.y, 0.f, 1.f},
                          glm::vec2(_textCoords.x, _textCoords.y), _color);
        }


    }

    void SpriteBatch::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};
        auto _screenPos0 = worldToScreenCoords(_p0);
        auto _screenPos1 = worldToScreenCoords(_p1);
        vertexDebugBufferLines.emplace_back(glm::vec3 {_screenPos0.x, _screenPos0.y, 1.0f}, _colorVec4);
        vertexDebugBufferLines.emplace_back(glm::vec3 {_screenPos1.x, _screenPos1.y, 1.0f}, _colorVec4);
    }

    void SpriteBatch::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        auto _screenPos = worldToScreenCoords(_position);
        auto _transformMat = glm::translate(glm::mat4(1.f),glm::vec3 (_screenPos.x,_screenPos.y, 1.f));

        if(_rotation != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation), { 0.0f, 0.0f, 1.0f });

        glm::vec4 _colorVec4 = {(float)_color.r / 255.f, (float)_color.g/ 255.f,(float)_color.b/ 255.f, (float)_color.a/ 255.f};

        auto _screenSize = worldToScreenSize(_size);
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
        auto _transformMat = glm::translate(glm::mat4(1.f), glm::vec3 (_shape.getPosition().x, _shape.getPosition().y, 1.f));

        if(_shape.getRotation() != 0)
            _transformMat *= glm::rotate(glm::mat4(1.0f), glm::radians(_shape.getRotation()), { 0.0f, 0.0f, 1.0f });

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
        if (vertexBuffer.empty() || texture == nullptr)
            return;

        glUseProgram(basicShaderID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getGLTexture());
        glUniform1i(textureUniform, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (long)(sizeof(Vertex2dUVColor) * vertexBuffer.size()), &vertexBuffer[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glEnableVertexAttribArray(0); // position
        glEnableVertexAttribArray(1); // color
        glEnableVertexAttribArray(2); // textureCoords

        glDrawArrays(GL_TRIANGLES, 0, (int)vertexBuffer.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        totalTriangles += (int)vertexBuffer.size() / 3;
        vertexBuffer.clear();

        drawCalls++;
    }

    void SpriteBatch::flushDebug() {
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
        glUseProgram(0);
    }

    void SpriteBatch::setDebugLinesThickness(float _thickness) {
        glLineWidth(_thickness);
    }

    Vec2F SpriteBatch::worldToScreenCoords(Vec2F _position) {
        return {_position.x * aspectRatio / ((float)window->getWindowSize().x / 2), _position.y / ((float)window->getWindowSize().y / 2)};
    }

    Vec2F SpriteBatch::worldToScreenSize(Vec2F _size) {
        return {_size.x * aspectRatio / ((float)window->getWindowSize().x), _size.y / ((float)window->getWindowSize().y)};
    }
}