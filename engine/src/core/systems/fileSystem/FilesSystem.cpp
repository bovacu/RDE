// Created by borja on 14/2/22.

#include "core/systems/fileSystem/FilesSystem.h"

namespace GDE {

    FileHandler* FilesSystem::open(const std::string& _filePath, const FileMode& _fileMode, bool _silentNotFound) {
        const char* _fm = FileHandler::modeToChar(_fileMode);
        auto* _file = SDL_RWFromFile(_filePath.c_str(), _fm);
        if(_file == nullptr) {
            if(!_silentNotFound) LOG_E("Couldn't open file: ", _filePath)
            return nullptr;
        }

        return new FileHandler(_file, _fileMode, _filePath);
    }

    void FilesSystem::close(FileHandler* _file) {
        SDL_RWclose(_file->file);
        delete _file;
    }



    FileStr FilesSystem::readFullFile(FileHandler* _handler) {
        FileStr _f;
        checkFileMode(_handler, FileMode::READ);
        SDL_RWseek(_handler->file, 0, RW_SEEK_END);
        _f.content.resize((size_t)SDL_RWtell(_handler->file));
        SDL_RWseek(_handler->file, 0, RW_SEEK_SET);
        SDL_RWread(_handler->file, &_f.content[0], (int)_f.content.size(), (int)_f.content.size());
        return _f;
    }

    FileLines FilesSystem::readAllLinesFile(FileHandler* _handler) {
        int _currentLine = 0;
        int _finalLinePtr = 0;
        std::string _content;
        std::string _foundLine;
        FileLines _f;

        checkFileMode(_handler, FileMode::READ);
        SDL_RWseek(_handler->file, 0, RW_SEEK_END);
        long _contentSize = SDL_RWtell(_handler->file);
        _content.resize(_contentSize);
        SDL_RWseek(_handler->file, 0, RW_SEEK_SET);

        for(long _i = 0; _i < _contentSize; _i++) {
            SDL_RWread(_handler->file, &_content[_i], sizeof(char), 1);

            if(_content[_i] == '\n' || (_content[_i] == '\r' && _content[_i + 1] == '\n')) {
                _f.content.push_back(_foundLine);

                _finalLinePtr = 0;
                _foundLine.clear();

                _currentLine++;
                continue;
            }

            _foundLine.insert(_foundLine.begin() + _finalLinePtr, _content[_i]);
            _finalLinePtr++;
        }

        if(!_foundLine.empty()) _f.content.push_back(_foundLine);
        return _f;
    }

    FileStr FilesSystem::readChunkFile(FileHandler* _handler, int _initByte, int _endByte) {
        FileStr _f;
        checkFileMode(_handler, FileMode::READ);
        SDL_RWseek(_handler->file, 0, RW_SEEK_END);
        _f.content.resize((_endByte + 1) - _initByte);
        SDL_RWseek(_handler->file, _initByte, RW_SEEK_SET);
        SDL_RWread(_handler->file, &_f.content[0], _f.content.size(), _f.content.size());

        return _f;
    }

    FileStr FilesSystem::readLineInFile(FileHandler* _handler, int _line) {
        int _currentLine = 0;
        int _finalLinePtr = 0;
        std::string _content;
        FileStr _f;
        bool _lineFound;

        checkFileMode(_handler, FileMode::READ);
        SDL_RWseek(_handler->file, 0, RW_SEEK_END);
        long _contentSize = SDL_RWtell(_handler->file);
        _content.resize(_contentSize);
        SDL_RWseek(_handler->file, 0, RW_SEEK_SET);

        for(long _i = 0; _i < _contentSize; _i++) {
            SDL_RWread(_handler->file, &_content[_i], sizeof(char), 1);
            if(_currentLine == _line) {
                _lineFound = true;
                if(_content[_i] == '\n' || (_content[_i] == '\r' && _content[_i + 1] == '\n')) break;
                _f.content.insert(_f.content.begin() + _finalLinePtr, _content[_i]);
                _finalLinePtr++;
                continue;
            }

            if(_content[_i] == '\n') _currentLine++;
        }

        _f.result = _lineFound ? _f.result : FileResult::FILE_LINE_OUT_OF_SCOPE;
        return _f;
    }

    FileLines FilesSystem::readLinesInFile(FileHandler* _handler, std::vector<int>& _lines) {
        int _currentLine = 0;
        int _finalLinePtr = 0;
        std::string _content;
        std::string _foundLine;
        FileLines _f;

        checkFileMode(_handler, FileMode::READ);

        SDL_RWseek(_handler->file, 0, RW_SEEK_END);
        long _contentSize = SDL_RWtell(_handler->file);
        _content.resize(_contentSize);
        SDL_RWseek(_handler->file, 0, RW_SEEK_SET);

        std::sort(_lines.begin(), _lines.end());

        for(long _i = 0; _i < _contentSize; _i++) {
            SDL_RWread(_handler->file, &_content[_i], sizeof(char), 1);

            if(std::find(_lines.begin(), _lines.end(), _currentLine) != _lines.end()) {
                if(_content[_i] == '\n' || (_content[_i] == '\r' && _content[_i + 1] == '\n')) {
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

        if(!_foundLine.empty()) _f.content.push_back(_foundLine);
        _f.result = _lines.size() != _f.content.size() ? FileResult::FILE_NOT_ALL_LINES_IN_SCOPE : _f.result;
        return _f;
    }



    void FilesSystem::writeChunkToFile(FileHandler* _handler, const char* _content, size_t _size) {
        checkFileMode(_handler, FileMode::WRITE);
        SDL_RWwrite(_handler->file, _content, 1, _size);
    }

    void FilesSystem::writeChunkToFile(FileHandler* _handler, const std::string& _content) {
        checkFileMode(_handler, FileMode::WRITE);
        writeChunkToFile(_handler, _content.c_str(), SDL_strlen(_content.c_str()));
    }

    void FilesSystem::appendChunkToFileAtEnd(FileHandler* _handler, const char* _content, size_t _size) {
        checkFileMode(_handler, FileMode::APPEND);
        SDL_RWwrite(_handler->file, _content, 1, _size);
    }

    void FilesSystem::appendChunkToFileAtEnd(FileHandler* _handler, const std::string& _content) {
        appendChunkToFileAtEnd(_handler, _content.c_str(), SDL_strlen(_content.c_str()));
    }

    void FilesSystem::appendChunkToFile(FileHandler* _handler, const char* _content, size_t _size, int _where) {
        checkFileMode(_handler, READ_AND_WRITE);
        auto _f = readFullFile(_handler);

        auto _init = _f.content.substr(0, _where);
        writeChunkToFile(_handler, _init.c_str(), SDL_strlen(_init.c_str()));

        checkFileMode(_handler, APPEND);
        appendChunkToFileAtEnd(_handler, _content, _size);
        auto _end = _f.content.substr(_where, _f.content.size());
        appendChunkToFileAtEnd(_handler, _end.c_str(), SDL_strlen(_end.c_str()));
    }

    void FilesSystem::appendChunkToFile(FileHandler* _handler, const std::string& _content, int _where) {
        appendChunkToFile(_handler, _content.c_str(), SDL_strlen(_content.c_str()), _where);
    }

    void FilesSystem::appendChunkInLineToFile(FileHandler* _handler, const char* _content, size_t _size, int _line) {
        checkFileMode(_handler, FileMode::READ);
        auto _f = readAllLinesFile(_handler);

        clearFile(_handler);

        checkFileMode(_handler, FileMode::APPEND);
        int _currentLine = 0;
        for(auto& _l : _f.content) {
            if(_currentLine == _line) {
                appendChunkToFileAtEnd(_handler, _content, _size);
                appendChunkToFileAtEnd(_handler, "\n", SDL_strlen("\n"));
            }
            appendChunkToFileAtEnd(_handler, _l);
            appendChunkToFileAtEnd(_handler, "\n", SDL_strlen("\n"));
            _currentLine++;
        }
    }

    void FilesSystem::appendChunkInLineToFile(FileHandler* _handler, const std::string& _content, int _line) {
        appendChunkInLineToFile(_handler, _content.c_str(), SDL_strlen(_content.c_str()), _line);
    }



    void FilesSystem::replaceChunkInFile(FileHandler* _handler, const std::string& _old, const std::string& _new) {
        checkFileMode(_handler, FileMode::READ_AND_WRITE);
        auto _f = readFullFile(_handler);
        LOG_I(_f.content);
        REPLACE_S(_f.content, _old, _new);
        writeChunkToFile(_handler, _f.content);
    }

    void FilesSystem::removeFile(std::string& _filePath) {
        #if !IS_MOBILE()
        std::remove(_filePath.c_str());
        #else
        LOG_W("[NOT IMPLEMENTED] removeFile was called from a mobile target but it is NOT implemented yet")
        #endif
    }

    void FilesSystem::removeChunkLineInFile(FileHandler* _handler, int _line) {
        checkFileMode(_handler, FileMode::READ);
        auto _f = readAllLinesFile(_handler);

        clearFile(_handler);

        checkFileMode(_handler, FileMode::APPEND);
        int _currentLine = 0;
        for(auto& _l : _f.content) {
            if(_currentLine == _line) {
                _currentLine++;
                continue;
            }
            appendChunkToFileAtEnd(_handler, _l);
            appendChunkToFileAtEnd(_handler, "\n", SDL_strlen("\n"));
            _currentLine++;
        }
    }

    void FilesSystem::removeChunkLinesInFile(FileHandler* _handler, std::vector<int>& _lines) {
        checkFileMode(_handler, FileMode::READ);
        auto _f = readAllLinesFile(_handler);

        clearFile(_handler);

        std::sort(_lines.begin(), _lines.end());

        checkFileMode(_handler, FileMode::APPEND);
        int _currentLine = 0;
        for(auto& _l : _f.content) {
            if(std::find(_lines.begin(), _lines.end(), _currentLine) != _lines.end()) {
                _lines.erase(_lines.begin());
                _currentLine++;
                continue;
            }
            appendChunkToFileAtEnd(_handler, _l);
            appendChunkToFileAtEnd(_handler, "\n", SDL_strlen("\n"));
            _currentLine++;
        }
    }

    void FilesSystem::removeChunk(FileHandler* _handler, int _initByte, int _endByte) {
        checkFileMode(_handler, READ_AND_WRITE);
        auto _f = readFullFile(_handler);

        if(_initByte < 0 || _endByte > _f.content.size()) {
            LOG_E("Tried to remove with init: ", _initByte, ", end: ", _endByte, ", but file size is: ", _f.content.size())
            return;
        }

        clearFile(_handler);

        auto _init = _f.content.substr(0, _initByte);
        writeChunkToFile(_handler, _init.c_str(), SDL_strlen(_init.c_str()));

        checkFileMode(_handler, APPEND);
        auto _end = _f.content.substr(_endByte, _f.content.size());
        appendChunkToFileAtEnd(_handler, _end.c_str(), SDL_strlen(_end.c_str()));
    }



    FileHandler* FilesSystem::createFile(const std::string& _filePath) {
        return new FileHandler(SDL_RWFromFile(_filePath.c_str(), "w"), FileMode::WRITE, _filePath);
    }

    void FilesSystem::moveOrRenameFile(const std::string& _filePath, const std::string& _newPath) {
        #if !IS_MOBILE()
        std::rename(_filePath.c_str(), _newPath.c_str());
        #else
        LOG_W("[NOT IMPLEMENTED] renameFile was called from a mobile target but it is NOT implemented yet")
        #endif
    }

    void FilesSystem::clearFile(FileHandler* _handler) {
        checkFileMode(_handler, FileMode::WRITE);
        writeChunkToFile(_handler, "");
    }

    void FilesSystem::checkFileMode(FileHandler* _handler, const FileMode& _expected) {
        if(_handler->mode == _expected) return;
        SDL_RWclose(_handler->file);
        _handler->file = SDL_RWFromFile(_handler->originalPath.c_str(), FileHandler::modeToChar(_expected));
        _handler->mode = _expected;
    }

    bool FilesSystem::fileExists(const std::string& _pathToFile) {
        auto* _file = open(_pathToFile, FileMode::READ, true);
        if(_file == nullptr) return false;
        close(_file);
        return true;
    }

    void FilesSystem::appendChunkAtEndOfLine(FileHandler* _handler, const char* _content, size_t _size, int _line) {
        checkFileMode(_handler, FileMode::READ);
        auto _f = readAllLinesFile(_handler);

        clearFile(_handler);

        checkFileMode(_handler, FileMode::APPEND);
        int _currentLine = 0;
        for(auto& _l : _f.content) {
            if(_currentLine == _line) _l.append(_content);
            appendChunkToFileAtEnd(_handler, _l);
            appendChunkToFileAtEnd(_handler, "\n", SDL_strlen("\n"));
            _currentLine++;
        }
    }

    void FilesSystem::appendChunkAtEndOfLine(FileHandler* _handler, const std::string& _content, int _line) {
        appendChunkAtEndOfLine(_handler, _content.c_str(), SDL_strlen(_content.c_str()), _line);
    }

}