// Created by borja on 12/1/22.


#ifndef ENGINE2_0_CONSOLE_H
#define ENGINE2_0_CONSOLE_H


#include "core/util/Util.h"

namespace GDE {

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

    /// This is a class basically used for debugging and allows to create custom commands to execute on runtime.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
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
            /// This function should not be called by the end users.
            void init();

            /// This function allows to create a custom command.
            /// @param _commandName the name you will use to call the command.
            /// @param _description the description displayed if command 'help' is called.
            /// @param _class this is the instance of the class that contains the function to be called, usually just pass 'this'.
            /// @param _argumentsDescription the arguments that should be passed that will be displayed if 'help' is called.
            template<auto Func, typename Class>
            void addCommand(const std::string& _commandName, const std::string& _description, Class* _class = nullptr, const std::string& _argumentsDescription = "") {
                commands[_commandName].template bind<Func>(_class);
                commandsAndDescriptions[_commandName] = _argumentsDescription + ": " + _description;
            }

            /// Deletes a before created command.
            void deleteCommand(const std::string& _commandName);

            /// This function should not be called by end users.
            Logs call(const Command& _command);
            /// This function should not be called by end users.
            void collectLogs(const Logs& _log);

            /// This function should not be called by end users.
            std::string getUpCommand();
            /// This function should not be called by end users.
            std::string getDownCommand();
    };

}


#endif //ENGINE2_0_CONSOLE_H
