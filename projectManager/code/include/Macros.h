// Created by borja on 2/3/22.


#ifndef ENGINE_MACROS_H
#define ENGINE_MACROS_H

#include "libs/json.h"
using namespace nlohmann;
#include "ProjectManagerLayer.h"


namespace engine {

    #define SAVE_CONFIG(_globalConfig) {                                     \
        json _j = *(_globalConfig);                                          \
        FilesSystem::writeChunkToFile((_globalConfig)->handler, _j.dump(4)); \
    }

    #define LOAD_CONFIG(_globalConfig) {                                                        \
        auto _config = FilesSystem::readFullFile((_globalConfig)->handler).content;             \
        json _configJson = json::parse(_config);                                                \
        LOG_W(_configJson.dump(4))                                                              \
        auto _c = _configJson.get<GlobalConfig>();                                              \
        *(_globalConfig) = _c;                                                                  \
        for(auto& _project : (_globalConfig)->projects)                                         \
            _project.stillExists = FilesSystem::fileExists(_project.projectPath + "/.config");  \
    }

    inline void to_json(json& _j, const engine::GlobalConfig& _gc) {
        _j = json{
            {"data", {
                 {"gde_path", _gc.GDEPath},
                 {"IDEs", json::array()},
                 {"android", {
                         {"android_sdk_path", _gc.android.sdk},
                         {"android_ndk_path", _gc.android.ndk},
                         {"android_studio_path", _gc.android.androidStudio},
                         {"jdk8_path", _gc.android.jdk},
                     }
                 },
                 {"firebase", {
                          {"firebase_path", _gc.firebase.path}
                      }
                 },
             }},
            {"projects", json::array()}
        };

        for(auto& _ide : _gc.IDEs)
            _j["data"]["IDEs"] += { {"ide_name", _ide.name}, {"ide_path", _ide.path} };

        for(auto& _project : _gc.projects) {
            _j["projects"] += json {
                    {"project_name", _project.projectName},
                    {"project_path", _project.projectPath},
                    {"default_ide", "CLion"}
            };
        }
    }

    inline void from_json(const json& _j, engine::GlobalConfig& _gc) {
        _j.at("data").at("gde_path").get_to(_gc.GDEPath);

        _j.at("data").at("android").at("android_sdk_path").get_to(_gc.android.sdk);
        _j.at("data").at("android").at("android_ndk_path").get_to(_gc.android.ndk);
        _j.at("data").at("android").at("android_studio_path").get_to(_gc.android.androidStudio);
        _j.at("data").at("android").at("jdk8_path").get_to(_gc.android.jdk);

        _j.at("data").at("firebase").at("firebase_path").get_to(_gc.firebase.path);

        for (auto& [_key, _value] : _j.at("data").at("IDEs").items())
            _gc.IDEs.emplace_back(IDE{
                    _value.at("ide_name"),
                    _value.at("ide_path")
            });

        for (auto& [_key, _value] : _j.at("projects").items())
            _gc.projects.emplace_back(Project{
                    _value.at("project_name").get<std::string>() + "=" + _value.at("project_path").get<std::string>(),
                    _value.at("project_name"),
                    _value.at("project_path").get<std::string>(),
                    ProjectConfig { _value.at("default_ide") }
                }
            );
    }

#define INIT_CENTERED_WINDOW_WITH_SIZE(_width, _height) {                                                                                           \
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});                                                                              \
    ImGui::SetNextWindowSize({_width, _height}, ImGuiCond_Always);                                                                                  \
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f)); \
}
#define INIT_CENTERED_WINDOW INIT_CENTERED_WINDOW_WITH_SIZE(-1, -1)
#define END_CENTERED_WINDOW ImGui::PopStyleVar();

#define INIT_MODAL(_modalID) ImGui::OpenPopup(_modalID);

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

#define TOOLTIP(_text)                                                                          \
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {                            \
        ImGui::BeginTooltip();                                                                  \
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);                                   \
        ImGui::TextUnformatted(_text);                                                          \
        ImGui::PopTextWrapPos();                                                                \
        ImGui::EndTooltip();                                                                    \
    }                                                                                           \

#define BROWSER_SELECT(_fileBrowser, _id, _codeBlock)                                                                                                                                               \
    if(_fileBrowser->showFileDialog(_id, imgui_addons::ImGuiFileBrowser::DialogMode::SELECT, ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*")) {    \
        _codeBlock                                                                                                                                                                               \
    }

#define BROWSER_OPEN(_fileBrowser, _id, _codeBlock)                                                                                                                                               \
    if(_fileBrowser->showFileDialog(_id, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*")) {    \
        _codeBlock;                                                                                                                                                                               \
    }

}




#endif //ENGINE_MACROS_H
