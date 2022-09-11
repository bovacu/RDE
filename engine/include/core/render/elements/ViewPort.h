// Created by borja on 27/2/22.


#ifndef GDE_VIEWPORT_H
#define GDE_VIEWPORT_H

#include "core/util/Util.h"

namespace GDE {

    /**
     * @brief This class represents the way the scene is going to be rendered and how much of the screen is going to be
     * used to draw. This interface is the base of ant custom-viewport provided by the engine and must be followed
     * for any user-defined viewport.
     */
    class IViewPort {
        protected:
            /**
             * @brief Resolution that we want to render at.
             */
            Vec2I virtualResolution = {};

            /**
             * @brief Actual resolution of the Window or the device.
             */
            Vec2I deviceResolution = {};

            /**
             * @brief Scalar that scales up or down the scene to be rendered according to the IViewPort implementation.
             */
            Vec2F scalingFactor = {1, 1};

            /**
             * @brief Aspect ratio of the Window or device screen.
             */
            float aspectRatio = -1;

        public:
            /**
             * @brief Returns the virtual resolution.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getVirtualResolution() const;

            /**
             * @brief Returns the aspect ration.
             * @return float
             */
            [[nodiscard]] float getAspectRatio() const;

            /**
             * @brief Returns the scaling factor.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getScalingFactor() const;

            /**
             * @brief Returns the device resolution.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getDeviceResolution() const;

            /**
             * @brief Updates the device resolution.
             * @param _deviceSize Device resolution
             */
            virtual void update(const Vec2I& _deviceSize) = 0;

            /**
             * @brief Updates the virtual resolution.
             * @param _virtualResolution Virtual resolution
             */
            virtual void updateVirtualResolution(const Vec2I& _virtualResolution) = 0;

            virtual ~IViewPort() {};
    };

    /**
     * @brief Make everything scale with the aspect-ratio of the device size and virtual device
     */
    class AdaptiveViewPort : public IViewPort {
        public:
            explicit AdaptiveViewPort(const Vec2I& _virtualDesiredResolution);

            /**
             * @see IViewPort
             */
            void update(const Vec2I& _deviceSize) override;

            /**
             * @see IViewPort
             */
            void updateVirtualResolution(const Vec2I& _virtualResolution) override;

            ~AdaptiveViewPort() override {}
    };

    /**
     * @brief The bigger the device screen, the more you show, the smaller, the less
     */
    class FreeViewPort : public IViewPort {
        public:
            explicit FreeViewPort(const Vec2I & _windowSize);

            /**
             * @see IViewPort
             */
            void update(const Vec2I& _deviceSize) override;

            /**
             * @see IViewPort
             */
            void updateVirtualResolution(const Vec2I& _virtualResolution) override;

            ~FreeViewPort() override {}
    };

    /**
     * @brief This one is the one that sets the black lines
     * @warning Not implemented yet
     */
    class FixedViewPort : public IViewPort {

    };
}

#endif //GDE_VIEWPORT_H
