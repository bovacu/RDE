// Created by borja on 12/1/22.


#include "Console.h"
#include <utility>

namespace engine {

    Console& Console::get() {
        static Console console;
        return console;
    }

    void Console::init() {
        addCommand("help", BIND_FUNC_1(Console::help));
    }

    void Console::addCommand(const std::string& _commandName, CommandFunc _commandFunc) {
        commands[_commandName] = std::move(_commandFunc);
    }

    void Console::deleteCommand(const std::string& _commandName) {
        commands.erase(_commandName);
    }

    Logs Console::call(const Command& _command) {
        commandHistory.push_back(_command);
        if(commands.find(_command.name) == commands.end()) {
            std::string _error = "[error] Command '" + std::string(_command.name) + "' not recognized";
            return {_error, "\n"};
        }

        std::vector<std::string> _logs;
        std::string _commandUsed = "#" + _command.name;
        for(const auto & argument : _command.arguments)
            _commandUsed += " " + argument;

        _logs.push_back(_commandUsed);

        auto _commandResult = commands[_command.name](_command.arguments);
        _logs.insert(_logs.end(), _commandResult.begin(), _commandResult.end());
        _logs.emplace_back("\n");
        return _logs;
    }

    Logs Console::help(const std::vector<std::string>& _arguments) {
        Logs _logs;

        _logs.emplace_back("This is the command line of the game engine, here we will list all the available commands:");
        _logs.emplace_back("> help: ");
        _logs.emplace_back("> show_debug(true|false): shows the debugging tools");

        return _logs;
    }

    Logs Console::printHistory(const Command& _command) {
        Logs _logs;

        for(auto& _c : commandHistory) {
            std::string _fullCommand = _command.name + " ";

            for(int _i = 0; _i < _c.arguments.size(); _i++) {
                if(_i < _c.arguments.size() - 1)
                    _fullCommand += " ";
                _fullCommand += _c.arguments[_i];
            }

            _logs.push_back(_fullCommand);
        }

        return _logs;
    }
}