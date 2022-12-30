//
// Created by borja on 9/5/22.
//

#include "core/graph/components/TextRenderer.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/util/Vec.h"

namespace RDE {

    TextRenderer::TextRenderer(Node* _node, Scene* _scene, const TextRendererConfig& _config) :
    TextRenderer(_node, &_scene->engine->manager, _scene->getMainGraph(), _config) {  }

    TextRenderer::TextRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const TextRendererConfig& _config) :
    TextRenderer(_node, &_scene->engine->manager, _canvas->getGraph(), _config) {  }

    TextRenderer::TextRenderer(Node* _node, Manager* _manager, Graph* _graph, const TextRendererConfig& _config) {
        
        RENDERIZABLE_BASIC_PROPERTIES_INITIALIZATION(50, TEXT_RENDERER_SHADER, BatchPriority::TextPriority)

        font = _config.font == nullptr ? _manager->fontManager.getDefaultFont("MontserratRegular") : _config.font;
        innerText = _config.text;
        recalcTextDimensions(innerText);
        data.texture = &font->getTexture();
        setColor(_config.color);
    }

    RENDERIZABLE_BASIC_METHODS_IMPL(TextRenderer, size.x / 2.f * node->getTransform()->getScale().x, size.y / 2.f * node->getTransform()->getScale().y)

    void TextRenderer::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void TextRenderer::setFont(Font* _font) {
        font = _font;
    }

    Font* TextRenderer::getFont() const {
        return font;
    }

    const std::string& TextRenderer::getText() const {
        return innerText;
    }

    Vec2F TextRenderer::getTextSize() const {
        return size;
    }

    void TextRenderer::recalcTextDimensions(const std::string& _text) {
        auto [_, _totalWidth, _totalHeight] = calculateLinesInfo(font->getChars());
        size.set(_totalWidth, _totalHeight);
    }

    void TextRenderer::setFontSize(int _fontSize) {
        Util::Log::warn("Set font size not working!!");
//        fontSize = _fontSize;
//        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
//        recalcTextDimensions(innerText);
    }

    int TextRenderer::getFontSize() const {
        return fontSize;
    }

    float TextRenderer::getSpacesBetweenChars() const {
        return spaceBetweenChars;
    }

    void TextRenderer::setNewLineSize(float _newLineSize) {
        newLineSize = _newLineSize;
        recalcTextDimensions(innerText);
    }

    float TextRenderer::getNewLineSize() const {
        return newLineSize;
    }

    void TextRenderer::setSpacesBetweenChars(float _spaceBetweenChars) {
        spaceBetweenChars = _spaceBetweenChars;
        recalcTextDimensions(innerText);
    }

    std::tuple<std::vector<TextRenderer::LineInfo>, float, float> TextRenderer::calculateLinesInfo(CharInfo* _chars) const {
        auto _linesInfo = std::vector<LineInfo> {  };
        auto _ss = std::stringstream { innerText };

        auto _totalHeight = 0.0f;
        auto _totalWidth = 0.0f;
        for (std::string _line; std::getline(_ss, _line, '\n');) {
            LineInfo _lineInfo {
                    .line = _line
            };

            float _biggestHeight = font->getBiggestCharHeight();
            float _biggestWidth = 0.f;
            if(!_line.empty()) {
                for(auto _char : _line) {
                    _biggestWidth += (float)_chars[_char].advance.x;
                }
            }

            _lineInfo.biggestCharHeight = _biggestHeight;
            _linesInfo.push_back(_lineInfo);
            _totalHeight += _biggestHeight;
            _totalWidth = std::max(_totalWidth, _biggestWidth);
        }

        return std::tuple {_linesInfo, _totalWidth, _totalHeight};
    }

}