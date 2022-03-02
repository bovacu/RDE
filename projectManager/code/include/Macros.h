// Created by borja on 2/3/22.


#ifndef ENGINE_MACROS_H
#define ENGINE_MACROS_H

#define INIT_CENTERED_WINDOW_WITH_SIZE(_width, _height) {                                                                                                                      \
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});                                                                              \
    ImGui::SetNextWindowSize({_width, _height}, ImGuiCond_Always);                                                                                           \
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));                              \
}
#define INIT_CENTERED_WINDOW INIT_CENTERED_WINDOW_WITH_SIZE(-1, -1)
#define END_CENTERED_WINDOW { ImGui::PopStyleVar(); }

#define INIT_MODAL(_modalID) { ImGui::OpenPopup(_modalID); }

#define YES_NO_MACRO_WITH_SIZE(_modalID, _close, _width, _height, _text, _yesText, _noText, _onYesAction, _onNoAction) {   \
    INIT_CENTERED_WINDOW_WITH_SIZE(_width, _height)                                             \
                                                                                                \
    if(ImGui::BeginPopupModal(_modalID, _close, ImGuiWindowFlags_NoResize)) {                   \
        static float _yesWidth = 0, _noWidth = 0;                                               \
        auto _textSize = ImGui::CalcTextSize(_text);                                            \
                                                                                                \
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textSize.x / 2.f);                \
        ImGui::Text("%s", _text);                                                               \
                                                                                                \
        ImGui::NewLine();                                                                       \
                                                                                                \
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - (_yesWidth + _noWidth) / 2.f);     \
        if(ImGui::Button(_yesText)) {                                                           \
            _onYesAction();                                                                     \
        }                                                                                       \
        _yesWidth = ImGui::GetItemRectSize().x;                                                 \
                                                                                                \
        ImGui::SameLine();                                                                      \
                                                                                                \
        if(ImGui::Button(_noText)) {                                                            \
            _onNoAction();                                                                      \
        }                                                                                       \
        _noWidth = ImGui::GetItemRectSize().x;                                                  \
        ImGui::EndPopup();                                                                      \
    }                                                                                           \
                                                                                                \
     END_CENTERED_WINDOW                                                                        \
}

#define YES_NO_MACRO(_modalID, _close, _text, _yesText, _noText, _onYesAction, _onNoAction) YES_NO_MACRO_WITH_SIZE(_modalID, _close, -1, -1, _text, _yesText, _noText, _onYesAction, _onNoAction)

#endif //ENGINE_MACROS_H
