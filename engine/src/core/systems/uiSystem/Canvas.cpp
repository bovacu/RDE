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

        if (!get().rmlData.rmlContext) {
            LOG_E("Rml context couldn't initialize")
            return;
        }

        struct FontFace {
            Rml::String filename;
            bool fallback_face;
        };
        FontFace _fontFaces[] = {
                { "LatoLatin-Regular.ttf",    false },
                { "LatoLatin-Italic.ttf",     false },
                { "LatoLatin-Bold.ttf",       false },
                { "LatoLatin-BoldItalic.ttf", false },
                { "NotoEmoji-Regular.ttf",    true  },
        };

        for (const FontFace& _fontFace : _fontFaces) {
            Rml::LoadFontFace("assets/" + _fontFace.filename, _fontFace.fallback_face);
        }

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
        if(!get().enabled) return;
        get().rmlData.rmlRenderer->BeginFrame();
        get().rmlData.rmlRenderer->SetViewport(_viewport.x, _viewport.y);
    }

    void Canvas::update(Delta _dt) {
        if(!get().enabled) return;
        get().rmlData.rmlContext->Update();
    }

    void Canvas::render() {
        if(!get().enabled) return;
        get().rmlData.rmlContext->Render();
    }

    void Canvas::destroy() {
        Rml::Shutdown();
        delete get().rmlData.rmlRenderer;
        delete get().rmlData.rmlSystemInterface;
    }

    RmlData& Canvas::getData() {
        return get().rmlData;
    }

    void Canvas::enable(bool _enable) {
        get().enabled = _enable;
    }
}