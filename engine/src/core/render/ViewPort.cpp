// Created by borja on 27/2/22.

#include "core/render/elements/ViewPort.h"

namespace GDE {

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
}