// Created by borja on 27/2/22.


#ifndef ENGINE_VIEWPORT_H
#define ENGINE_VIEWPORT_H

#include "core/util/Util.h"

namespace GDE {

    class IViewPort {
        protected:
            Vec2I virtualResolution = {};
            Vec2I deviceResolution = {};
            Vec2F scalingFactor = {1, 1};
            float aspectRatio = -1;

        public:
            [[nodiscard]] Vec2I getVirtualResolution();
            [[nodiscard]] float getAspectRatio() const;
            [[nodiscard]] Vec2F getScalingFactor();
            [[nodiscard]] Vec2I getDeviceResolution();
            virtual void update(const Vec2I& _deviceSize) = 0;
            virtual void updateVirtualResolution(const Vec2I& _virtualResolution) = 0;
            virtual ~IViewPort() = default;
    };

    // Make everything scale with the aspect-ratio of the device size and virtual device
    class AdaptiveViewPort : public IViewPort {
        public:
            explicit AdaptiveViewPort(const Vec2I& _virtualDesiredResolution);
            void update(const Vec2I& _deviceSize) override;
            void updateVirtualResolution(const Vec2I& _virtualResolution) override;
            ~AdaptiveViewPort() override {  };
    };

    // The bigger the device screen, the more you show, the smaller, the less
    class FreeViewPort : public IViewPort {
        public:
            explicit FreeViewPort(const Vec2I & _windowSize);
            void update(const Vec2I& _deviceSize) override;
            void updateVirtualResolution(const Vec2I& _virtualResolution) override;
            ~FreeViewPort() override {  };
    };

    // This one is the one that sets the black lines
    class FixedViewPort : public IViewPort {

    };
}

#endif //ENGINE_VIEWPORT_H
