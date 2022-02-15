// Created by borja on 14/2/22.


#ifndef ENGINE2_0_FILES_SYSTEM_H
#define ENGINE2_0_FILES_SYSTEM_H

#include "core/util/Util.h"

namespace engine {

    enum FileResult {
        FILE_OK,
        FILE_NOT_FOUND,
        FILE_LINE_OUT_OF_SCOPE,
        FILE_NOT_ALL_LINES_IN_SCOPE
    };




    enum FileMode {
        READ,
        WRITE,
        APPEND,
        READ_AND_WRITE,
        READ_AND_APPEND,
    };




    template<typename T>
    struct File {
        T content;
        FileResult result = FileResult::FILE_OK;
    };
    typedef File<std::string> FileStr;
    typedef File<std::vector<std::string>> FileLines;




    class FilesSystem;
    class FileHandler {
        private:
            SDL_RWops* file;
            FileMode mode;
            std::string originalPath;

        private:
            FileHandler(SDL_RWops* _file, const FileMode& _mode, const std::string& _originalPath) : file(_file), mode(_mode), originalPath(_originalPath) {}
            static const char* modeToChar(const FileMode& _mode) {
                switch (_mode) {
                    case READ: return "rb";
                    case WRITE: return "w";
                    case APPEND: return "a";
                    case READ_AND_WRITE: return "r+";
                    case READ_AND_APPEND: return "a+";
                }
            }

        friend class FilesSystem;
    };




    class FilesSystem {
        private:
            static void checkFileMode(FileHandler* _handler, const FileMode& _expected);

        public:
            static FileHandler* open(const std::string& _filePath, const FileMode& _fileMode);
            static void close(FileHandler* _file);

            static FileStr readFullFile(FileHandler* _handler);
            static FileLines readAllLinesFile(FileHandler* _handler);
            static FileStr readChunkFile(FileHandler* _handler, int _initByte, int _endByte);
            static FileStr readLineInFile(FileHandler* _handler, int _line);
            static FileLines readLinesInFile(FileHandler* _handler, std::vector<int>& _lines);


            static void writeChunkToFile(FileHandler* _handler, const char* _content, size_t _size);
            static void writeChunkToFile(FileHandler* _handler, const std::string& _content);

            static void appendChunkToFile(FileHandler* _handler, const char* _content, size_t _size, int _where);
            static void appendChunkToFile(FileHandler* _handler, const std::string& _content, int _where);

            static void appendChunkToFileAtEnd(FileHandler* _handler, const char* _content, size_t _size);
            static void appendChunkToFileAtEnd(FileHandler* _handler, const std::string& _content);

            static void appendChunkInLineToFile(FileHandler* _handler, const char* _content, size_t _size, int _line);
            static void appendChunkInLineToFile(FileHandler* _handler, const std::string& _content, int _line);

            static void replaceChunkInFile(FileHandler* _handler, const std::string& _old, const std::string& _new);

            static void clearFile(FileHandler* _handler);

            static void removeFile(std::string& _filePath);
            static void removeChunkLineInFile(FileHandler* _handler, int _line);
            static void removeChunkLinesInFile(FileHandler* _handler, std::vector<int>& _lines);
            static void removeChunk(FileHandler* _handler, int _initByte, int _endByte);

            static FileHandler* createFile(const std::string& _filePath);
            static void moveOrRenameFile(const std::string& _filePath, const std::string& _newName);
    };

}

#endif //ENGINE2_0_FILES_SYSTEM_H
