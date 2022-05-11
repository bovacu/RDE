//
// Created by borja on 09/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include <RmlUi/Debugger.h>

namespace GDE {

    void Canvas::init(int _width, int _height) {
        get().rmlData.rmlRenderer = new RmlOpenGLRenderer;
        get().rmlData.rmlSystemInterface = new RmlUiSDL2SystemInterface;
        Rml::SetRenderInterface(get().rmlData.rmlRenderer);
        Rml::SetSystemInterface(get().rmlData.rmlSystemInterface);

        if (!Rml::Initialise()) {
            LOG_E("Rml couldn't initialize")
            return;
        }

        get().rmlData.rmlContext = Rml::CreateContext("default", Rml::Vector2i(_width, _height));
        Rml::Debugger::Initialise(get().rmlData.rmlContext);
        Rml::Debugger::SetVisible(true);

        get().rmlData.rmlDocument = get().rmlData.rmlContext->LoadDocument("assets/demo.rml");

        if (get().rmlData.rmlDocument) {
            get().rmlData.rmlDocument->Show();
            LOG_S("Document loaded")
        } else {
            LOG_E("Document is nullptr")
        }

        LOG_S("Rml successfully initialized")
    }

    Canvas& Canvas::get() {
        static Canvas _canvas;
        return _canvas;
    }

    void Canvas::beginFrame(const Vec2I _viewport) {
        get().rmlData.rmlRenderer->BeginFrame();
        get().rmlData.rmlRenderer->SetViewport(_viewport.x, _viewport.y);
    }

    void Canvas::update(Delta _dt) {
        get().rmlData.rmlContext->Update();
    }

    void Canvas::render() {
        get().rmlData.rmlContext->Render();
    }

    void Canvas::destroy() {
        Rml::Shutdown();
        delete get().rmlData.rmlRenderer;
        delete get().rmlData.rmlSystemInterface;
        delete get().rmlData.rmlDocument;
    }

    RmlData& Canvas::getData() {
        return get().rmlData;
    }
}