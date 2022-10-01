//
// Created by borja on 10/1/22.
//

#ifndef GDE_LOCALIZATION_MANAGER_H
#define GDE_LOCALIZATION_MANAGER_H


#include <string>
#include "core/Core.h"
#include "nlohmann/json.hpp"
#include "core/util/StringUtils.h"
#include "core/util/Logger.h"

namespace GDE {

    struct LocalizationInfo {
        enum Language { EN, ES, PT_BR, FR, MAX };
        Language language;

        static const char* toString(Language _language) {
            switch (_language) {
                case EN:    return "en";
                case ES:    return "es";
                case PT_BR: return "pt-br";
                case FR:    return "fr";
                default:    return "NON_LANGUAGE";
            }
        }
    };

    FORWARD_DECLARE_CLASS(Engine)

    class LocalizationManager {

        FRIEND_CLASS(Manager)

        private:
            std::unordered_map<LocalizationInfo::Language, std::unordered_map<std::string, std::string>> localizationTable;
            Engine* engine;

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
                        char*   STRING;
                    } data {  };

                public:
                    explicit Any(int   _e)   { data.INT    = _e; type = Int;     }
                    explicit Any(uint   _e)  { data.UINT   = _e; type = UInt;    }
                    explicit Any(long   _e)  { data.LONG   = _e; type = Long;    }
                    explicit Any(ulong   _e) { data.ULONG  = _e; type = ULong;   }
                    explicit Any(float _e)   { data.FLOAT  = _e; type = Float;   }
                    explicit Any(double _e)  { data.DOUBLE = _e; type = Double;  }
                    explicit Any(char* _e)   { data.STRING = _e; type = String;  }
                    [[nodiscard]] Type getType()    const { return type;        }
                    [[nodiscard]] int getInt()      const { return data.INT;    }
                    [[nodiscard]] float getFloat()  const { return data.FLOAT;  }
                    [[nodiscard]] char* getString() const { return data.STRING; }
                    [[nodiscard]] uint getUInt()    const { return data.UINT;   }
                    [[nodiscard]] long getLong()    const { return data.LONG;   }
                    [[nodiscard]] ulong getULong()  const { return data.ULONG;  }
            };

        public:
            LocalizationInfo localizationInfo;

        private:
            void init(Engine* _engine);
            std::string localizeSubstitution(const std::string& _string, const std::string& _replacement);

        public:
            void loadAllLanguages();
            void loadLanguage(LocalizationInfo::Language _language);

            std::string localize(const std::string& _key);

            template<typename... Args>
            std::string localize(const std::string& _key, Args&&... _args);
    };

    template<typename... Args>
    std::string LocalizationManager::localize(const std::string& _key, Args&&... _args) {
        std::vector<Any> _vec = { _args... };

        if(localizationTable.find(localizationInfo.language) == localizationTable.end()) {
            LOG_E("Tried to localize to language '", LocalizationInfo::toString(localizationInfo.language), "', but it wasn't loaded")
            return APPEND_S("NotLanguageLoadedLocalizationError -> ", localizationInfo.language);
        }

        if(localizationTable[localizationInfo.language].find(_key) == localizationTable[localizationInfo.language].end()) {
            LOG_E("Tried to localize key '", _key, "', but it wasn't found!")
            return APPEND_S("KeyNotFoundLocalizationError -> ", _key);
        }

        auto _string = localizationTable[localizationInfo.language][_key];

        for (auto& _i : _vec) {
            switch (_i.getType()) {
                case Any::Int: {
                    _string = localizeSubstitution(_string, std::to_string(_i.Int));
                    break;
                }
                case Any::UInt:{
                    _string = localizeSubstitution(_string, std::to_string(_i.UInt));
                    break;
                }
                case Any::Long:{
                    _string = localizeSubstitution(_string, std::to_string(_i.Long));
                    break;
                }
                case Any::ULong:{
                    _string = localizeSubstitution(_string, std::to_string(_i.ULong));
                    break;
                }
                case Any::Float:{
                    _string = localizeSubstitution(_string, std::to_string(_i.Float));
                    break;
                }
                case Any::Double:{
                    _string = localizeSubstitution(_string, std::to_string(_i.Double));
                    break;
                }
                case Any::String:{
                    _string = localizeSubstitution(_string, std::to_string(_i.String));
                    break;
                }
            }
        }
    }


}

#endif //GDE_LOCALIZATION_MANAGER_H
