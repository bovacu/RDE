// Created by borja on 27/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include "core/util/Util.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/Graph.h"
#include "glm/ext/matrix_transform.hpp"
#include "core/systems/uiSystem/FontManager.h"
#include "core/render/elements/IRenderizable.h"
#include "box2d/box2d.h"
#include "core/systems/physicsSystem/Physics.h"

namespace GDE {

    class Manager;
    class Window;

    struct Tag {
        std::string tag;

        Tag() = default;
        Tag(const Tag& _tag) = default;
        explicit Tag(const std::string& _tag) : tag(_tag) {  }

        explicit operator std::string& () { return tag; }
    };

    struct Active {
        bool active;

        Active() = default;
        Active(const Active& _active) = default;
        explicit Active(bool _active) : active(_active) {  }
    };

    struct SpriteRenderer : public IRenderizable {
        Color color = Color::White;
        Texture* texture = nullptr;
        GLuint shaderID = -1;
        int layer = 0;

        explicit SpriteRenderer(Manager* _manager);
        SpriteRenderer(Manager* _manager, Texture* _texture);

        [[nodiscard]] Texture* getTexture() const override { return texture; }
        [[nodiscard]] int getLayer() const override { return layer; }
        [[nodiscard]] Color getColor() const override { return color; }
        [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }
    };

    class TextRenderer : public IRenderizable {

        friend class SpriteBatch;
        public:
            Color color = Color::White;
            GLuint shaderID = -1;
            int layer = 0;

        private:
            Font* font;
            std::string innerText;
            Vec2F size {};
            float spaceBetweenChars {};
            float spaceWidth {};
            float enterHeight {};
            int fontSize {};
            Texture* texture = nullptr;

        [[nodiscard]] Texture* getTexture() const override { return texture; }
        [[nodiscard]] int getLayer() const override { return layer; }
        [[nodiscard]] Color getColor() const override { return color; }
        [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            TextRenderer(Manager* _manager, Font* _font, const std::string& _text);
            TextRenderer(Manager* _manager, Font* _font);

            void setText(const std::string& _text);
            void setFont(Font* _font);

            [[nodiscard]] Font* getFont() const;
            [[nodiscard]] const std::string& getText() const;
            [[nodiscard]] Vec2F getTextSize() const;

            void setFontSize(int _fontSize);
            [[nodiscard]] int getFontSize() const;

            void setSpaceWidth(float _spaceWidth);
            [[nodiscard]] float getSpaceWidth() const;

            [[nodiscard]] float getSpacesBetweenChars() const;
            void setSpacesBetweenChars(float _spaceBetweenChars);
    };

    struct StaticTransform {
        bool foo;
    };

    struct Transform {
        // Local Space
        private:
            glm::vec3 localPosition {0.0f, 0.0f, 0.0f};
            glm::vec3 localScale { 1.0f, 1.0f, 1.0f };
            float localRotation = 0.0f;
            bool constant = false;
            Window* window;

        public:
            explicit Transform(Window* _window);
            NodeID parent;
            std::vector<NodeID> children;

            // World Space
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glm::mat4 localModelMatrix = glm::mat4(1.0f);

            glm::mat4 getLocalModelMatrix();
            void update(Graph* _graph);

            void setPosition(const Vec2F& _position);
            void setPosition(float _x, float _y);
            [[nodiscard]] Vec2F getPositionLocal() const;
            [[nodiscard]] Vec2F getPositionWorld() const;
            void translate(const Vec2F& _direction);
            void translate(float _x, float _y);

            void setRotation(float _rotation);
            [[nodiscard]] float getRotationLocal() const;
            [[nodiscard]] float getRotationWorld() const;
            void rotate(float _amount);

            void setScale(const Vec2F& _scale);
            void setScale(float _x, float _y);
            [[nodiscard]] Vec2F getScaleLocal() const;
            [[nodiscard]] Vec2F getScaleLWorld() const;
            void scale(const Vec2F& _scale);
            void scale(float _x, float _y);

            void setConstant(bool _constant);
            [[nodiscard]] bool isConstant() const;
    };


    struct Body {
        public:
            B2DConfig b2dConfig;
            BodyConfig bodyConfig;

        public:
            explicit Body(const BodyConfig& _config, Transform* _transform);
            Body(const Body& _body) = default;
            ~Body();

            [[nodiscard]] Vec2F getPosition() const;
            [[nodiscard]] float getRotation() const;

            void setApplyGravity(bool applyGravity) const;
            [[nodiscard]] bool isApplyingGravity() const;

            void setGravityMultiplier(float _gravityMultiplier) const;
            [[nodiscard]] float getGravityMultiplayer() const;

            void setSensor(bool _sensor) const;
            [[nodiscard]] bool isSensor() const;

            void setSelfCollisionMask(CollisionMask _mask) const;

        private:
            b2BodyType gdeBodyTypeToB2dBodyType(const BodyType& _bodyType);
    };
}

#endif //ENGINE2_0_COMPONENTS_H
