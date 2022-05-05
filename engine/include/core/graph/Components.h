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

namespace GDE {

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
        GLuint shaderID = ShaderManager::get().getShader("basic");
        int layer = 0;

        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;
        explicit SpriteRenderer(Texture* _texture) : texture(_texture) {  }

        [[nodiscard]] Texture* getTexture() const override { return texture; }
        [[nodiscard]] int getLayer() const override { return layer; }
        [[nodiscard]] Color getColor() const override { return color; }
        [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }
    };

    class TextRenderer : public IRenderizable {

        friend class SpriteBatch;
        private:
            Font* font;
            std::string innerText;
            Vec2F size {};
            float spaceBetweenChars {};
            float spaceWidth {};
            float enterHeight {};
            int fontSize {};

            Color color = Color::White;
            Texture* texture = nullptr;
            GLuint shaderID = ShaderManager::get().getShader("basic");
            int layer = 0;

        [[nodiscard]] Texture* getTexture() const override { return texture; }
        [[nodiscard]] int getLayer() const override { return layer; }
        [[nodiscard]] Color getColor() const override { return color; }
        [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            TextRenderer(Font* _font, const std::string& _text);
            explicit TextRenderer(Font* _font);

            void setText(const std::string& _text);
            void setFont(Font* _font);

            [[nodiscard]] Font* getFont() const;
            [[nodiscard]] const std::string& getText() const;
            [[nodiscard]] Vec2F getTextSize() const;

            void setFontSize(int _fontSize);
            int getFontSize() const;

            void setSpaceWidth(float _spaceWidth);
            [[nodiscard]] float getSpaceWidth() const;

            [[nodiscard]] float getSpacesBetweenChars() const;
            void setSpacesBetweenChars(float _spaceBetweenChars);

            void setColor(const Color& _color);

    //        Shape& getDebugShape();
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

        public:
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
            bool isConstant() const;
    };


    enum BodyShapeType {
        BOX,
        CIRCLE,
        POLYGON
    };


    typedef unsigned int CollisionMask;
    struct Body {
        public:
            enum BodyType {
                DYNAMIC,
                STATIC,
                KINEMATIC
            };

            struct BodyConfig {
                float mass = 1;
                Vec2F size = { 64, 64 };
                float friction = 0;
                float restitution = 0;

                BodyType bodyType = BodyType::DYNAMIC;
                BodyShapeType bodyShapeType = BodyShapeType::BOX;
            };

            struct B2DConfig {
                b2BodyDef bodyDefinition;
                b2PolygonShape polygonShape;
                b2CircleShape circleShape;
                b2FixtureDef fixtureDef;
                b2Body* body;
            };

        friend class Physics;
        friend class Graph;

        private:
            B2DConfig b2dConfig;
            BodyConfig bodyConfig;

        public:
            explicit Body(const BodyConfig& _config, Transform* _transform);
            Body(const Body& _body) = default;
            ~Body();

            [[nodiscard]] Vec2F getPosition() const;
            [[nodiscard]] float getRotation() const;

            void updateBodyConfig(const BodyConfig& _bodyConfig, Transform* _transform = nullptr);
            [[nodiscard]] BodyConfig getConfig() const;

        private:
            b2BodyType gdeBodyTypeToB2dBodyType(const BodyType& _bodyType);
    };
}

#endif //ENGINE2_0_COMPONENTS_H
