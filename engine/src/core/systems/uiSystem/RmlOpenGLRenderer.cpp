//
// Created by borja on 09/05/22.
//

#include "core/systems/uiSystem/RmlOpenGLRenderer.h"
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Platform.h>

namespace GDE {

    static int viewportWidth = 0;
    static int viewportHeight = 0;

    struct CompiledGeometryData {
        GLuint texture;
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLsizei draw_count;
    };
    static Rml::Matrix4f projection;

    static void CheckGLError(const char* _operationName) {
        #ifdef RMLUI_DEBUG
        GLenum _errorCode = glGetError();
        if (_errorCode != GL_NO_ERROR) {
            static const Rml::Pair<GLenum, const char*> _errorNames[] = {{GL_INVALID_ENUM,      "GL_INVALID_ENUM"}, {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
                                                                         {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"}, {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"}};
            const char* _errorStr = "''";
            for (auto& _err : _errorNames)
            {
                if (_err.first == _errorCode)
                {
                    _errorStr = _err.second;
                    break;
                }
            }
            Rml::Log::Message(Rml::Log::LT_ERROR, "OpenGL error during %s. Error code 0x%x (%s).", _operationName, _errorCode, _errorStr);
        }
        #endif
        (void)_operationName;
    }

    void RmlOpenGLRenderer::RenderGeometry(Rml::Vertex* _vertices, int _numVertices, int* _indices, int _numIndices, const Rml::TextureHandle _texture, const Rml::Vector2f& _translation) {
        Rml::CompiledGeometryHandle _geometry = CompileGeometry(_vertices, _numVertices, _indices, _numIndices, _texture);

        if (_geometry) {
            RenderCompiledGeometry(_geometry, _translation);
            ReleaseCompiledGeometry(_geometry);
        }
    }

    Rml::CompiledGeometryHandle RmlOpenGLRenderer::CompileGeometry(Rml::Vertex* _vertices, int _numVertices, int* _indices, int _numIndices, Rml::TextureHandle _texture) {
        constexpr GLenum _drawUsages = GL_STATIC_DRAW;

        GLuint _vao = 0;
        GLuint _vbo = 0;
        GLuint _ibo = 0;

        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ibo);
        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, (long)sizeof(Rml::Vertex) * _numVertices, (const void*)_vertices, _drawUsages);

        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, position)));

        glEnableVertexAttribArray(13);
        glVertexAttribPointer(13, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, colour)));

        glEnableVertexAttribArray(14);
        glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, tex_coord)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)sizeof(int) * _numIndices, (const void*)_indices, _drawUsages);
        glBindVertexArray(0);

        auto* _geometry = new CompiledGeometryData;
        _geometry->texture = (GLuint)_texture;
        _geometry->vao = _vao;
        _geometry->vbo = _vbo;
        _geometry->ibo = _ibo;
        _geometry->draw_count = _numIndices;

        CheckGLError("CompileGeometry");

        return (Rml::CompiledGeometryHandle)_geometry;
    }

    void RmlOpenGLRenderer::RenderCompiledGeometry(Rml::CompiledGeometryHandle _handle, const Rml::Vector2f& _translation) {
        auto* _geometry = (CompiledGeometryData*)_handle;

        if (_geometry->texture) {
            glUseProgram(ShaderManager::get().getShader("rml"));
            glBindTexture(GL_TEXTURE_2D, _geometry->texture);

            GLint _location = glGetUniformLocation(ShaderManager::get().getShader("rml"), "viewProjectionMatrix");
            glUniformMatrix4fv(_location, 1, GL_FALSE, transform.data());

            _location = glGetUniformLocation(ShaderManager::get().getShader("rml"), "position");
            glUniform2fv(_location, 1, &_translation.x);
        } else {
            glUseProgram(ShaderManager::get().getShader("rmlColor"));
            glBindTexture(GL_TEXTURE_2D, 0);

            GLint _location = glGetUniformLocation(ShaderManager::get().getShader("rmlColor"), "viewProjectionMatrix");
            glUniformMatrix4fv(_location, 1, GL_FALSE, transform.data());

            _location = glGetUniformLocation(ShaderManager::get().getShader("rmlColor"), "position");
            glUniform2fv(_location, 1, &_translation.x);
        }

        glBindVertexArray(_geometry->vao);
        glDrawElements(GL_TRIANGLES, _geometry->draw_count, GL_UNSIGNED_INT, nullptr);

        CheckGLError("RenderCompiledGeometry");
    }

    void RmlOpenGLRenderer::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle _handle) {
        auto* _geometry = (CompiledGeometryData*)_handle;

        glDeleteVertexArrays(1, &_geometry->vao);
        glDeleteBuffers(1, &_geometry->vbo);
        glDeleteBuffers(1, &_geometry->ibo);

        delete _geometry;
    }

    void RmlOpenGLRenderer::EnableScissorRegion(bool _enable) {
        if (_enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void RmlOpenGLRenderer::SetScissorRegion(int _x, int _y, int _width, int _height) {
        glScissor(_x, viewportHeight - (_y + _height), _width, _height);
    }

#pragma pack(1)
    struct TGAHeader {
        char idLength;
        char colourMapType;
        char dataType;
        short int colourMapOrigin;
        short int colourMapLength;
        char colourMapDepth;
        short int xOrigin;
        short int yOrigin;
        short int width;
        short int height;
        char bitsPerPixel;
        char imageDescriptor;
    };
#pragma pack()

    bool RmlOpenGLRenderer::LoadTexture(Rml::TextureHandle& _textureHandle, Rml::Vector2i& _textureDimension, const Rml::String& _source) {
        Rml::FileInterface* _fileInterface = Rml::GetFileInterface();
        Rml::FileHandle _fileHandle = _fileInterface->Open(_source);
        if (!_fileHandle) {
            return false;
        }

        _fileInterface->Seek(_fileHandle, 0, SEEK_END);
        size_t _bufferSize = _fileInterface->Tell(_fileHandle);
        _fileInterface->Seek(_fileHandle, 0, SEEK_SET);

        if (_bufferSize <= sizeof(TGAHeader)) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Texture file size is smaller than TGAHeader, file is not a valid TGA image.");
            _fileInterface->Close(_fileHandle);
            return false;
        }

        using Rml::byte;
        byte* _buffer = new byte[_bufferSize];
        _fileInterface->Read(_buffer, _bufferSize, _fileHandle);
        _fileInterface->Close(_fileHandle);

        TGAHeader _header{};
        memcpy(&_header, _buffer, sizeof(TGAHeader));

        int _colorMode = _header.bitsPerPixel / 8;
        int _imageSize = _header.width * _header.height * 4; // We always make 32bit textures

        if (_header.dataType != 2) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
            delete[] _buffer;
            return false;
        }

        // Ensure we have at least 3 colors
        if (_colorMode < 3) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24 and 32bit textures are supported.");
            delete[] _buffer;
            return false;
        }

        const byte* _imageSrc = _buffer + sizeof(TGAHeader);
        byte* _imageDest = new byte[_imageSize];

        // Targa is BGR, swap to RGB and flip Y axis
        for (long _y = 0; _y < _header.height; _y++) {
            long _readIndex = _y * _header.width * _colorMode;
            long _writeIndex = ((_header.imageDescriptor & 32) != 0) ? _readIndex : (_header.height - _y - 1) * _header.width * 4;
            for (long x = 0; x < _header.width; x++) {
                _imageDest[_writeIndex] = _imageSrc[_readIndex + 2];
                _imageDest[_writeIndex + 1] = _imageSrc[_readIndex + 1];
                _imageDest[_writeIndex + 2] = _imageSrc[_readIndex];
                if (_colorMode == 4) {
                    const int _alpha = _imageSrc[_readIndex + 3];
#ifdef RMLUI_SRGB_PREMULTIPLIED_ALPHA_NO
                    _imageDest[_writeIndex + 0] = (_imageDest[_writeIndex + 0] * _alpha) / 255;
                    _imageDest[_writeIndex + 1] = (_imageDest[_writeIndex + 1] * _alpha) / 255;
                    _imageDest[_writeIndex + 2] = (_imageDest[_writeIndex + 2] * _alpha) / 255;
#endif
                    _imageDest[_writeIndex + 3] = (byte)_alpha;
                } else {
                    _imageDest[_writeIndex + 3] = 255;
                }

                _writeIndex += 4;
                _readIndex += _colorMode;
            }
        }

        _textureDimension.x = _header.width;
        _textureDimension.y = _header.height;

        bool _success = GenerateTexture(_textureHandle, _imageDest, _textureDimension);

        delete[] _imageDest;
        delete[] _buffer;

        return _success;
    }

    bool RmlOpenGLRenderer::GenerateTexture(Rml::TextureHandle& _textureHandle, const Rml::byte* _source, const Rml::Vector2i& _sourceDimensions) {
        GLuint _textureId = 0;
        glGenTextures(1, &_textureId);
        if (_textureId == 0) {
            LOG_E("Failed to generate texture.")
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, _textureId);

        GLint _internalFormat = GL_RGBA8;
        glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _sourceDimensions.x, _sourceDimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, _source);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        _textureHandle = (Rml::TextureHandle)_textureId;

        CheckGLError("GenerateTexture");

        return true;
    }

    void RmlOpenGLRenderer::ReleaseTexture(Rml::TextureHandle _textureHandle) {
        glDeleteTextures(1, (GLuint*)&_textureHandle);
    }

    void RmlOpenGLRenderer::SetTransform(const Rml::Matrix4f* _newTransform) {
        transform = projection * (_newTransform ? *_newTransform : Rml::Matrix4f::Identity());
    }

    void RmlOpenGLRenderer::SetViewport(int _width, int _height) {
        viewportWidth = _width;
        viewportHeight = _height;
    }

    void RmlOpenGLRenderer::BeginFrame() {
        glDisable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        projection = Rml::Matrix4f::ProjectOrtho(0, (float)viewportWidth, (float)viewportHeight, 0, -10000, 10000);
        SetTransform(&Rml::Matrix4f::Identity());
    }
}