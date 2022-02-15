// Created by borja on 14/2/22.

#include <functional>
#include "core/systems/fileSystem/FilesSystem.h"

namespace engine {


    FileStr FilesSystem::readFullFile(const std::string& _filePath) {
        SDL_RWops* _file = SDL_RWFromFile(_filePath.c_str(), "rb");
        if (_file == nullptr) {
            LOG_E("Can't read file (readFullFile): ", _filePath)
            return {"", FileResult::FILE_NOT_FOUND};
        }

        FileStr _f;
        SDL_RWseek(_file, 0, RW_SEEK_END);
        _f.content.resize((size_t)SDL_RWtell(_file));
        SDL_RWseek(_file, 0, RW_SEEK_SET);
        SDL_RWread(_file, &_f.content[0], (int)_f.content.size(), (int)_f.content.size());
        SDL_RWclose(_file);

        return _f;
    }

    FileStr FilesSystem::readChunkFile(const std::string& _filePath, int _initByte, int _endByte) {
        SDL_RWops* _file = SDL_RWFromFile(_filePath.c_str(), "rb");
        if (_file == nullptr) {
            LOG_E("Can't read file (readChunkFile): ", _filePath)
            return {"", FileResult::FILE_NOT_FOUND};
        }

        FileStr _f;
        SDL_RWseek(_file, 0, RW_SEEK_END);
        _f.content.resize((_endByte + 1) - _initByte);
        SDL_RWseek(_file, _initByte, RW_SEEK_SET);
        SDL_RWread(_file, &_f.content[0], _f.content.size(), _f.content.size());
        SDL_RWclose(_file);

        return _f;
    }

    FileStr FilesSystem::readLineInFile(const std::string& _filePath, int _line) {
        SDL_RWops* _file = SDL_RWFromFile(_filePath.c_str(), "rb");

        if (_file == nullptr) {
            LOG_E("Can't read file (readChunkFile): ", _filePath)
            return {"", FileResult::FILE_NOT_FOUND};
        }

        int _currentLine = 0;
        int _finalLinePtr = 0;
        std::string _content;
        FileStr _f;
        bool _lineFound;

        SDL_RWseek(_file, 0, RW_SEEK_END);
        long _contentSize = SDL_RWtell(_file);
        _content.resize(_contentSize);
        SDL_RWseek(_file, 0, RW_SEEK_SET);

        for(long _i = 0; _i < _contentSize; _i++) {
            SDL_RWread(_file, &_content[_i], sizeof(char), 1);
            if(_currentLine == _line) {
                _lineFound = true;
                if(_content[_i] == std::char_traits<char>::eof() || _content[_i] == '\n' || (_content[_i] == '\r' && _content[_i + 1] == '\n')) break;
                _f.content.insert(_f.content.begin() + _finalLinePtr, _content[_i]);
                _finalLinePtr++;

                _currentLine++;
                continue;
            }

            if(_content[_i] == '\n') _currentLine++;
        }

        SDL_RWclose(_file);
        _f.result = _lineFound ? _f.result : FileResult::FILE_LINE_OUT_OF_SCOPE;
        return _f;
    }

    FileLines FilesSystem::readLinesInFile(const std::string& _filePath, std::vector<int>& _lines) {
        SDL_RWops* _file = SDL_RWFromFile(_filePath.c_str(), "rb");

        if (_file == nullptr) {
            LOG_E("Can't read file (readChunkFile): ", _filePath)
            return {{}, FileResult::FILE_NOT_FOUND};
        }

        int _currentLine = 0;
        int _finalLinePtr = 0;
        std::string _content;
        std::string _foundLine;
        FileLines _f;

        SDL_RWseek(_file, 0, RW_SEEK_END);
        long _contentSize = SDL_RWtell(_file);
        _content.resize(_contentSize);
        SDL_RWseek(_file, 0, RW_SEEK_SET);

        std::sort(_lines.begin(), _lines.end());

        for(long _i = 0; _i < _contentSize; _i++) {
            SDL_RWread(_file, &_content[_i], sizeof(char), 1);

            if(std::find(_lines.begin(), _lines.end(), _currentLine) != _lines.end()) {
                if(_content[_i] == std::char_traits<char>::eof()) {
                    _f.content.push_back(_foundLine);
                    break;
                } else if(_content[_i] == '\n' || (_content[_i] == '\r' && _content[_i + 1] == '\n')) {
                    _lines.erase(_lines.begin());
                    _f.content.push_back(_foundLine);

                    _finalLinePtr = 0;
                    _foundLine.clear();

                    _currentLine++;
                    continue;
                }

                _foundLine.insert(_foundLine.begin() + _finalLinePtr, _content[_i]);
                _finalLinePtr++;
            }

            if(_content[_i] == '\n') _currentLine++;
        }

        SDL_RWclose(_file);
        _f.result = _lines.size() != _f.content.size() ? FileResult::FILE_NOT_ALL_LINES_IN_SCOPE : _f.result;
        return _f;
    }



    void FilesSystem::writeChunkToFile(const std::string& _filePath, void* _content) {

    }

    void FilesSystem::appendChunkToFile(const std::string& _filePath, void* _content, int _where) {

    }

    void FilesSystem::appendChunkInLineToFile(const std::string& _filePath, void* _content, int _line) {

    }

    void FilesSystem::replaceChunkInFile(const std::string& _old, const std::string& _new) {

    }

    void FilesSystem::removeFile(std::string& _filePath) {

    }

    void FilesSystem::removeChunkLineInFile(std::string& _filePath, int _line) {

    }

    void FilesSystem::removeChunkLinesInFile(std::string& _filePath, std::vector<int> _lines) {

    }

    void FilesSystem::removeChunk(const std::string& _filePath, int _initByte, int _endByte) {

    }

    void FilesSystem::createFile(const std::string& _filePath) {
        SDL_RWclose(SDL_RWFromFile(_filePath.c_str(), "w"));
    }

    void FilesSystem::renameFile(const std::string& _filePath, const std::string& _newName, const std::string& _newExtension) {
        // Open current name
        // Create new one
        // Copy old to new
        // Delete old
        // Close new
    }
}