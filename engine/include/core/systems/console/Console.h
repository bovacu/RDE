// Created by borja on 12/1/22.


#ifndef RDE_CONSOLE_H
#define RDE_CONSOLE_H


#include "core/util/Util.h"

namespace RDE {

    /**
     * @brief This class represents a command of the internal engine console.
     */
    struct Command {
        /**
         * @brief Name of the command.
         */
        std::string name;

        /**
         * @brief List of arguments of the command.
         */
        std::vector<std::string> arguments;

        /**
         * @brief Returns the name of the command + its arguments.
         * @return std::string
         */
        std::string toString() {
            std::string _fullCommand = name;
            for(auto& _arg : arguments)
                _fullCommand += " " + _arg;

            return _fullCommand;
        }
    };

    typedef std::vector<std::string> Logs;

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This is a class basically used for debugging and allows to create custom commands to execute on runtime.
     */
    class Console {
        private:
            /**
             * @brief All the registeredCommands.
             */
            std::map<std::string, MDelegate<Logs (const std::vector<std::string>&)>> commands;

            /**
             * @brief All the descriptions of the commands.
             */
            std::map<std::string, std::string> commandsAndDescriptions;

            /**
             * @brief Internal command 'help'.
             * @param _arguments Arguments
             * @return Logs
             */
            Logs help(const std::vector<std::string>& _arguments = {});

            /**
             * @brief Internal command 'clear'.
             * @param _arguments Arguments
             * @return Logs
             */
            Logs clear(const std::vector<std::string>& _arguments = {});

            /**
             * @brief Internal command 'printHistory'.
             * @param _arguments Arguments
             * @return Logs
             */
            Logs printHistory(const std::vector<std::string>& _arguments = {});

            /**
             * @brief History of the used commands.
             */
            std::vector<Command> commandHistory;

            /**
             * @brief Pointer to the current command in the history list.
             */
            int historyPtr = 0;

        public:
            std::vector<std::string> logs;

        public:
            /**
             * @attention This function should not be called by end users.
             * @brief Initiates all the crucial elements of the Console.
             */
            void init();

            /**
             * @brief This function allows to create a custom command.
             * @param _commandName the name you will use to call the command.
             * @param _description the description displayed if command 'help' is called.
             * @param _class this is the instance of the class that contains the function to be called, usually just pass 'this'.
             * @param _argumentsDescription the arguments that should be passed that will be displayed if 'help' is called.
             */
            template<auto Func, typename Class>
            void addCommand(const std::string& _commandName, const std::string& _description, Class* _class = nullptr, const std::string& _argumentsDescription = "") {
                commands[_commandName].template bind<Func>(_class);
                commandsAndDescriptions[_commandName] = _argumentsDescription + ": " + _description;
            }

            /**
             * @brief Deletes a before created command.
             * @param _commandName Command to remove from the registry
             */
            void deleteCommand(const std::string& _commandName);

            /**
             * @attention This function should not be called by end users.
             * @brief Invokes a command.
             * @param _command Commadn name
             * @return Logs
             */
            Logs call(const Command& _command);

            /**
             * @attention This function should not be called by end users.
             * @brief Gets the logs of a command call
             * @param _log Logs
             */
            void collectLogs(const Logs& _log);

            /**
             * @attention This function should not be called by end users.
             * @brief Gets the upper command of the historyPtr.
             * @return std::string
             */
            std::string getUpCommand();

            /**
             * @attention This function should not be called by end users.
             * @brief Gets the lower command of the historyPtr.
             * @return std::string
             */
            std::string getDownCommand();
    };

}


#endif //RDE_CONSOLE_H
