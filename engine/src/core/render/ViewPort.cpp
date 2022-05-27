// Created by borja on 27/2/22.

#include "core/render/elements/ViewPort.h"

namespace GDE {

    Vec2I IViewPort::getVirtualResolution() {
        return virtualResolution;
    }

    float IViewPort::getAspectRatio() const {
        return aspectRatio;
    }

    Vec2F IViewPort::getScalingFactor() {
        return scalingFactor;
    }

    Vec2I IViewPort::getDeviceResolution() {
        return deviceResolution;
    }


    // ---------------- FREE VIEWPORT


    FreeViewPort::FreeViewPort(const Vec2I& _windowSize) {
        virtualResolution = _windowSize;
        deviceResolution = _windowSize;
    }

    void FreeViewPort::update(const Vec2I& _deviceSize) {
        virtualResolution = _deviceSize;
        aspectRatio = (float)_deviceSize.x / (float)_deviceSize.y;
        scalingFactor = {1, 1};
        deviceResolution = _deviceSize;
    }

    void FreeViewPort::updateVirtualResolution(const Vec2I& _virtualResolution) {  }


    // --------------- ADAPTIVE VIEWPORT


    AdaptiveViewPort::AdaptiveViewPort(const Vec2I& _virtualDesiredResolution) {
        virtualResolution = _virtualDesiredResolution;
    }

    void AdaptiveViewPort::update(const Vec2I& _deviceSize) {
        scalingFactor = {(float)_deviceSize.x / (float)virtualResolution.x, (float)_deviceSize.y / (float)virtualResolution.y};
        aspectRatio = (float)_deviceSize.x / (float)_deviceSize.y;
        deviceResolution = _deviceSize;
    }

    void AdaptiveViewPort::updateVirtualResolution(const Vec2I& _virtualResolution) {
        virtualResolution = _virtualResolution;
    }
}