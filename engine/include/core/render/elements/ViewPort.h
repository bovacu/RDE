// Created by borja on 27/2/22.


#ifndef RDE_VIEWPORT_H
#define RDE_VIEWPORT_H

#include "core/util/Vec.h"
#include <stdint.h>

namespace RDE {

    /**
     * @brief This class represents the way the scene is going to be rendered and how much of the screen is going to be
     * used to drawBatched. This interface is the base of ant custom-viewport provided by the engine and must be followed
     * for any user-defined viewport.
     */

	class Window;
    class Camera;
    class ViewPort {

        friend class Camera;
        friend class Canvas;

        private:
            Vec2I size;
            Vec2I position;

            bool landscape = true;
            float scaleWithWidth = 0.5f;

			const Window* window = nullptr;
			Camera* camera = nullptr;

        public:
            bool autoResizeWhenWindowSizeChanges = true;

        protected:
			RDE_FUNC void update();
            RDE_FUNC void onResize(uint32_t _width, uint32_t _height);

        public:
			RDE_FUNC ViewPort(const Vec2I& _deviceSize, Camera* _camera, const Window* _window);
			RDE_FUNC_ND Vec2I getSize() const;
			RDE_FUNC void setSize(const Vec2I& _size);


			RDE_FUNC void setPosition(const Vec2I& _pos);
			RDE_FUNC_ND Vec2I getPosition();

			RDE_FUNC_ND float getAspectRatio() const;
            /**
             * Sets if the UI scales towards width screen changes, height screen changes or both.
             * @param _width Value between 1f and 0f.
             * @param _height The complement of _width to add up to 1f.
             */
			RDE_FUNC void setUIScaleWeightsForWidthAndHeight(float _width, float _height);
			RDE_FUNC_ND Vec2F getUIScaleWeights() const;

			RDE_FUNC_ND bool isLandscape() const { return landscape; }
			RDE_FUNC void matchVirtualResolutionToDeviceResolution();
			RDE_FUNC virtual ~ViewPort() {};
    };
}

#endif //RDE_VIEWPORT_H
