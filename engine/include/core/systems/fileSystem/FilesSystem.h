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

    template<typename T>
    struct File {
        T content;
        FileResult result = FileResult::FILE_OK;
    };

    typedef File<std::string> FileStr;
    typedef File<std::vector<std::string>> FileLines;

    class FilesSystem {
        public:
            static FileStr readFullFile(const std::string& _filePath);
            static FileStr readChunkFile(const std::string& _filePath, int _initByte, int _endByte);
            static FileStr readLineInFile(const std::string& _filePath, int _line);
            static FileLines readLinesInFile(const std::string& _filePath, std::vector<int>& _lines);


            static void writeChunkToFile(const std::string& _filePath, void* _content);
            static void appendChunkToFile(const std::string& _filePath, void* _content, int _where);
            static void appendChunkInLineToFile(const std::string& _filePath, void* _content, int _line);
            static void replaceChunkInFile(const std::string& _old, const std::string& _new);


            static void removeFile(std::string& _filePath);
            static void removeChunkLineInFile(std::string& _filePath, int _line);
            static void removeChunkLinesInFile(std::string& _filePath, std::vector<int> _lines);
            static void removeChunk(const std::string& _filePath, int _initByte, int _endByte);

            static void createFile(const std::string& _filePath);
            static void renameFile(const std::string& _filePath, const std::string& _newName, const std::string& _newExtension = "");
    };

}

#endif //ENGINE2_0_FILES_SYSTEM_H
