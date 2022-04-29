// Created by borja on 12/1/22.


#include "core/systems/console/Console.h"

namespace GDE {

    Console& Console::get() {
        static Console console;
        return console;
    }

    void Console::init() {
        addCommand<&Console::help>("help", "Gives all available commands information", this);
        addCommand<&Console::clear>("clear", "clears the screen", this);
        addCommand<&Console::printHistory>("history", "shows all the commands used in the session", this);
    }

    void Console::deleteCommand(const std::string& _commandName) {
        commands.erase(_commandName);
    }

    void Console::collectLogs(const Logs& _logs) {

    }

    Logs Console::call(const Command& _command) {
        historyPtr++;
        commandHistory.push_back(_command);
        if(commands.find(_command.name) == commands.end()) {
            std::string _error = "[error] Command '" + std::string(_command.name) + "' not recognized";
            return {_error, "\n"};
        }

        std::vector<std::string> _logs;

        if(_command.name != "clear") {
            std::string _commandUsed = "#" + _command.name;
            for(const auto & argument : _command.arguments)
                _commandUsed += " " + argument;
            _logs.push_back(_commandUsed);
        }

        std::vector<Logs> _commandResult {};
        commands[_command.name].exec(_commandResult, _command.arguments);
        for(auto& _c : _commandResult)
            for(auto& _log : _c)
                _logs.push_back(_log);

        if(_command.name != "clear")
            _logs.emplace_back("\n");

        return _logs;
    }

    Logs Console::help(const std::vector<std::string>& _arguments) {
        Logs _logs;

        for(auto& _command : commandsAndDescriptions)
            _logs.emplace_back("> " + _command.first + " " +  _command.second);

        return _logs;
    }

    Logs Console::printHistory(const std::vector<std::string>& _arguments) {
        Logs _logs;

        for(auto& _c : commandHistory) {
            std::string _fullCommand = "-> " + _c.name + " ";

            for(int _i = 0; _i < _c.arguments.size(); _i++) {
                if(_i < _c.arguments.size() - 1)
                    _fullCommand += " ";
                _fullCommand += _c.arguments[_i];
            }

            _logs.push_back(_fullCommand);
        }

        return _logs;
    }

    std::string Console::getUpCommand() {
        if(historyPtr - 1 < 0)
            return commandHistory[0].toString();

        historyPtr--;
        return commandHistory[historyPtr].toString();
    }

    std::string Console::getDownCommand() {
        if(historyPtr + 1 >= commandHistory.size())
            return {commandHistory[commandHistory.size() - 1].toString()};

        historyPtr++;
        return commandHistory[historyPtr].toString();
    }

    Logs Console::clear(const std::vector<std::string>& _arguments) {
        logs.clear();
        return {};
    }
}