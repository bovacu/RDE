//
// Created by borja on 10/1/22.
//

#ifndef RDE_LOCALIZATION_MANAGER_H
#define RDE_LOCALIZATION_MANAGER_H


#include <string>
#include "core/Core.h"
#include "nlohmann/json.hpp"
#include "core/util/Functions.h"

namespace RDE {

    /**
     * @brief Small data struct to save the information of the current language and to transform it as enum <-> string.
    */
    struct LocalizationInfo {
        /**
         * @brief Available languages by default of the engine.
        */
		enum RDE_LANGUAGE_ { 
			RDE_LANGUAGE_EN_US, 
			RDE_LANGUAGE_EN_GB, 
			RDE_LANGUAGE_EN_CA, 
			RDE_LANGUAGE_ES_MX, 
			RDE_LANGUAGE_ES_ES, 
			RDE_LANGUAGE_PT_BR, 
			RDE_LANGUAGE_FR, 
			RDE_LANGUAGE_ZH, 
			RDE_LANGUAGE_RU, 
			RDE_LANGUAGE_DE, 
			RDE_LANGUAGE_IT, 
			RDE_LANGUAGE_JP, 
			RDE_LANGUAGE_MAX 
		};
		RDE_LANGUAGE_ language;

        /**
         * @brief Transforms the enum into a string.
         * @param _language The Language enum
         * @return A const char* of the Language code
        */
		static const char* toString(RDE_LANGUAGE_ _language) {
            switch (_language) {
				case RDE_LANGUAGE_EN_US: return "en-us";
				case RDE_LANGUAGE_ES_ES: return "es-es";
				case RDE_LANGUAGE_PT_BR: return "pt-br";
				case RDE_LANGUAGE_FR:    return "fr";
				case RDE_LANGUAGE_EN_GB: return "en-gb";
				case RDE_LANGUAGE_EN_CA: return "en-ca";
				case RDE_LANGUAGE_ES_MX: return "es-mx";
				case RDE_LANGUAGE_ZH:    return "zh";
				case RDE_LANGUAGE_RU:    return "ru";
				case RDE_LANGUAGE_DE:    return "de";
				case RDE_LANGUAGE_IT:    return "it";
				case RDE_LANGUAGE_JP:    return "jp";
                default:    return "NON_LANGUAGE";
            }
        }

        /**
         * @brief Transforms a valid code as char* to an enum.
         * @param _language A valid language code.
         * @return Language
        */
		static RDE_LANGUAGE_ toEnum(const char* _language) {
            return toEnum(std::string(_language));
        }

        /**
         * @brief Transforms a valid code as char* to an enum.
         * @param _language A valid language code.
         * @return Language
        */
		static RDE_LANGUAGE_ toEnum(const std::string& _language) {
            auto _strCpy = _language;
            _strCpy = Util::String::toLower(_strCpy);

			if(std::equal(_strCpy.begin(), _strCpy.end(), "en_us")) return RDE_LANGUAGE_EN_US;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "en"))    return RDE_LANGUAGE_EN_US;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "es"))    return RDE_LANGUAGE_ES_ES;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "es_es")) return RDE_LANGUAGE_ES_ES;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "pt"))    return RDE_LANGUAGE_PT_BR;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "pt_br")) return RDE_LANGUAGE_PT_BR;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "fr"))    return RDE_LANGUAGE_FR;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "en_gb")) return RDE_LANGUAGE_EN_GB;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "en_ca")) return RDE_LANGUAGE_EN_CA;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "es_mx")) return RDE_LANGUAGE_ES_MX;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "zh"))    return RDE_LANGUAGE_ZH;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "ru"))    return RDE_LANGUAGE_RU;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "de"))    return RDE_LANGUAGE_DE;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "it"))    return RDE_LANGUAGE_IT;
			if(std::equal(_strCpy.begin(), _strCpy.end(), "jp"))    return RDE_LANGUAGE_JP;

            Util::Log::warn("Locale: '", _language, "' is not implemented in the engine, defaulting to english");
			return RDE_LANGUAGE_EN_US;
        }
    };

    class Engine;

    class LocalizationManager {

        friend class Manager;

        private:
            /**
             * @brief A map that stores [LocalizationKey] -> (keyWord, TranslationWord).
            */
			std::unordered_map<LocalizationInfo::RDE_LANGUAGE_, std::unordered_map<std::string, std::string>> localizationTable;

            /**
             * @brief Reference to the Engine.
            */
            Engine* engine;

            /**
             * @brief Struct that contains all of the basic types (or the most common ones at least), so we can pass them to the translate function and work with the specific values.
            */
            struct Any {
                enum Type { Int, UInt, Long, ULong, Float, Double, String };

                private:
                    Type type;
                    union {
                        int     INT;
                        uint    UINT;
                        long    LONG;
                        ulong   ULONG;
                        float   FLOAT;
                        double  DOUBLE;
                        const char*   STRING;
                    } data {  };

                public:
                    // DO NOT SET HERE explicit ON CONSTRUCTORS
                    Any(int   _e)   { data.INT    = _e; type = Int;     }
                    Any(uint   _e)  { data.UINT   = _e; type = UInt;    }
                    Any(long   _e)  { data.LONG   = _e; type = Long;    }
                    Any(ulong   _e) { data.ULONG  = _e; type = ULong;   }
                    Any(float _e)   { data.FLOAT  = _e; type = Float;   }
                    Any(double _e)  { data.DOUBLE = _e; type = Double;  }
                    Any(const char* _e)   { data.STRING = _e; type = String;  }
                    Any(const std::string &_e)   { data.STRING = _e.c_str(); type = String;  }
                    [[nodiscard]] Type getType()    const { return type;        }
                    [[nodiscard]] int getInt()      const { return data.INT;    }
                    [[nodiscard]] float getFloat()  const { return data.FLOAT;  }
                    [[nodiscard]] double getDouble()  const { return data.DOUBLE;  }
                    [[nodiscard]] const char* getString() const { return data.STRING; }
                    [[nodiscard]] uint getUInt()    const { return data.UINT;   }
                    [[nodiscard]] long getLong()    const { return data.LONG;   }
                    [[nodiscard]] ulong getULong()  const { return data.ULONG;  }
            };

        public:
            LocalizationInfo localizationInfo;

        private:
            /**
             * @brief Initiates the Localization module.
            */
            void init(Engine* _engine);

            /**
             * @brief Substitutes a "key" replacement char with the corresponding value.
             * @param _string Value to substitute the _replacement, this will be the final value
             * @param _replacement Value used in the configuration and that will be substituted with _string, this value will be lost
             * @return The string with the _replacemente replaced by _string
            */
            std::string localizeSubstitution(const std::string& _string, const std::string& _replacement);

        public:
            /**
             * @brief Loads all languages configured in the localization.json
            */
            void loadAllLanguages();

            /**
             * @brief Loads a specific language configured in the localization.json
             * @param _language The Language to load
            */
            void loadLanguage(LocalizationInfo::RDE_LANGUAGE_ _language);

            /**
             * @brief Returns the localized value of a key in the localization.json in the current loaded Language.
             * @param _key The word we want to translate defined in the localization.json
             * @return A string with the localization in the specific Language loaded
            */
            std::string localize(const std::string& _key);

            /**
             * @brief Returns the localized value of a key in the localization.json in the current loaded Language.
             * @param _key The word we want to translate defined in the localization.json
             * @param _args A sequence of values that will be substitute in the string value
             * @return A string with the localization in the specific Language loaded
            */
            template<typename... Args>
            std::string localize(const std::string& _key, Args&... _args);
    };

    template<typename... Args>
    std::string LocalizationManager::localize(const std::string& _key, Args&... _args) {
        std::vector<Any> _vec = { _args... };

        if(localizationTable.find(localizationInfo.language) == localizationTable.end()) {
            Util::Log::error("Tried to localize to language '", LocalizationInfo::toString(localizationInfo.language), "', but it wasn't loaded");
            return Util::String::appendToString("NotLanguageLoadedLocalizationError -> ", localizationInfo.language);
        }

        if(localizationTable[localizationInfo.language].find(_key) == localizationTable[localizationInfo.language].end()) {
            Util::Log::error("Tried to localize key '", _key, "', but it wasn't found!");
            return Util::String::appendToString("KeyNotFoundLocalizationError -> ", _key);
        }

        auto _string = localizationTable[localizationInfo.language][_key];

        for (auto& _i : _vec) {
            switch (_i.getType()) {
                case Any::Int: {
                    _string = localizeSubstitution(_string, std::to_string(_i.getInt()));
                    break;
                }
                case Any::UInt:{
                    _string = localizeSubstitution(_string, std::to_string(_i.getUInt()));
                    break;
                }
                case Any::Long:{
                    _string = localizeSubstitution(_string, std::to_string(_i.getLong()));
                    break;
                }
                case Any::ULong:{
                    _string = localizeSubstitution(_string, std::to_string(_i.getULong()));
                    break;
                }
                case Any::Float:{
                    _string = localizeSubstitution(_string, std::to_string(_i.getFloat()));
                    break;
                }
                case Any::Double:{
                    _string = localizeSubstitution(_string, std::to_string(_i.getDouble()));
                    break;
                }
                case Any::String:{
                    _string = localizeSubstitution(_string, std::string(_i.getString()));
                    break;
                }
            }
        }

        return _string;
    }


}

#endif //RDE_LOCALIZATION_MANAGER_H
