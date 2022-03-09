// Created by borja on 20/1/22.


#ifndef ENGINE2_0_I_RENDERIZABLE_H
#define ENGINE2_0_I_RENDERIZABLE_H

#include "engine/include/core/scene/Components.h"
#include "ShaderManager.h"

namespace engine {
    class IRenderizable{
        public:
            virtual ShaderID getShaderID() = 0;
            virtual void setShader(ShaderID _shaderID) = 0;

            virtual Texture* getTexture() = 0;
            virtual void setTexture(Texture* _texture) = 0;
    };
}

#endif //ENGINE2_0_I_RENDERIZABLE_H
