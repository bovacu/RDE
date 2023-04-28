//
// Created by borja on 14/04/2022.
//

#ifndef RDE_RENDERIZABLE_H
#define RDE_RENDERIZABLE_H

#include "core/Core.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/components/ComponentBase.h"
#include "core/util/Color.h"
#include "core/util/Vec.h"
#include "core/util/Rect.h"
#include "core/render/elements/Batch.h"
#include "core/Enums.h"
#include <vector>

typedef uint32_t FramebufferID;

namespace RDE {

    struct OpenGLVertex;
    class IViewPort;
    class Transform;
	class Texture;

    typedef GLuint TextureID;

    struct DrawAndFlushData {
        std::vector<OpenGLVertex> vertices;
        std::vector<uint32_t> indices;
        ShaderID shaderID;
        GLuint textureID;
    };

    struct RenderizableInnerData {
        Texture* texture = nullptr;
        ShaderID shader = -1;
        Color color = Color::White;
        int layer = 0;
        std::vector<OpenGLVertex> vertices;
        bool dirty = false;
        RDE_BATCH_PRIORITY_ batchPriority = RDE_BATCH_PRIORITY_SPRITE;
		RDE_RENDERIZABLE_TYPE_ renderizableType = RDE_RENDERIZABLE_TYPE_NONE;
		bool draw = true;
		FramebufferID framebufferToRenderTo = 1;
        void* extraInfo = nullptr;
    };

    struct RenderizableInnerDataUI {
        Vec2F originOffset;
        RenderizableInnerData RenderizableInnerData;
        uint imageRenderingType = 0;
        float partialRenderingPercentage = 1.f;
        bool partialRenderingInverted = false;
    };

    struct CommonUIConfig {
        bool stopFurtherClicks = true;
    };


    #define RENDERIZABLE_BASIC_PROPERTIES()     \
    friend class Scene;                         \
    public:                                     \
        Node* node = nullptr;                   \
    private:                                    \
        RenderizableInnerData data;             




    #define RENDERIZABLE_BASIC_PROPERTIES_INITIALIZATION(_verticesSize, _shaderName, _batchPriority)    \
    node = _node;                                                                                       \
    data.shader = _manager->shaderManager.getShader(_shaderName)->getShaderID();                        \
	data.framebufferToRenderTo = _manager->renderManager.getDefaultFramebufferID();                     \
    data.batchPriority = _batchPriority;                                                                \
    data.vertices.resize(_verticesSize);                                                                




    #define RENDERIZABLE_UI_BASIC_PROPERTIES()  	\
    friend class Canvas;                        	\
    public:                                     	\
        Node* node = nullptr;                   	\
    private:                                    	\
        RenderizableInnerDataUI data;           	\
        UIInteractable* uiInteractable = nullptr;	\
		UIAnchoring* anchoring = nullptr;




    #define RENDERIZABLE_UI_BASIC_PROPERTIES_INITIALIZATION(_verticesSize, _shaderName, _batchPriority)     \
    node = _node;                                                                                           \
                                                                                                            \
    if(!_node->hasComponent<UIInteractable>()) {                                                            \
        uiInteractable = _node->addComponent<UIInteractable>();                                             \
    }                                                                                                       \
                                                                                                            \
    if(_config.stopFurtherClicks) {                                                                         \
        if(!node->hasComponent<CanvasEventStopper>()) {                                                     \
            node->addComponent<CanvasEventStopper>();                                                       \
        }                                                                                                   \
    } else {                                                                                                \
        if (node->hasComponent<CanvasEventStopper>()) {                                                     \
            node->removeComponent<CanvasEventStopper>();                                                    \
        }                                                                                                   \
    }                                                                                                       \
                                                                                                            \
    data.RenderizableInnerData.shader = _manager->shaderManager.getShader(_shaderName)->getShaderID();      \
    data.RenderizableInnerData.batchPriority = _batchPriority;                                              \
                                                                                                            \
    data.RenderizableInnerData.vertices.resize(_verticesSize);                                              \
	anchoring = node->getComponent<UIAnchoring>();															\






    #define RENDERIZABLE_BASIC_METHODS()                    		\
    friend class Graph;                                     		\
	RDE_FUNC_ND Texture* getTexture() const;              			\
	RDE_FUNC void setTexture(Texture* _texture);                    \
                                                            		\
	RDE_FUNC_ND Color getColor() const;                   			\
	RDE_FUNC void setColor(const Color& _color);                    \
                                                            		\
	RDE_FUNC_ND ShaderID getShaderID() const;             			\
	RDE_FUNC void setShaderID(ShaderID _shaderID);                  \
                                                            		\
	RDE_FUNC_ND int getLayer() const;                               \
	RDE_FUNC void setLayer(int _layer);                             \
                                                            		\
	RDE_FUNC_ND Vec2F getSize() const;                    			\
	RDE_FUNC_ND FloatRect getRegion() const;              			\
                                                            		\
	RDE_FUNC_ND RDE_BATCH_PRIORITY_ getBatchPriority() const;   	\
                                                            		\
	RDE_FUNC_ND bool isEnabled() const;                             \
	RDE_FUNC void setEnabled(bool _enabled);						\
																	\
	RDE_FUNC_ND FramebufferID getFramebuffer() const;				\
	RDE_FUNC void setFramebuffer(FramebufferID _id);




    #define RENDERIZABLE_BASIC_METHODS_IMPL(_className, _getSizeX, _getSizeY)                           \
    Texture* _className::getTexture() const { return data.texture; };                                   \
    void _className::setTexture(Texture* _texture) { data.texture = _texture; data.dirty = true; };     \
                                                                                                        \
    Color _className::getColor() const { return data.color; }                                           \
    void _className::setColor(const Color& _color) { data.color = _color; data.dirty = true; }          \
                                                                                                        \
    ShaderID _className::getShaderID() const { return data.shader; }                                    \
    void _className::setShaderID(ShaderID _shaderID) { data.shader = _shaderID; data.dirty = true; }    \
                                                                                                        \
    int _className::getLayer() const { return data.layer; }                                             \
    void _className::setLayer(int _layer) { data.layer = _layer; data.dirty = true; }                   \
                                                                                                        \
    Vec2F _className::getSize() const { return { _getSizeX, _getSizeY }; };                             \
    FloatRect _className::getRegion() const { return data.texture->getRegion(); };                      \
                                                                                                        \
    RDE_BATCH_PRIORITY_ _className::getBatchPriority() const { return data.batchPriority; }             \
                                                                                                        \
    bool _className::isEnabled() const { return !node->hasComponent<DisabledForRender>(); }             \
    void _className::setEnabled(bool _enabled) {                                                        \
        if(_enabled && node->hasComponent<DisabledForRender>()) {                                       \
            node->removeComponent<DisabledForRender>();                                                 \
            data.dirty = true;                                                                          \
            return;                                                                                     \
        }                                                                                               \
                                                                                                        \
        if(!_enabled && !node->hasComponent<DisabledForRender>()) {                                     \
            node->addComponent<DisabledForRender>();                                                    \
            data.dirty = true;                                                                          \
        }                                                                                               \
    }																									\
	FramebufferID _className::getFramebuffer() const { return data.framebufferToRenderTo; }				\
	void _className::setFramebuffer(FramebufferID _framebuffer) { data.framebufferToRenderTo = _framebuffer; data.dirty = true; }




    #define RENDERIZABLE_UI_BASIC_METHODS()     			\
    RENDERIZABLE_BASIC_METHODS()                			\
                                                			\
	RDE_FUNC void setSize(const Vec2F& _size);           	\
                                                			\
	RDE_FUNC_ND bool isInteractable() const;                \
	RDE_FUNC void setInteractable(bool _interactable);   	\
                                                			\
	RDE_FUNC void setOriginOffset(const Vec2F& _offset); 	\
	RDE_FUNC_ND Vec2F getOriginOffset() const;              




    #define ENABLED_DEFAULT_IMPL(_className)                                                                                                    \
    bool _className::isEnabled() const { return !node->hasComponent<DisabledForRender>(); }                                                     \
    void _className::setEnabled(bool _enabled) {                                                                                                \
        if(_enabled && node->hasComponent<DisabledForRender>()) {                                                                               \
            node->removeComponent<DisabledForRender>();                                                                                         \
            data.RenderizableInnerData.dirty = true;                                                                                            \
            return;                                                                                                                             \
        }                                                                                                                                       \
                                                                                                                                                \
        if(!_enabled && !node->hasComponent<DisabledForRender>()) {                                                                             \
            node->addComponent<DisabledForRender>();                                                                                            \
            data.RenderizableInnerData.dirty = true;                                                                                            \
        }                                                                                                                                       \
    }                           




    #define INTERACTABLE_DEFAULT_IMPL(_className)                                                                                               \
    bool _className::isInteractable() const {                                                                                                   \
        return uiInteractable->interactable;                                                                                                    \
    }                                                                                                                                           \
    void _className::setInteractable(bool _interactable) {                                                                                      \
        uiInteractable->interactable = _interactable;                                                                                           \
    }                                                                                                                                           \




    #define SIZE_METHODS_DEFAULT_IMPL(_className)                                                                                               \
    Vec2F _className::getSize() const {                                                                                                         \
        return anchoring->getSize();                                                                                 \
    }                                                                                                                                           \
                                                                                                                                                \
    void _className::setSize(const Vec2F &_size) {                                                                                              \
        anchoring->setSize(_size);                                                                                   \
    }




    #define RENDERIZABLE_UI_BASIC_METHODS_IMPL(_className, _getSizeX, _getSizeY, _setSizeCode)                                                  \
    Texture* _className::getTexture() const { return data.RenderizableInnerData.texture; };                                                     \
    void _className::setTexture(Texture* _texture) { data.RenderizableInnerData.texture = _texture; data.RenderizableInnerData.dirty = true; }; \
                                                                                                                                                \
    Color _className::getColor() const { return data.RenderizableInnerData.color; }                                                             \
    void _className::setColor(const Color& _color) { data.RenderizableInnerData.color = _color; data.RenderizableInnerData.dirty = true; }      \
                                                                                                                                                \
    ShaderID _className::getShaderID() const { return data.RenderizableInnerData.shader; }                                                      \
    void _className::setShaderID(ShaderID _shaderID) { data.RenderizableInnerData.shader = _shaderID; data.RenderizableInnerData.dirty = true; }\
                                                                                                                                                \
    int _className::getLayer() const { return data.RenderizableInnerData.layer; }                                                               \
    void _className::setLayer(int _layer) { data.RenderizableInnerData.layer = _layer; data.RenderizableInnerData.dirty = true; }               \
                                                                                                                                                \
    Vec2F _className::getSize() const { return { _getSizeX, _getSizeY }; };                                                                     \
    FloatRect _className::getRegion() const { return data.RenderizableInnerData.texture->getRegion(); };                                        \
                                                                                                                                                \
    RDE_BATCH_PRIORITY_ _className::getBatchPriority() const { return data.RenderizableInnerData.batchPriority; }                               \
                                                                                                                                                \
    ENABLED_DEFAULT_IMPL(_className)                                                                                                            \
                                                                                                                                                \
    void _className::setSize(const Vec2F& _size) {                                                                                              \
        do { _setSizeCode } while(0);                                                                                                           \
        data.RenderizableInnerData.dirty = true;                                                                                                \
    }                                                                                                                                           \
                                                                                                                                                \
    INTERACTABLE_DEFAULT_IMPL(_className)                                                                                                       \
                                                                                                                                                \
    void _className::setOriginOffset(const Vec2F& _offset) {                                                                                    \
        data.originOffset = _offset;																											\
		data.RenderizableInnerData.dirty = true;																								\
    }                                                                                                                                           \
    Vec2F _className::getOriginOffset() const {                                                                                                 \
        return data.originOffset;                                                                                                               \
    }              
}

#endif //RDE_RENDERIZABLE_H
