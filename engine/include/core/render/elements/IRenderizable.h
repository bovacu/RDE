//
// Created by borja on 14/04/2022.
//

#ifndef ENGINE_I_RENDERIZABLE_H
#define ENGINE_I_RENDERIZABLE_H

#include "Texture.h"
#include "ShaderManager.h"

namespace engine {

    class IRenderizable {
        public:
            [[nodiscard]] virtual Texture* getTexture() const = 0;
            [[nodiscard]] virtual int getLayer() const = 0;
            [[nodiscard]] virtual Color getColor() const = 0;
            [[nodiscard]] virtual ShaderID getShaderID() const = 0;
    };

}

#endif //ENGINE_I_RENDERIZABLE_H
