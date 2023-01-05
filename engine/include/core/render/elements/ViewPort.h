// Created by borja on 27/2/22.


#ifndef RDE_VIEWPORT_H
#define RDE_VIEWPORT_H

#include "core/util/Vec.h"

namespace RDE {

    /**
     * @brief This class represents the way the scene is going to be rendered and how much of the screen is going to be
     * used to drawBatched. This interface is the base of ant custom-viewport provided by the engine and must be followed
     * for any user-defined viewport.
     */

    class ViewPort {

        friend class Camera;
        friend class Canvas;

        private:
            /**
             * @brief Resolution that we want to render at.
             */
            Vec2I virtualResolution = {};

            /**
             * @brief Actual resolution of the Window or the device.
             */
            Vec2I deviceResolution = {};

            bool landscape = true;

            float scaleWithWidth = 0.5f;

        protected:
            /**
            * @brief Updates the device resolution.
            * @param _deviceSize Device resolution
            */
            virtual void update(const Vec2I& _deviceSize);

        public:
            ViewPort(const Vec2I& _deviceSize, const Vec2I& _resolutionSize);

            /**
             * @brief Returns the virtual resolution.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getVirtualResolution() const;

            /**
             * @brief Returns the aspect ratio we want to keep rewarding the physical screen size.
             * @return float
             */
            [[nodiscard]] float getVirtualAspectRatio() const;

            /**
             * @brief Returns the aspect ratio of the physical screen.
             * @return float
             */
            [[nodiscard]] float getPhysicalAspectRatio() const;

            /**
             * @brief Returns the device resolution.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getDeviceResolution() const;

            /**
             * Sets if the UI scales towards width screen changes, height screen changes or both.
             * @param _width Value between 1f and 0f.
             * @param _height The complement of _width to add up to 1f.
             */
            void setUIScaleWeightsForWidthAndHeight(float _width, float _height);

            /**
             * Returns the values set in setUIScaleWeightsForWidthAndHeight.
             * @return Vec2F
             */
            Vec2F getUIScaleWeights() const;

            bool isLandscape() const { return landscape; }

            virtual ~ViewPort() {};
    };
}

#endif //RDE_VIEWPORT_H
