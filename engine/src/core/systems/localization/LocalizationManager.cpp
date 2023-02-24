//
// Created by borja on 10/1/22.
//

#include "core/systems/localization/LocalizationManager.h"
#include "core/Engine.h"

namespace RDE {

    void LocalizationManager::init(Engine* _engine) {
        engine = _engine;

        auto* _locales = SDL_GetPreferredLocales();

        if(_locales[0].language != nullptr) {
            std::string _country;
            if(_locales->country != nullptr) {
                auto _countryStr = std::string(_locales[0].country);
                _country = Util::String::appendToString("_", _countryStr);
            }

            localizationInfo.language = LocalizationInfo::toEnum(Util::String::appendToString(_locales[0].language, _country));
        } else {
			localizationInfo.language = LocalizationInfo::RDE_LANGUAGE_EN_US;
        }

        SDL_free(_locales);
    }

    void LocalizationManager::loadAllLanguages() {
		for(auto _i = 0; _i < LocalizationInfo::RDE_LANGUAGE_MAX; _i++) {
			loadLanguage((LocalizationInfo::RDE_LANGUAGE_)_i);
        }
    }

	void LocalizationManager::loadLanguage(LocalizationInfo::RDE_LANGUAGE_ _language) {
        if(engine->rdeConfig.projectData.localizationConfig.localizationPath.empty()) {
            Util::Log::error("Cannot load localization because the file_path was not provided in the configuration file");
            return;
        }

        auto* _manager = &engine->manager;
		auto _fileHandler = _manager->fileManager.open(Util::String::appendToString("assets/", engine->rdeConfig.projectData.localizationConfig.localizationPath), RDE_FILE_MODE_READ);

        auto _content = _manager->fileManager.readFullFile(_fileHandler).content;
        nlohmann::json _localizationJson = nlohmann::json::parse(_content);
        _manager->fileManager.close(_fileHandler);

        for(auto it = _localizationJson.begin(); it != _localizationJson.end(); ++it) {
            auto _languageString = LocalizationInfo::toString(_language);

            auto _counter = 0;
            for(auto _localization = (*it).begin(); _localization != (*it).end(); ++_localization) {
                auto _languageKey = _localization.key();
                _languageKey = Util::String::toLower(_languageKey);
                if(std::equal(_languageKey.begin(), _languageKey.end(), _languageString)) {
                    auto _value = (*_localization).get<std::string>();
                    localizationTable[_language][it.key()] = _value;
                    break;
                }

                if(_counter + 1 == (*it).size()) {
                    Util::Log::warn("Localization in language '", LocalizationInfo::toString(_language), "' for key '", it.key(), "' was not present in localization.json");
                }

                _counter++;
            }
        }
    }

    std::string LocalizationManager::localize(const std::string& _key) {
        if(localizationTable.find(localizationInfo.language) == localizationTable.end()) {
            Util::Log::error("Tried to localize to language '", LocalizationInfo::toString(localizationInfo.language), "', but it wasn't loaded");
            return Util::String::appendToString("NotLanguageLoadedLocalizationError -> ", localizationInfo.language);
        }

        if(localizationTable[localizationInfo.language].find(_key) == localizationTable[localizationInfo.language].end()) {
            Util::Log::error("Tried to localize key '", _key, "', but it wasn't found!");
            return Util::String::appendToString("KeyNotFoundLocalizationError -> ", _key);
        }

        return localizationTable[localizationInfo.language][_key];
    }

    std::string LocalizationManager::localizeSubstitution(const std::string& _string, const std::string& _replacement) {
        auto _str = _string;
        const std::string _search = engine->rdeConfig.projectData.localizationConfig.replacementSymbol;
        auto pos = _str.find(_search, 0);
        if (pos == std::string::npos) return _str;

        _str.erase(pos, _search.length());
        _str.insert(pos, _replacement);

        return _str;
    }
}