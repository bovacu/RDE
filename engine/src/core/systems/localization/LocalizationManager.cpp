//
// Created by borja on 10/1/22.
//

#include "core/systems/localization/LocalizationManager.h"
#include "core/Engine.h"

namespace GDE {

    void LocalizationManager::init(Engine* _engine) {
        engine = _engine;

        auto* _locales = SDL_GetPreferredLocales();

        if(_locales[0].language != nullptr) {
            std::string _country;
            if(_locales->country != nullptr) {
                auto _countryStr = std::string(_locales[0].country);
                _country = APPEND_S("_", _countryStr);
            }

            localizationInfo.language = LocalizationInfo::toEnum(APPEND_S(_locales[0].language, _country));
        } else {
            localizationInfo.language = LocalizationInfo::EN_US;
        }

        SDL_free(_locales);
    }

    void LocalizationManager::loadAllLanguages() {
        for(auto _i = 0; _i < LocalizationInfo::MAX; _i++) {
            loadLanguage((LocalizationInfo::Language)_i);
        }
    }

    void LocalizationManager::loadLanguage(LocalizationInfo::Language _language) {
        if(engine->gdeConfig.projectData.localizationConfig.localizationPath.empty()) {
            LOG_E("Cannot load localization because the file_path was not provided in the configuration file")
            return;
        }

        auto* _manager = &engine->manager;
        auto _fileHandler = _manager->fileManager.open(APPEND_S("assets/", engine->gdeConfig.projectData.localizationConfig.localizationPath), FileMode::READ);

        auto _content = _manager->fileManager.readFullFile(_fileHandler).content;
        nlohmann::json _localizationJson = nlohmann::json::parse(_content);
        _manager->fileManager.close(_fileHandler);

        for(auto it = _localizationJson.begin(); it != _localizationJson.end(); ++it) {
            auto _languageString = LocalizationInfo::toString(_language);
            if((*it).contains(_languageString)) {
                auto _value = (*it)[_languageString].get<std::string>();
                localizationTable[_language][it.key()] = _value;
            } else {
                LOG_W("Localization in language '", LocalizationInfo::toString(_language), "' for key '", it.key(), "' was not present in localization.json")
            }
        }
    }

    std::string LocalizationManager::localize(const std::string& _key) {
        if(localizationTable.find(localizationInfo.language) == localizationTable.end()) {
            LOG_E("Tried to localize to language '", LocalizationInfo::toString(localizationInfo.language), "', but it wasn't loaded")
            return APPEND_S("NotLanguageLoadedLocalizationError -> ", localizationInfo.language);
        }

        if(localizationTable[localizationInfo.language].find(_key) == localizationTable[localizationInfo.language].end()) {
            LOG_E("Tried to localize key '", _key, "', but it wasn't found!")
            return APPEND_S("KeyNotFoundLocalizationError -> ", _key);
        }

        return localizationTable[localizationInfo.language][_key];
    }

    std::string LocalizationManager::localizeSubstitution(const std::string& _string, const std::string& _replacement) {
        auto _str = _string;
        const std::string _search = engine->gdeConfig.projectData.localizationConfig.replacementSymbol;
        auto pos = _str.find(_search, 0);
        if (pos == std::string::npos) return _str;

        _str.erase(pos, _search.length());
        _str.insert(pos, _replacement);

        return _str;
    }
}