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
#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"

namespace GDE {

    class Manager; class Window; class Camera; class Canvas; class IViewPort;

    /**
     * @brief Component common to every entity and that identifies it whit a name. It is recommended 
     * every entity to have a different one.
     */
    struct Tag {
        public:
            /**
            * @brief Name of the entity.
            */
            std::string tag;

            Tag() = default;
            Tag(const Tag& _tag) = default;
            Tag(const NodeID& _nodeId, const std::string& _tag) : tag(_tag) {  }

            explicit operator std::string& () { return tag; }
    };



    /**
     * @brief Component that tells the internal ECS system if an entity should or not be updated in the different systems.
     */
    struct Active {
        explicit Active(const NodeID& _nodeId) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };



    /**
     * @brief Component that allows to render a texture or part of a texture on screen. End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    struct SpriteRenderer : public IRenderizable {

        friend class SpriteBatch;

        private:
            /**
             * @brief Texture that contains the sprite. This is the whole SpriteSheet, but it is limited by the region and 
             * only its region will be rendered.
             */
            Texture* texture = nullptr;

            /**
             * @deprecated Not of use anymore, needs to go.
             */
            IViewPort* viewport = nullptr; // TODO remove this, is not used anymore

        public:
            explicit SpriteRenderer(const NodeID& _nodeId, Scene* _scene);
            SpriteRenderer(const NodeID& _nodeId, Scene* _scene, Texture* _texture);

            /**
             * @brief Sets the texture. This should be taken from the TextureAtlasManager.
             * @return Vec2 with {width, height}.
             * @see TextureAtlasManager
             */
            void setTexture(Texture* _texture) { texture = _texture; }

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


            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2I getSize() const override { return texture->getSize(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }
    };



    /**
     * @brief Component used to render text on screen.  End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class TextRenderer : public IRenderizable {

        friend class SpriteBatch;

        private:
            /**
             * @brief The font used to render the text.
             */
            Font* font;

            /**
             * @brief Rendered text.
             */
            std::string innerText;

            /**
             * @brief Size of the whole text as a rectangle.
             */
            Vec2F size {};

            /**
             * @brief Amount of space between each character.
             */
            float spaceBetweenChars {};

            /**
             * @brief Size of the space character.
             */
            float spaceWidth {};

            /**
             * @brief Height of the enter character.
             */
            float enterHeight {};

            /**
             * @brief Size of the font.
             */
            int fontSize {};

            /**
             * @brief The texture of the generated font.
             */
            Texture* texture = nullptr;

            /**
             * @deprecated This needs to go.
             */
            IViewPort* viewport = nullptr;
            

        private:
            /**
             * @brief Recalculates the dimensions of the new text as a rectangle.
             * 
             * @param _text the inner text.
             */
            void recalcTextDimensions(const std::string& _text);

        public:
            TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text);
            TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font);

            /**
             * @brief Sets the text to be rendered.
             * 
             * @param _text The text to render.
             */
            void setText(const std::string& _text);

            /**
             * @brief Sets the font.
             * 
             * @param _font The font.
             */
            void setFont(Font* _font);

            /**
             * @brief Get sthe font.
             * 
             * @return Font* 
             */
            [[nodiscard]] Font* getFont() const;

            /**
             * @brief Gets the rendered text
             * 
             * @return const std::string& 
             */
            [[nodiscard]] const std::string& getText() const;

            /**
             * @brief Gets the dimensions of the rendered text as a rectangle.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getTextSize() const;

            /**
             * @brief Sets the font size.
             * 
             * @param _fontSize Size of the font.
             */
            void setFontSize(int _fontSize);

            /**
             * @brief Gets the font size.
             * 
             * @return int 
             */
            [[nodiscard]] int getFontSize() const;

            /**
             * @brief Sets the space character width.
             * 
             * @param _spaceWidth Space character width.
             */
            void setSpaceWidth(float _spaceWidth);

            /**
             * @brief Get the space character width.
             * 
             * @return float 
             */
            [[nodiscard]] float getSpaceWidth() const;

            /**
             * @brief Gets the space between characters.
             * 
             * @return float 
             */
            [[nodiscard]] float getSpacesBetweenChars() const;

            /**
             * @brief Sets the spaces between characters.
             * 
             * @param _spaceBetweenChars Space between characters.
             */
            void setSpacesBetweenChars(float _spaceBetweenChars);

            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2I getSize() const override { return texture->getSize(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }
    };



    /**
     * @brief Component that helps the internal system work quickier. This component should be added to entities that won't move at all during
     * the scene rendering and life cycle.
     * 
     */
    struct StaticTransform {
        explicit StaticTransform(const NodeID& _nodeId);

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };



    /**
     * @brief Component common to every entity that tells the engine where it is, which scale it has and how much it is rotated.
     * 
     */
    struct Transform {
        // Local Space
        private:
            /**
             * @brief 3D (although just 2D coords are used) of where the entity is, related to its parent, in WorldSpace (but relative to its parent).
             * 
             */
            glm::vec3 localPosition {0.0f, 0.0f, 0.0f};

            /**
             * @brief 3D  (although just 2D coords are used) of the entity's scale.
             * 
             */
            glm::vec3 localScale { 1.0f, 1.0f, 1.0f };

            /**
             * @brief Rotation of the entity.
             * 
             */
            float localRotation = 0.0f;

            /**
             * @brief Flag that tells the internal ECS system if an object needs or not to be updated. This saves lots of computing resources.
             * 
             */
            bool dirty = false;

        public:
            explicit Transform(const NodeID& _nodeId);

            /**
             * @brief ID of the direct parent.
             * 
             */
            NodeID parent;

            /**
             * @brief All the children that an entity has.
             * 
             */
            std::vector<NodeID> children;

            /**
             * @brief 4x4 matrix containing the WorldSpace position, rotation and scale. Is the result of ParentModelMatrix * localModelMatrix.
             * 
             */
            glm::mat4 modelMatrix = glm::mat4(1.0f);

            /**
             * @brief Same as modelMatrix but relative to its parent.
             * 
             */
            glm::mat4 localModelMatrix = glm::mat4(1.0f);

            /**
             * @brief Calculates the new local model matrix of the entity.
             * 
             * @return glm::mat4 
             */
            glm::mat4 getLocalModelMatrix();

            /**
             * @brief This method updates the position, rotation and scale of the entity, modifying its local and model matrices.
             * 
             * @param _graph The main Graph of the scene.
             */
            void update(Graph* _graph);

            /**
             * @brief Sets the position of the object in Local Coordintes, so relative to its parent.
             * 
             * @param _position Vector with position.
             */
            void setPosition(const Vec2F& _position);

            /**
             * @brief Sets the position of the object in Local Coordintes, so relative to its parent.
             * 
             * @param _x Position x
             * @param _y Position y
             */
            void setPosition(float _x, float _y);

            /**
             * @brief Returns the position relative to its parent.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getPositionLocal() const;

            /**
             * @brief Returns the position relative to the World.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getPositionWorld() const;

            /**
             * @brief Translates the current position by an amount. It is not the same as setPosition.
             * 
             * @param _direction The direction it is moving to.
             */
            void translate(const Vec2F& _direction);

            /**
             * @brief Translates the current position by an amount. It is not the same as setPosition.
             * 
             * @param _x Direction in x it is moving to
             * @param _y Direction in y it is moving to
             */
            void translate(float _x, float _y);

            /**
             * @brief Sets the rotation of the entity in Local value.
             * 
             * @param _rotation Degrees of rotation.
             */
            void setRotation(float _rotation);

            /**
             * @brief Returns the rotation relative to its parent.
             * 
             * @return float 
             */
            [[nodiscard]] float getRotationLocal() const;

            /**
             * @brief Returns the rotation relative to the World.
             * 
             * @return float 
             */
            [[nodiscard]] float getRotationWorld() const;

            /**
             * @brief Rotates the entity by an amount of degrees. It is not the same as setRotation.
             * 
             * @param _amount Amount of degrees to rotate.
             */
            void rotate(float _amount);

            /**
             * @brief Sets the scale of the entity in Local value.
             * 
             * @param _scale Scale of the entity.
             */
            void setScale(const Vec2F& _scale);

            /**
             * @brief Sets the scale of the entity in Local value.
             * 
             * @param _x Scale in x
             * @param _y Scale in y
             */
            void setScale(float _x, float _y);

            /**
             * @brief Returns the scale relative to its parent.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getScaleLocal() const;

            /**
             * @brief Returns the scale relative to the World.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getScaleLWorld() const;

            /**
             * @brief Adds an amount to the current scale. It is not the same as setScale.
             * 
             * @param _scale The amount to increase or decrease the scale.
             */
            void scale(const Vec2F& _scale);

            /**
             * @brief Adds an amount to the current scale. It is not the same as setScale.
             * 
             * @param _x The amount to increase/decrease the scale in x
             * @param _y The amount to increase/decrease the scale in y
             */
            void scale(float _x, float _y);
    };



    /**
     * @brief Component used to give physics properties to an entity. It includes both having gravity and the ability to collide with other elements.
     * 
     */
    struct Body {

        public:
            /**
             * @see B2DConfig
             */
            B2DConfig b2dConfig;

            /**
             * @see BodyConfig
             */
            BodyConfig bodyConfig;

        public:
            explicit Body(const NodeID& _nodeId, Scene* _scene, const BodyConfig& _config, Transform* _transform);
            Body(const Body& _body) = default;
            ~Body();

            /**
             * @brief Gets the position of the body in WorldSpace.
             * 
             * @return Vec2F 
             */
            [[nodiscard]] Vec2F getPosition() const;

            /**
             * @brief Gets the rotation of the Body.
             * 
             * @return float 
             */
            [[nodiscard]] float getRotation() const;

            /**
             * @brief Sets if this body should be affected by gravity or not.
             * 
             * @param applyGravity 
             */
            void setApplyGravity(bool applyGravity) const;

            /**
             * @brief Returns if the current Body is affected by gravity or not.
             * 
             * @return true 
             * @return false 
             */
            [[nodiscard]] bool isApplyingGravity() const;

            /**
             * @brief Sets a scalar to increase or decrease the gravity that affects this Body.
             * 
             * @param _gravityMultiplier 
             */
            void setGravityMultiplier(float _gravityMultiplier) const;

            /**
             * @brief Gets the scalar that increases or decreases the gravity of this Body.
             * 
             * @return float 
             */
            [[nodiscard]] float getGravityMultiplayer() const;

            /**
             * @brief Sets if the collision should just trigger the callback or if also needs to simulate the collision.
             * 
             * @param _sensor 
             */
            void setSensor(bool _sensor) const;

            /**
             * @brief Returns if the Body just triggers the collision callback.
             * 
             * @return true 
             * @return false 
             */
            [[nodiscard]] bool isSensor() const;

            /**
             * @brief Sets the mask of the Body, so PhysicsManager can understand what kind of Body this is and calculate correctly the collision detection.
             * 
             * @param _mask 
             */
            void setSelfCollisionMask(CollisionMask _mask) const;

        private:
            /**
             * @brief Conversion from GDE's BodyType to Box2D's BodyType
             * 
             * @param _bodyType 
             * @return b2BodyType 
             */
            b2BodyType gdeBodyTypeToB2dBodyType(const BodyType& _bodyType);
    };



    /**
     * @brief Component that every UI element (Buttons, UITexts...) should follow to be rendered correctly by the engine.
     * 
     */
    struct UI : public IRenderizable {
        protected:
            /**
             * @brief Texture that contains the sprite of the rendered UI.
             * 
             */
            Texture* texture = nullptr;

        public:
            friend class SpriteBatch;

            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2I getSize() const override { return texture->getSize(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }

            ~UI() = default;
    };



    /**
     * @brief Component that every UI element that wants to interact with events on the screen must include.
     * 
     */
    struct UIInteractable {

        friend class Canvas;
        
        /**
         * @brief Sets the function that will allow the system to know when any of the below callbacks should be triggered or not.
         * Usually this function will be if the mouse is inside the rectangles bound of the UI element.
         * 
         * It is not set by default by the engine and MUST be provided by the end-user.
         * 
         */
        UDelegate<bool(NodeID, Canvas*)> interactionTrigger;

        /**
         * @brief Callback triggered when the mouse is clicked.
         * 
         */
        MDelegate<void(MouseCode)> onClick;

        /**
         * @brief Callback triggered when the mouse has just entered.
         * 
         */
        MDelegate<void()> onMouseEntered;

        /**
         * @brief Callback triggered when the mouse has just exited.
         * 
         */
        MDelegate<void()> onMouseExited;

        /**
         * @brief Callback triggered when the mouse is scrolled.
         * 
         */
        MDelegate<void(Vec2F)> onScroll;

        /**
         * @brief Callback triggered when a key is pressed.
         * 
         */
        MDelegate<void(KeyCode)> onKeyPressed;

        /**
         * @brief Callback triggered when a gamepad button is pressed.
         * 
         */
        MDelegate<void(ControllerButtons)> onGamepadButtonPressed;

        /**
         * @brief Callback triggered when the mobile screen is touched.
         * 
         */
        MDelegate<void(int)> onMobileClick;

        /**
         * @brief Order of execution of the different UI elements.
         * 
         */
        int priority = 0;

        explicit UIInteractable(const NodeID& _nodeId);

        private:
            /**
             * @brief Status of the mouse entering and exiting.
             * 
             */
            enum MouseStatus {
                MouseEntered,
                MouseExited
            };

            /**
             * @brief Current mouse status.
             * 
             */
            MouseStatus mouseStatus = MouseStatus::MouseExited;

            /**
             * @brief Function called inside the Graph system to take the events and handle them inside the UI elements.
             * 
             * @param _nodeID Entity that has the UI element
             * @param _eventDispatcher Event dispatcher
             * @param _event Event being handled
             * @param _canvas Canvas that contains the Entity
             */
            void onEvent(const NodeID& _nodeID, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas);
    };



    /**
     * @brief Component that is based on the SpriteRenderer, but allows to render UI images with a different size than the original
     * without loosing quality. The setup of this NinePatch should be done via TexturePacker.
     * 
     */
    struct NinePatchSprite : public UI {

        /**
         * @brief This is the size we want the UI to be rendered, may or may not match UI::getSize().
         *
         */
        Vec2I ninePatchSize;

        /**
         * @see UIInteractable
         *
         */
        UIInteractable* interaction = nullptr;

        NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas);
        NinePatchSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture);

        /**
         * @brief Returns the NinePatch data. This is almost sure not to be needed by end-users.
         * 
         * @return NinePatch& 
         */
        [[nodiscard]] NinePatch& getNinePatch() const;
    };



    /**
     * @brief Component used to render UI text on the screen. 
     * 
     */
    struct UIText : public UI {
        TextRenderer* text;
        UIInteractable* interaction = nullptr;

        UIText(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const std::string& _text);
        ~UIText() = default;
    };



    /**
     * @brief Component used to render and interact with a button within the UI.
     * 
     */
    struct UIButton : public UI {
        UIText* text = nullptr;
        NinePatchSprite* background = nullptr;
        UIInteractable* interaction = nullptr;

        public:
            UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _buttonTexture, const std::string& _text);
            ~UIButton() = default;
    };


    /**
     * @brief Component used to block the event that the entity containing this component handled. If an entity
     * handles an event without this component, the event will propagate in deeper elements.
     * 
     */
    struct CanvasEventStopper {
        explicit CanvasEventStopper(const NodeID& _nodeId);

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };
}

#endif //ENGINE2_0_COMPONENTS_H
