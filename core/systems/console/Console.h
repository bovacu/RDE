// Created by borja on 12/1/22.


#ifndef ENGINE2_0_CONSOLE_H
#define ENGINE2_0_CONSOLE_H


#include "core/util/Util.h"
#include <functional>

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

    typedef std::function<Logs (const std::vector<std::string>&)> CommandFunc;

    class Console {
        private:
            std::map<std::string, CommandFunc> commands;
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
            void addCommand(const std::string& _commandName, CommandFunc _commandFunc);
            void deleteCommand(const std::string& _commandName);

            Logs call(const Command& _command);

            std::string getUpCommand();
            std::string getDownCommand();
    };

}


#endif //ENGINE2_0_CONSOLE_H
