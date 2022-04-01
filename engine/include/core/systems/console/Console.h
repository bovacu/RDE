// Created by borja on 12/1/22.


#ifndef ENGINE2_0_CONSOLE_H
#define ENGINE2_0_CONSOLE_H


#include "core/util/Util.h"

namespace engine {

    struct Command {
        std::string name;
        std::vector<std::string> arguments;

        std::string toString() {
            std::string _fullCommand = name;
            for(auto& _arg : arguments)
                _fullCommand += " " + _arg;

            return _fullCommand;
        }
    };

    typedef std::vector<std::string> Logs;

    class Console {
        private:
            std::map<std::string, MDelegate<Logs (const std::vector<std::string>&)>> commands;
            std::map<std::string, std::string> commandsAndDescriptions;
            Logs help(const std::vector<std::string>& _arguments = {});
            Logs clear(const std::vector<std::string>& _arguments = {});
            Logs printHistory(const std::vector<std::string>& _arguments = {});
            std::vector<Command> commandHistory;
            int historyPtr = 0;

        public:
            std::vector<std::string> logs;

        public:
            static Console& get();

            void init();

            template<auto Func, typename Class>
            void addCommand(const std::string& _commandName, const std::string& _description, Class* _class = nullptr, const std::string& _argumentsDescription = "") {
                commands[_commandName].template bind<Func>(_class);
                commandsAndDescriptions[_commandName] = _argumentsDescription + ": " + _description;
            }
            void deleteCommand(const std::string& _commandName);

            Logs call(const Command& _command);
            void collectLogs(const Logs& _log);

            std::string getUpCommand();
            std::string getDownCommand();
    };

}


#endif //ENGINE2_0_CONSOLE_H
