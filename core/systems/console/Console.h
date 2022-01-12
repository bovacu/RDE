// Created by borja on 12/1/22.


#ifndef ENGINE2_0_CONSOLE_H
#define ENGINE2_0_CONSOLE_H


#include "core/util/Util.h"
#include <functional>

namespace engine {

    struct Command {
        std::string name;
        std::vector<std::string> arguments;
    };

    typedef std::vector<std::string> Logs;

    typedef std::function<Logs (const std::vector<std::string>&)> CommandFunc;

    class Console {
        private:
            std::map<std::string, CommandFunc> commands;
            Logs help(const std::vector<std::string>& _arguments = {});
            std::vector<Command> commandHistory;

        public:
            static Console& get();

            void init();
            void addCommand(const std::string& _commandName, CommandFunc _commandFunc);
            void deleteCommand(const std::string& _commandName);

            Logs call(const Command& _command);
            Logs printHistory(const Command& _command);
    };

}


#endif //ENGINE2_0_CONSOLE_H
