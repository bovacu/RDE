//
// Created by borja on 14/04/2022.
//

#ifndef ENGINE_I_RENDERIZABLE_H
#define ENGINE_I_RENDERIZABLE_H

#include "Texture.h"
#include "ShaderManager.h"

namespace GDE {

    class IViewPort;
    class IRenderizable {
        public:
            [[nodiscard]] virtual GLuint getTexture() const = 0;
            [[nodiscard]] virtual int getLayer() const = 0;
            [[nodiscard]] virtual Color getColor() const = 0;
            [[nodiscard]] virtual ShaderID getShaderID() const = 0;
            virtual void updateViewport(IViewPort* _viewport) = 0;
    };

}

#endif //ENGINE_I_RENDERIZABLE_H
