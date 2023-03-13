//
// Created by vazqu on 12/19/2022.
//

#include "core/util/Functions.h"
#include "core/render/elements/Texture.h"
#include "core/render/window/Window.h"
#include "core/render/elements/ViewPort.h"
#include "core/platform/PlatformHeaderSDL.h"
#include "core/platform/PlatformHeaderSDLImage.h"

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif

namespace RDE::Util::Math {
	Vec2F worldToScreenCoords(const ViewPort* _viewport, const Vec2F& _position) {
        auto _windowSize = _viewport->getVirtualResolution();
        return {_position.x * _viewport->getVirtualAspectRatio() / ((float)_windowSize.x * 0.5f), _position.y / ((float)_windowSize.y * 0.5f) };
    }

    Vec2F screenToWorldCoords(const ViewPort* _viewport, const Vec2F& _position) {
        auto _windowSize = _viewport->getVirtualResolution();
        return {_position.x / _viewport->getVirtualAspectRatio() * ((float)_windowSize.x * 0.5f),
                _position.y * ((float)_windowSize.y * 0.5f) };
    }

    Vec2F worldToScreenSize(const ViewPort* _viewport, const Vec2F& _size) {
        auto _virtualRes = _viewport->getVirtualResolution();
        return {_size.x * _viewport->getVirtualAspectRatio() / ((float)_virtualRes.x),
                _size.y / ((float)_virtualRes.y) };
    }

    Vec2F worldToScreenCoordsUI(const ViewPort* _viewport, const Vec2F& _position) {
        auto _physicalRes = _viewport->getDeviceResolution();
        return {_position.x * _viewport->getPhysicalAspectRatio() / ((float)_physicalRes.x * 0.5f), _position.y / ((float)_physicalRes.y * 0.5f) };
    }

    Vec2F worldToScreenSizeUI(const ViewPort* _viewport, const Vec2F& _size) {
        auto _physicalRes = _viewport->getDeviceResolution();
        return {_size.x * _viewport->getPhysicalAspectRatio() / ((float)_physicalRes.x), _size.y / ((float)_physicalRes.y) };
    }

    void worldToScreenSize(const ViewPort* _viewport, float& _x, float& _y) {
        auto _new = Util::Math::worldToScreenCoords(_viewport, {_x, _y});
        _x = _new.x;
        _y = _new.y;
    }

    void screenToWorldCoords(const ViewPort* _viewport, float& _x, float& _y) {
        auto _new = screenToWorldCoords(_viewport, {_x, _y});
        _x = _new.x;
        _y = _new.y;
    }
}

namespace RDE::Util::GL {
    void checkError(const std::string& _message)  {
        #ifdef ENGINE_DEBUG
            GLenum err;
            while((err = glGetError()) != GL_NO_ERROR){
                Util::Log::error("GL_ERROR ", _message," -> ", err);
            }
        #endif
    }
}

namespace RDE::Util::Graphics {

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 rmask = 0xff000000;
        Uint32 gmask = 0x00ff0000;
        Uint32 bmask = 0x0000ff00;
        Uint32 amask = 0x000000ff;  
    #else
        Uint32 rmask = 0x000000ff;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x00ff0000;
        Uint32 amask = 0xff000000;
    #endif

	void takeScreenshot(Window& _window, const std::string& _path) {
		takeScreenshot(_window, {0, 0}, _window.getWindowSize(), _path);
	}

	unsigned char* getScreenPixels(Window& _window) {
		return getAreaOfScreenPixels(_window, {0, 0}, _window.getWindowSize());
	}

	void takeScreenshot(Window& _window, const Vec2I& _position, const Vec2I& _size, const std::string& _path) {
		auto* _pixels = new unsigned char[4 * _size.x * _size.y];
        memset(_pixels, 0, 4 * _size.x * _size.y);

		glReadPixels((int)(_window.getWindowSize().x * 0.5f + _position.x - _size.x * 0.5f ), (int)(_window.getWindowSize().y * 0.5f + _position.y - _size.y * 0.5f),
		 _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);

		auto* _screenSurface = SDL_CreateRGBSurfaceFrom(_pixels, _size.x, _size.y, 32, _size.x * 4, rmask, gmask, bmask, amask);
        Util::Texture::invertSDLSurface(_screenSurface);

        if(!_screenSurface) {
            Util::Log::error("Error getting pixels for screenshot: ", SDL_GetError());
            return;
        }

	    auto _extension = Util::String::getFileExtension(_path);
	    if(_extension == "png") {
	    	if(IMG_SavePNG(_screenSurface, _path.c_str()) != 0) {
		    	Util::Log::error("Error saving png screenshot: ", SDL_GetError());
	            return;
		    } else {
		    	Util::Log::info("Screenshot saved at: ", _path);
		    }
	    } else if(_extension == "jpg" || _extension == "jpeg") {
	    	if(IMG_SaveJPG(_screenSurface, _path.c_str(), 100) != 0) {
		    	Util::Log::error("Error saving jpg screenshot: ", SDL_GetError());
	            return;
		    } else {
		    	Util::Log::info("Screenshot saved at: ", _path);
		    }
	    } else if(_extension == "bmp") {
	    	if(SDL_SaveBMP(_screenSurface, _path.c_str()) != 0) {
		    	Util::Log::error("Error saving bmp screenshot: ", SDL_GetError());
	            return;
		    } else {
		    	Util::Log::info("Screenshot saved at: ", _path);
		    }
	    } else {
	    	Util::Log::error("Tried to save screenshot with an unsupported format, use [png, jpg, jpeg, bmp].", _path);
	    }

	    SDL_FreeSurface(_screenSurface);
	}

	unsigned char* getAreaOfScreenPixels(Window& _window, const Vec2I& _position, const Vec2I& _size) {
		auto* _pixels = new unsigned char[4 * _window.getWindowSize().x * _window.getWindowSize().y];
        memset(_pixels, 0, 4 * _window.getWindowSize().x * _window.getWindowSize().y);
		glReadPixels((int)(_window.getWindowSize().x * 0.5f + _position.x - _size.x * 0.5f ), (int)(_window.getWindowSize().y * 0.5f + _position.y - _size.y * 0.5f),
		 _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
		return _pixels;
	}

	CPUTexture* pixelsToCPUTexture(const Vec2I& _size, unsigned char* _pixels, RDE_IMAGE_EXTENSION_ _imageType = RDE_IMAGE_EXTENSION_PNG) {
		auto* _cpuTexture = new CPUTexture();
       _cpuTexture->init(_size.x, _size.y, _pixels, _imageType);
       return _cpuTexture;
	}

}