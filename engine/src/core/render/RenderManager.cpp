// Created by borja on 26/12/21.

#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/Transform.h"
#include "core/platform/PlatformHeaderSDLImage.h"
#include "core/render/RenderManager.h"
#include "core/Engine.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Functions.h"
#include "core/util/Vec.h"

namespace RDE {

    void RenderManager::init(Engine* _engine) {
    
        engine = _engine;

        #if !IS_MOBILE()
        Util::Log::info("OpenGL Version: ", glGetString(GL_VERSION), ", Vendor: ", glGetString(GL_VENDOR), ", GPU: ", glGetString(GL_RENDERER));
        #endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        #if !IS_MOBILE() 
            #ifndef __EMSCRIPTEN__
                glEnable(GL_PROGRAM_POINT_SIZE);
        		Util::GL::checkError("Invalid Point size");
                glEnable(GL_LINE_SMOOTH);
        		Util::GL::checkError("Invalid Line Smooth");
                glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
        		Util::GL::checkError("Invalid Line Smooth Hint -> GL_NICEST");
            #endif
		
            #if !IS_MAC()
                #ifndef __EMSCRIPTEN__
                    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
                    Util::GL::checkError("Invalid Point Smooth Hint -> GL_NICEST");
                #endif
            #endif

		#endif

        int _flags = IMG_INIT_PNG | IMG_INIT_JPG;
		auto _returned = IMG_Init(_flags);
		
		if(_returned != (IMG_INIT_PNG | IMG_INIT_JPG)){
			Util::Log::warn(SDL_GetError());
		}

        batch.init(_engine);
        batch.debug.init(&batch);
        Util::GL::checkError("RenderManager Initialization");

		engine->manager.shaderManager.loadShaderVertexConfig(FRAMEBUFFER_SHADER, {
			VertexConfig {
				0, 2, GL_FLOAT, 0, sizeof(glm::vec2) * 2
			},
			VertexConfig {
				1, 2, GL_FLOAT, sizeof(float) * 2, sizeof(glm::vec2) * 2
			}
		},{

        },
		1);
		Util::GL::checkError("FrameBuffer LoadConfig");

		FrameBufferSpecification _specs = {(uint32_t)engine->getWindow()->getWindowSize().x,(uint32_t)engine->getWindow()->getWindowSize().y};
		auto* _fb = new FrameBuffer(_specs, &engine->manager);
		defaultFramebufferID = _fb->getID();
		framebuffers[_fb->getID()] = _fb;
		framebufferIdConversionTable[1 << framebufferCounter++] = _fb->getID();
    }

	FramebufferID RenderManager::createFrameBuffer(const FrameBufferSpecification& _specs) {
		auto* _fb = new FrameBuffer(_specs, &engine->manager);
		framebuffers[_fb->getID()] = _fb;
		auto _externalValue = 1 << framebufferCounter++;
		framebufferIdConversionTable[_externalValue] = _fb->getID();
		return _externalValue;
	}

	void RenderManager::removeFrameBuffer(FramebufferID _id) {
		auto _it = framebuffers.find(framebufferIdConversionTable[_id]);
		if(_it != framebuffers.end()) {
			framebuffers.erase(_it);
		}
	}

	FrameBuffer* RenderManager::getFramebuffer(FramebufferID _id) {
		return framebuffers[framebufferIdConversionTable[_id]];
	}

	void RenderManager::onResize(uint32_t _width, uint32_t _height) {
		for(auto& _pair : framebuffers) {
			_pair.second->resize(_width, _height);
		}
	}

	void RenderManager::clearBatches() {
		batch.clearBatches();
	}

    void RenderManager::clear() {
        glClearColor((float)clearColor.r / 255.f, (float)clearColor.g / 255.f, (float)clearColor.b / 255.f, (float)clearColor.a / 255.f);
        Util::GL::checkError("RenderManager Clear");
        resetBuffers();
    }

    void RenderManager::resetBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderManager::beginDraw(Camera* _camera, Transform* _cameraTransform) {
        batch.beginDraw(_camera, _cameraTransform);
    }

    void RenderManager::drawPoint(const Vec2F& _position, const Color& _color) {
        batch.debug.drawPoint(_position, _color);
    }

    void RenderManager::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        batch.debug.drawLine(_p0, _p1, _color);
    }

    void RenderManager::endDraw() {
        batch.flush();
    }

	void RenderManager::flushFramebuffers() {
		for(auto& _frameBufferKeyPair : framebuffers) {
			_frameBufferKeyPair.second->flush();
		}
	}

    void RenderManager::drawSpriteRenderer(RenderizableInnerData& _innerData, Transform* _transform) {
        batch.drawSpriteRenderer(_innerData, _transform);
    }

    void RenderManager::drawTextRenderer(RenderizableInnerData& _innerData, Transform* _transform) {
        batch.drawTextRenderer(_innerData, _transform);
    }

    void RenderManager::endDrawUI(std::vector<Batch>& _batches) {
		batch.flushUI(_batches);
    }

    void RenderManager::beginDebugDraw(Camera* _camera, Transform* _cameraTransform, float _thickness) {
        batch.debug.setLinesThickness(_thickness);
        batch.beginDraw(_camera, _cameraTransform);
    }

    void RenderManager::endDebugDraw() {
        batch.debug.flushDebug();
    }

    void RenderManager::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        batch.debug.drawSquare(_position, _size, _color, _rotation);
    }

    void RenderManager::drawShape(DebugShape& _shape) {
        if(_shape.getPoints().empty()) return;
        batch.debug.drawShape(_shape);
    }

    void RenderManager::resetDebugInfo() {
        batch.drawCalls = 0;
        batch.uiDrawCalls = 0;
        batch.totalTriangles = 0;
    }

    int RenderManager::getTotalTriangles() {
        return batch.totalTriangles;
    }

    std::tuple<int, int> RenderManager::getDrawCalls() {
        return { batch.drawCalls, batch.uiDrawCalls };
    }

    void RenderManager::destroy() {
        Util::Log::debug("Cleaning up RenderManager");
        IMG_Quit();
    }

    void RenderManager::setClearColor(const Color& _color) {
        clearColor = _color;
    }

    Color RenderManager::getClearColor() {
        return clearColor;
    }

    void RenderManager::drawGrid(const Color& _color) {
        batch.debug.drawGrid(_color);
    }

    void RenderManager::setLineThickness(float _thickness) {
        batch.debug.setLinesThickness(_thickness);
    }

    void RenderManager::setPointSize(float _size) {
        batch.debug.setPointSize(_size);
    }



    void RenderManager::fillBackgroundCPUTexture(CPUTexture* _cpuTexture, const Color& _color) {
        for(auto _y = 0; _y < _cpuTexture->getSize().y; _y++) {
            for(auto _x = 0; _x < _cpuTexture->getSize().x; _x++) {
                _cpuTexture->setPixel(_x, _y, _color);
            }
        }
    }

    void RenderManager::drawPointToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _position, const Color& _color, int _size) {
        auto _halfTextureSize = _cpuTexture->getSize() / 2;
        auto _nodePos = _cpuTextureTransform->getModelMatrixPosition();

        for(int _y = -_size / 2; _y < _size; _y++) {
            for(int _x = -_size / 2; _x < _size; _x++) {
                if(_position.x + _x > _halfTextureSize.x || _position.x + _x < -_halfTextureSize.x || 
                   _position.y + _y > _halfTextureSize.y || _position.y + _y < -_halfTextureSize.y) {
                    continue;
                }
                
                _cpuTexture->setPixel(_halfTextureSize.x + _position.x + _x, _halfTextureSize.y +  _position.y + _y, _color);
            }
        }
    }

    void RenderManager::drawRectangleToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _position, const Vec2F& _size, const Color& _color) {
        auto _halfTextureSize = _cpuTexture->getSize() / 2;

        for(auto _y = -_size.y / 2; _y < _size.y; _y++) {
            for(auto _x = -_size.x / 2; _x < _size.x; _x++) {
                if(_position.x + _x > _halfTextureSize.x || _position.x + _x < -_halfTextureSize.x || 
                   _position.y + _y > _halfTextureSize.y || _position.y + _y < -_halfTextureSize.y) {
                    continue;
                }
                _cpuTexture->setPixel(_halfTextureSize.x + _position.x + _x - _size.x * 0.25f, _halfTextureSize.y + _position.y + _y - _size.y * 0.25f, _color);
            }
        }
    }

	RenderManager::~RenderManager() {
		for(auto& _pair : framebuffers) {
			delete _pair.second;
		}
	}
}
