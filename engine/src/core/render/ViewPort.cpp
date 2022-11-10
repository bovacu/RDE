// Created by borja on 27/2/22.

#include "core/render/elements/ViewPort.h"

namespace GDE {

    Vec2I IViewPort::getVirtualResolution() const {
        return virtualResolution;
    }

    float IViewPort::getAspectRatio() const {
        return virtualResolution.x >= virtualResolution.y ? (float)virtualResolution.x / (float)virtualResolution.y : (float)virtualResolution.y / (float)virtualResolution.x;
    }

    Vec2F IViewPort::getScalingFactor() const {
        return scalingFactor;
    }

    Vec2I IViewPort::getDeviceResolution() const {
        return deviceResolution;
    }


    // ---------------- FREE VIEWPORT


    FreeViewPort::FreeViewPort(const Vec2I& _windowSize) {
        virtualResolution = _windowSize;
        deviceResolution = _windowSize;
    }

    void FreeViewPort::update(const Vec2I& _deviceSize, bool _isLandscape) {
        deviceResolution = _deviceSize;
    }

    void FreeViewPort::updateVirtualResolution(const Vec2I& _virtualResolution) {  }


    // --------------- ADAPTIVE VIEWPORT


    AdaptiveViewPort::AdaptiveViewPort(const Vec2I& _virtualDesiredResolution) {
        virtualResolution = _virtualDesiredResolution;
    }

    void AdaptiveViewPort::update(const Vec2I& _deviceSize, bool _isLandscape) {
        deviceResolution = _deviceSize;
    }

    void AdaptiveViewPort::updateVirtualResolution(const Vec2I& _virtualResolution) {
        virtualResolution = _virtualResolution;
    }
}