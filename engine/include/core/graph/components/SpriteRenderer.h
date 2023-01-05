//
// Created by borja on 9/5/22.
//

#ifndef RDE_SPRITE_RENDERER_H
#define RDE_SPRITE_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/Texture.h"
#include "core/render/elements/Vertex.h"
#include "core/util/Vec.h"
#include "entt/entity/entity.hpp"
#include <glm/fwd.hpp>

typedef entt::entity NodeID;

namespace RDE {

    class Manager; 
    class Graph;
    class Scene;
    class Canvas;

    struct Node;

    struct SpriteRendererConfig {
        Texture* texture = nullptr;
        Color color = Color::White;
    };

    /**
     * @brief Component that allows to render a texture or part of a texture on screen. End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class SpriteRenderer {

        friend class UICheckbox;
        friend class UIButton;
        friend class UIInput;
        friend class UISlider;

        RENDERIZABLE_BASIC_PROPERTIES()

        public:
            SpriteRenderer(Node* _node, Scene* _scene, const SpriteRendererConfig& _config = {});
            SpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const SpriteRendererConfig& _config = {});
            SpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const SpriteRendererConfig& _config = {});
            ~SpriteRenderer() {  }

            RENDERIZABLE_BASIC_METHODS()

            /**
             * @brief Gets the path where the SpriteSheet is stored.
             * @return std::string
             */
            [[nodiscard]] std::string getTexturePath();

            /**
             * @brief Gets the name of the file od the SpriteSheet without the extension.
             * @return std::string
             */
            [[nodiscard]] std::string getTextureName();

            /**
             * @brief Gets the extension of the file of the SpriteSheet.
             * @return std::string
             */
            [[nodiscard]] std::string getTextureExtension();
    };

}

#endif //RDE_SPRITE_RENDERER_H
