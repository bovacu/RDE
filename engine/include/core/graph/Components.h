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
#include "core/render/window/keysAndButtons/KeyboardKeys.h"
#include "core/render/window/keysAndButtons/MouseKeys.h"
#include "core/render/window/keysAndButtons/GamePadButtons.h"

namespace GDE {

    class Manager; class Window; class Camera; class Canvas; class IViewPort;

    struct Tag {
        std::string tag;

        Tag() = default;
        Tag(const Tag& _tag) = default;
        Tag(const NodeID& _nodeId, const std::string& _tag) : tag(_tag) {  }

        explicit operator std::string& () { return tag; }
    };

    struct Active {
        explicit Active(const NodeID& _nodeId) {  }

        private:
            short foo;
    };

    struct SpriteRenderer : public IRenderizable {

        friend class SpriteBatch;

        Color color = Color::White;
        GLuint shaderID = -1;
        int layer = 0;

        private:
            Texture* texture = nullptr;
            IViewPort* viewport = nullptr; // TODO remove this, is not used anymore

        public:
            explicit SpriteRenderer(const NodeID& _nodeId, Scene* _scene);
            SpriteRenderer(const NodeID& _nodeId, Scene* _scene, Texture* _texture);

            [[nodiscard]] Vec2F getSize() const;

            void setTexture(Texture* _texture) { texture = _texture; }
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }
            [[nodiscard]] int getLayer() const override { return layer; }
            [[nodiscard]] Color getColor() const override { return color; }
            [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }
            [[nodiscard]] std::string getTexturePath();
            [[nodiscard]] std::string getTextureName();
            [[nodiscard]] std::string getTextureExtension();
            void updateViewport(IViewPort* _viewport) override;
            [[nodiscard]] Vec2I getTextureSize() const override { return texture->getSize(); }
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }
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
            IViewPort* viewport = nullptr;

        public:
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }
            [[nodiscard]] int getLayer() const override { return layer; }
            [[nodiscard]] Color getColor() const override { return color; }
            [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }
            void updateViewport(IViewPort* _viewport) override { viewport = _viewport; }
            [[nodiscard]] Vec2I getTextureSize() const override { return texture->getSize(); }
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }

        private:
            void recalcTextDimensions(const std::string& _text);

        public:
            TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text);
            TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font);

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
        explicit StaticTransform(const NodeID& _nodeId);
        private:
            short foo;
    };

    struct Transform {
        // Local Space
        private:
            glm::vec3 localPosition {0.0f, 0.0f, 0.0f};
            glm::vec3 localScale { 1.0f, 1.0f, 1.0f };
            float localRotation = 0.0f;
            bool constant = false;

        public:
            explicit Transform(const NodeID& _nodeId);

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
            explicit Body(const NodeID& _nodeId, const BodyConfig& _config, Transform* _transform);
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

    struct UI : public IRenderizable {
        protected:
            Texture* texture = nullptr;

        public:
            friend class SpriteBatch;

            Color color = Color::White;
            GLuint shaderID = -1;
            int layer = 0;

            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }
            [[nodiscard]] int getLayer() const override { return layer; }
            [[nodiscard]] Color getColor() const override { return color; }
            [[nodiscard]] ShaderID getShaderID() const override { return shaderID; }
            [[nodiscard]] Vec2I getTextureSize() const override { return texture->getSize(); }
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }
            void updateViewport(IViewPort* _viewport) override {  }

            ~UI() = default;
    };

    struct UIInteractable {

        friend class Canvas;

        UDelegate<bool(NodeID, Canvas*)> interactionTrigger;
        MDelegate<void(MouseCode)> onClick;
        MDelegate<void(Vec2F)> onScroll;
        MDelegate<void(KeyCode)> onKeyPressed;
        MDelegate<void(GamePadButtons)> onGamepadButtonPressed;
        MDelegate<void(int)> onMobileClick;
        bool enabled = true;
        int priority = 0;

        explicit UIInteractable(const NodeID& _nodeId);

        private:
            void onEvent(const NodeID& _nodeID, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas);
    };

    struct NinePatchSprite : public UI {

        /// This is the size we want the UI to be rendered
        Vec2I size;
        UIInteractable* interaction = nullptr;

        NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas);
        NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture);

        [[nodiscard]] Vec2I getSize() const;
        [[nodiscard]] NinePatch& getNinePatch() const;
    };

    struct UIText : public UI {
        TextRenderer* text;
        UIInteractable* interaction = nullptr;

        UIText(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const std::string& _text);
        ~UIText() = default;
    };

    struct UIButton : public UI {
        UIText* text = nullptr;
        NinePatchSprite* background = nullptr;
        UIInteractable* interaction = nullptr;

        public:
            UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _buttonTexture, const std::string& _text);
            ~UIButton() = default;
    };

    struct CanvasEventStopper {
        explicit CanvasEventStopper(const NodeID& _nodeId);
        private:
            short foo;
    };
}

#endif //ENGINE2_0_COMPONENTS_H
