// Created by borja on 27/2/22.

#include "core/render/elements/ViewPort.h"
#include "core/util/Functions.h"
#include "core/render/window/Window.h"
#include "core/render/Camera.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif

namespace RDE {

    ViewPort::ViewPort(const Vec2I& _deviceSize, Camera* _camera, const Window* _window) : window(_window), camera(_camera) {
		size = _deviceSize;
        landscape = size.x > size.y;
    }

    float ViewPort::getAspectRatio() const {
        return (float)size.x / (float)size.y;
    }

    void ViewPort::update() {
		auto _zoom = 1.f / camera->getCurrentZoomLevel();

		glDisable(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glViewport(0, 0, window->getWidth() * _zoom, window->getHeight() * _zoom);
		glScissor(position.x * _zoom, 
		          position.y * _zoom, 
		          size.x * _zoom, 
		          size.y * _zoom);
		glEnable(GL_SCISSOR_TEST);

    }

	Vec2I ViewPort::getSize() const {
		return size;
	}

	void ViewPort::setSize(const Vec2I& _size) {
		size = _size;
		update();
	}

	Vec2I ViewPort::getPosition() {
		return position;
	}

	void ViewPort::setPosition(const Vec2I& _position) {
		position = _position;
		update();
		camera->dirty = true;
	}

    void ViewPort::setUIScaleWeightsForWidthAndHeight(float _width, float _height) {
        if(_width > 1.f || _width < 0.f || _height > 1.f || _height < 0.f || _height + _width != 1.f) {
            Util::Log::warn("Input values for setting UI scale are ", Vec2F { _width, _height }, " but in some way they don't add up to 1! Not changing values.");
            return;
        }

        scaleWithWidth = _width;
    }

	void ViewPort::matchVirtualResolutionToDeviceResolution() {
		size = window->getWindowSize();
		landscape = size.x > size.y;
	}

    Vec2F ViewPort::getUIScaleWeights() const {
        return { scaleWithWidth, 1.f - scaleWithWidth };
    }
}