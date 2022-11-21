// Created by borja on 27/2/22.

#include "core/render/elements/ViewPort.h"
#include "core/util/Functions.h"

namespace RDE {

    ViewPort::ViewPort(const Vec2I& _deviceSize, const Vec2I& _resolutionSize) {
        virtualResolution = _resolutionSize;
        deviceResolution = _deviceSize;

        landscape = virtualResolution.x > virtualResolution.y;
    }

    Vec2I ViewPort::getVirtualResolution() const {
        return virtualResolution;
    }

    float ViewPort::getVirtualAspectRatio() const {
        return (float)virtualResolution.x / (float)virtualResolution.y;
    }

    float ViewPort::getPhysicalAspectRatio() const {
        return (float)deviceResolution.x / (float)deviceResolution.y;
    }

    Vec2I ViewPort::getDeviceResolution() const {
        return deviceResolution;
    }

    void ViewPort::update(const Vec2I& _deviceSize) {
        deviceResolution = _deviceSize;
    }

    void ViewPort::setUIScaleWeightsForWidthAndHeight(float _width, float _height) {
        if(_width > 1.f || _width < 0.f || _height > 1.f || _height < 0.f || _height + _width != 1.f) {
            Util::Log::warn("Input values for setting UI scale are ", Vec2F { _width, _height }, " but in some way they don't add up to 1! Not changing values.");
            return;
        }

        scaleWithWidth = _width;
    }

    Vec2F ViewPort::getUIScaleWeights() const {
        return { scaleWithWidth, 1.f - scaleWithWidth };
    }
}