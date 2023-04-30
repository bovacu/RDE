//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIText.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/Node.h"
#include "core/systems/uiSystem/Canvas.h"

#define STARTING_VERTEX_SIZE 50

namespace RDE {

    UIText::UIText(Node* _node, Scene* _scene, Canvas* _canvas, const UITextConfig& _config) :
            UIText(_node, &_scene->engine->manager, _canvas->graph, _config) {  }

    UIText::UIText(Node* _node, Manager* _manager, Graph* _graph, const UITextConfig& _config) {

		RENDERIZABLE_UI_BASIC_PROPERTIES_INITIALIZATION(STARTING_VERTEX_SIZE, TEXT_RENDERER_SHADER, RDE_BATCH_PRIORITY_TEXT)

        font = _config.font == nullptr ? _manager->fontManager.getFont("MontserratRegular") : _config.font;
        innerText = _config.text;
		fontSize = _config.fontSize;
        recalcTextDimensions(innerText);
        setColor(_config.textColor);
        data.RenderizableInnerData.texture = font->getTexture();
        data.RenderizableInnerData.renderizableType = RDE_RENDERIZABLE_TYPE_UI_TEXT;

        node->getComponent<UIAnchoring>()->setSize(textSize);
    }

    void UIText::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void UIText::setFont(Font* _font) {
        font = _font;
    }

    Font* UIText::getFont() const {
        return font;
    }

    const std::string& UIText::getText() const {
        return innerText;
    }

    void UIText::recalcTextDimensions(const std::string& _text) {
        auto [_, _totalWidth, _totalHeight] = calculateLinesInfo(font->getChars());
        textSize.set(_totalWidth * 0.5f, _totalHeight * 0.5f);
		data.RenderizableInnerData.dirty = true;
    }

    void UIText::setFontSize(int _fontSize) {        
        fontSize = _fontSize;
        recalcTextDimensions(innerText);
    }

    int UIText::getFontSize() const {
        return fontSize;
    }

    float UIText::getSpacesBetweenChars() const {
        return spaceBetweenChars;
    }

    void UIText::setSpacesBetweenChars(float _spaceBetweenChars) {
        spaceBetweenChars = _spaceBetweenChars;
        recalcTextDimensions(innerText);
    }

    std::tuple<std::vector<UIText::LineInfo>, float, float> UIText::calculateLinesInfo(CharMap& _chars) const {
        auto _linesInfo = std::vector<LineInfo> {  };
        auto _ss = std::stringstream { innerText };

		auto _fontScale = fontSize / (float)DEFAULT_FONT_SIZE;
        auto _totalHeight = 0.0f;
        auto _totalWidth = 0.0f;
        for (std::string _line; std::getline(_ss, _line, '\n');) {
            LineInfo _lineInfo {
            	.line = _line
            };

            float _biggestHeight = 0.f;
            float _biggestWidth = 0.f;
            if(!_line.empty()) {
                for(auto _char : _line) {
					_biggestWidth += (float)_chars[_char].advance.x - (float)_chars[_char].offset.x;
					auto _currentHeight = (float)_chars[_char].size.y * _fontScale;
					_biggestHeight = _currentHeight > _biggestHeight ? _currentHeight : _biggestHeight;
                }
            }

            _lineInfo.biggestCharHeight = _biggestHeight;
            _linesInfo.push_back(_lineInfo);
            _totalHeight += _biggestHeight;
			_totalWidth = std::max(_totalWidth, _biggestWidth * _fontScale);
        }

		return std::tuple { _linesInfo, _totalWidth, _totalHeight };
    }

    Vec2F UIText::getTextSize() {
        return textSize;
    }

    float UIText::getNewLineSize() const {
        return newLineSize;
    }


    RENDERIZABLE_UI_BASIC_METHODS_IMPL(UIText, textSize.x, textSize.y, {})
    FRAMEBUFFER_METHODS_DEFAULT_IMPL(UIText)

	void UIText::setFramebuffer(FramebufferID _framebuffer) {
		data.RenderizableInnerData.framebufferToRenderTo = _framebuffer;
	}
}
