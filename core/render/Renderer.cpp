// Created by borja on 24/12/21.

#include <glm/ext/matrix_transform.hpp>
#include "Renderer.h"

namespace engine {
    std::unique_ptr<Renderer::SceneData> Renderer::sceneData = std::make_unique<Renderer::SceneData>();

    void OpenGLMessageCallback(
            unsigned        _source,
            unsigned        _type,
            unsigned        _id,
            unsigned        _severity,
            int             _length,
            const char*     _message,
            const void*     _userParam) {

        switch (_severity) {
            case GL_DEBUG_SEVERITY_HIGH:         LOG_E(_message); return;
            case GL_DEBUG_SEVERITY_MEDIUM:       LOG_E(_message); return;
            case GL_DEBUG_SEVERITY_LOW:          LOG_W(_message); return;
            case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_I(_message); return;
            default : {  };
        }

//        ENGINE_CORE_ASSERT(false, "Unknown severity level!");
    }

    struct QuadVertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;
        float texIndex;
        float tilingFactor;
    };

    struct Render2DData {
        static int              numberOfVertices;
        static uint32_t         maxQuads;
        static uint32_t         maxVertices;
        static uint32_t         maxIndices;
        static const uint32_t   maxTextureSlots = 32;

        VertexArrayPtr  quadVertexArray;
        VertexBufferPtr quadVertexBuffer;
        ShaderPtr       textureShader;
        TexturePtr    whiteTexture;

        uint32_t quadIndexCount = 0;
        QuadVertex* quadVertexBufferBase = nullptr;
        QuadVertex* quadVertexBufferPtr = nullptr;

        std::array<TexturePtr, maxTextureSlots> textureSlots;
        uint32_t textureSlotIndex = 1; // 0 = white texture

        glm::vec4 quadVertexPositions[4]{};
//#if defined(ENGINE_DEBUG)
//        Render2D::Statistics stats;
//#endif
    };

    int Render2DData::numberOfVertices = 4;
    uint32_t Render2DData::maxQuads = 100000;
    uint32_t Render2DData::maxVertices = Render2DData::maxQuads * Render2DData::numberOfVertices;
    uint32_t Render2DData::maxIndices = Render2DData::maxQuads * (Render2DData::numberOfVertices + 2);

    static Render2DData data;

    void Renderer::initOpenGL() {
        /// Enabling some debugging info to get the most of an error.
        #ifdef ENGINE_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        #endif

        /// Enables transparency
        glEnable(GL_BLEND);

        /// Most typical blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /// Using this, caused the engine not to render alpha
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::init() {
        initOpenGL();

        data.quadVertexArray = VertexArray::create();

        data.quadVertexBuffer = VertexBuffer::create(engine::Render2DData::maxVertices * sizeof(QuadVertex));
        data.quadVertexBuffer->setLayout({

                 { ShaderDataType::Float3, "position_lay"      },
                 { ShaderDataType::Float4, "color_lay"         },
                 { ShaderDataType::Float2, "texCoord_lay"      },
                 { ShaderDataType::Float,  "texIndex_lay"      },
                 { ShaderDataType::Float,  "tilingFactor_lay"  }

                                         });

        data.quadVertexArray->addVertexBuffer(data.quadVertexBuffer);
        data.quadVertexBufferBase = new QuadVertex[engine::Render2DData::maxVertices];

        auto* _quadIndices = new uint32_t[engine::Render2DData::maxIndices];

        uint32_t _offset = 0;
        for (uint32_t i = 0; i < engine::Render2DData::maxIndices; i += 6) {
            _quadIndices[i + 0] = _offset + 0;
            _quadIndices[i + 1] = _offset + 1;
            _quadIndices[i + 2] = _offset + 2;

            _quadIndices[i + 3] = _offset + 2;
            _quadIndices[i + 4] = _offset + 3;
            _quadIndices[i + 5] = _offset + 0;

            _offset += 4;
        }

        IndexBufferPtr _quadIB = IndexBuffer::create(_quadIndices, engine::Render2DData::maxIndices);
        data.quadVertexArray->setIndexBuffer(_quadIB);
        delete[] _quadIndices;

        data.whiteTexture = Texture::create(1, 1, true);
        uint32_t _whiteTextureData = 0xffffffff;
        data.whiteTexture->setData(&_whiteTextureData, sizeof(uint32_t));

        int32_t _samplers[engine::Render2DData::maxTextureSlots];
        for (uint32_t i = 0; i < engine::Render2DData::maxTextureSlots; i++)
            _samplers[i] = i;

        data.textureShader = Shader::create(PIXEL_SHADER);
        data.textureShader->bind();
        data.textureShader->setIntArray("textures_uniform", _samplers, engine::Render2DData::maxTextureSlots);

        // Set all texture slots to 0
        data.textureSlots[0] = data.whiteTexture;

        data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }

    void Renderer::shutdown() {
        delete [] data.quadVertexBufferBase;
    }

    void Renderer::onWindowResize(uint32_t _width, uint32_t _height) {
        setViewport(0, 0, _width, _height);
    }

    void Renderer::setViewport(int _x, int _y, int _width, int _height) {
        glViewport(_x, _y, _width, _height);
    }

    void Renderer::setClearColor(const Color& _color) {
        glClearColor(_color.r / 255.f, _color.g / 255.f, _color.b / 255.f, _color.a / 255.f);
    }

    void Renderer::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::beginDrawCall(const Camera& _camera, glm::mat4& _transform) {
        glm::mat4 _viewProjectionMatrix = _camera.getProjectionMatrix() * glm::inverse(_transform);

        data.textureShader->bind();
        data.textureShader->setMat4("viewProjection_uniform", _viewProjectionMatrix);

        data.quadIndexCount = 0;
        data.quadVertexBufferPtr = data.quadVertexBufferBase;

        data.textureSlotIndex = 1;
    }

    void Renderer::endDrawCall() {
        auto dataSize = (uint32_t)( (uint8_t*)data.quadVertexBufferPtr - (uint8_t*)data.quadVertexBufferBase );
        data.quadVertexBuffer->setData(data.quadVertexBufferBase, dataSize);
        flush();
    }

    void Renderer::flushAndReset() {
        endDrawCall();
        data.quadIndexCount = 0;
        data.quadVertexBufferPtr = data.quadVertexBufferBase;
        data.textureSlotIndex = 1;
    }

    void Renderer::flush() {
        if (data.quadIndexCount == 0)
            return; // Nothing to draw

        // Bind textures
        for (uint32_t i = 0; i < data.textureSlotIndex; i++)
            data.textureSlots[i]->bind(i);

        drawIndexed(data.quadVertexArray, data.quadIndexCount);
    }

    void Renderer::drawIndexed(const VertexArrayPtr& _vertexArray, uint32_t _indexCount) {
        uint32_t _count = _indexCount ? _indexCount : _vertexArray->getIndexBuffer()->getCount();
        glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::submit(const ShaderPtr& _shader, const VertexArrayPtr& _vertexArray, const glm::mat4& _transform) {
        _shader->bind();
        _shader->setMat4("u_ViewProjection", sceneData->viewProjectionMatrix);
        _shader->setMat4("u_Transform", _transform);

        _vertexArray->bind();
        drawIndexed(_vertexArray);
    }

    void Renderer::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color, float _thickness) {
        const float _dX = _p0.x - _p1.x;
        const float _dY = _p0.y - _p1.y;
        float _angle = std::atan2(_dY, _dX);

        constexpr size_t        _quadVertexCount = 4;
        const float             _textureIndex = 0.0f; // White Texture
        constexpr glm::vec2     _textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float             _tilingFactor = 1.0f;

        if (data.quadIndexCount >= Render2DData::maxIndices)
            flushAndReset();

        glm::vec3 _position = {(_p1.x + _p0.x) / 2.f, (_p1.y + _p0.y) / 2.f, 0.0f};
        glm::vec3 _size     = {_p0.distance(_p1), _thickness, 0.0f};

        glm::mat4 _transform = glm::translate(glm::mat4(1.0f), _position)
                               * glm::rotate(glm::mat4(1.0f), _angle, { 0.0f, 0.0f, 1.0f })
                               * glm::scale(glm::mat4(1.0f), _size);

        for (size_t _i = 0; _i < _quadVertexCount; _i++) {
            data.quadVertexBufferPtr->position = _transform * data.quadVertexPositions[_i];
            data.quadVertexBufferPtr->color = glm::vec4{ (float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f };;
            data.quadVertexBufferPtr->texCoord = _textureCoords[_i];
            data.quadVertexBufferPtr->texIndex = _textureIndex;
            data.quadVertexBufferPtr->tilingFactor = _tilingFactor;
            data.quadVertexBufferPtr++;
        }

        data.quadIndexCount += 6;
    }

    void Renderer::drawShape(Shape& _shape, const Color& _color, float _thickness) {
        Vec2F _p0, _p1;

        for(int _v = 0; _v < _shape.getVertices().size(); _v++) {

            if(_v < _shape.getVertices().size() - 1) {
                _p0 = _shape.getVertices()[_v];
                _p1 = _shape.getVertices()[_v + 1];
            } else {
                _p0 = _shape.getVertices()[_v];
                _p1 = _shape.getVertices()[0];
            }

            constexpr size_t _quadVertexCount = 4;
            const float _textureIndex = 0.0f; // White Texture
            constexpr glm::vec2 _textureCoords[] = {{0.0f, 0.0f},
                                                    {1.0f, 0.0f},
                                                    {1.0f, 1.0f},
                                                    {0.0f, 1.0f}};
            const float _tilingFactor = 1.0f;

            if (data.quadIndexCount >= Render2DData::maxIndices)
                flushAndReset();

            glm::vec3 _position = {(_p1.x + _p0.x) / 2.f, (_p1.y + _p0.y) / 2.f, 0.0f};
            glm::vec3 _size = {_p0.distance(_p1), _thickness, 0.0f};

            glm::mat4 _transform = glm::translate(glm::mat4(1.0f), _position)
                                   * glm::rotate(glm::mat4(1.0f), _shape.getAngles()[_v], {0.0f, 0.0f, 1.0f})
                                   * glm::scale(glm::mat4(1.0f), _size);

            for (size_t _i = 0; _i < _quadVertexCount; _i++) {
                data.quadVertexBufferPtr->position = _transform * data.quadVertexPositions[_i];
                data.quadVertexBufferPtr->color = glm::vec4{ (float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f };;
                data.quadVertexBufferPtr->texCoord = _textureCoords[_i];
                data.quadVertexBufferPtr->texIndex = _textureIndex;
                data.quadVertexBufferPtr->tilingFactor = _tilingFactor;
                data.quadVertexBufferPtr++;
            }

            data.quadIndexCount += 6;
        }
    }

    void Renderer::drawRectangle(const glm::mat4& _transform, const Color& _color) {
        constexpr size_t    _quadVertexCount = 4;
        const float         _textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 _textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float         _tilingFactor = 1.0f;

        if (data.quadIndexCount >= Render2DData::maxIndices)
            flushAndReset();

        for (size_t _i = 0; _i < _quadVertexCount; _i++) {
            data.quadVertexBufferPtr->position = _transform * data.quadVertexPositions[_i];
            data.quadVertexBufferPtr->color = glm::vec4{ (float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f };
            data.quadVertexBufferPtr->texCoord = _textureCoords[_i];
            data.quadVertexBufferPtr->texIndex = _textureIndex;
            data.quadVertexBufferPtr->tilingFactor = _tilingFactor;
            data.quadVertexBufferPtr++;
        }

        data.quadIndexCount += 6;
    }

    void Renderer::drawRectangle(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation, const Color& _tintColor) {
        if(_rotation > 0)
            _drawRotatedRect(_position, _size, _rotation, _color, _tintColor);
        else
            _drawRect(_position, _size, _color, _tintColor);
    }

    void Renderer::drawTexture(const Vec2F& _position, const Vec2F& _size, const TexturePtr& _texture, float _rotation, const Color& _tintColor) {
//        Render2D::drawTexture(_position, _size, _texture, _rotation, _tintColor);
    }

    void Renderer::drawTexture(const Vec2F& _position, const Vec2F& _size, const TextureRegionPtr& _textureRegion, float _rotation, const Color& _tintColor) {
//        Render2D::drawTexture(_position, _size, _textureRegion, _rotation, _tintColor);
    }

    void Renderer::_drawRect(const Vec2F& _position, const Vec2F& _size, const Color& _color, const Color& _tintColor) {

    }

    void Renderer::_drawRotatedRect(const Vec2F& _position, const Vec2F& _size, float _rotation, const Color& _color, const Color& _tintColor) {

    }
}