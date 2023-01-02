// Created by borja on 14/2/22.


#ifndef RDE_FILES_SYSTEM_H
#define RDE_FILES_SYSTEM_H

#include "core/util/Util.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /**
     * @brief Result of a file operation.
     */
    enum FileResult {
        FILE_OK,
        FILE_NOT_FOUND,
        FILE_LINE_OUT_OF_SCOPE,
        FILE_NOT_ALL_LINES_IN_SCOPE
    };

    /**
     * @brief Different types of file operations.
     */
    enum FileMode {
        READ,
        WRITE,
        APPEND,
        READ_AND_WRITE,
        READ_AND_APPEND,
    };

    /**
     * @brief This represents the content of a file.
     * @tparam T Type of content
     */
    template<typename T>
    struct File {
        /**
         * @brief Type of content, usually a simple string or a std::vector<string>
         */
        T content;

        /**
         * @brief Result of the operation.
         */
        FileResult result = FileResult::FILE_OK;
    };
    typedef File<std::string> FileStr;
    typedef File<std::vector<std::string>> FileLines;


    class FileManager;
    /**
     * @brief This class representes a loaded file.
     */
    class FileHandler {
        
        friend class FileManager;

        private:
            /**
             * @brief Raw data of the file.
             */
            SDL_RWops* file;

            /**
             * @brief Operations that it will work with.
             */
            FileMode mode;

            /**
             * @brief Path to the file.
             */
            std::string originalPath;

        private:
            FileHandler(SDL_RWops* _file, const FileMode& _mode, const std::string& _originalPath) : file(_file), mode(_mode), originalPath(_originalPath) {}

            /**
             * @brief Converts engine's file mode to SDL file mode.
             * @param _mode
             * @return const char*
             */
            static const char* modeToChar(const FileMode& _mode) {
                switch (_mode) {
                    case READ: return "rb";
                    case WRITE: return "w";
                    case APPEND: return "a";
                    case READ_AND_WRITE: return "r+";
                    case READ_AND_APPEND: return "a+";
                }

                return "rb";
            }
    };




    class FileManager {
        private:
            /**
             * @brief Checks the file mode and if it doesn't match, it changes it to the expected.
             * @param _handler File handler
             * @param _expected Expected file mode
             */
            void checkFileMode(FileHandler* _handler, const FileMode& _expected);

        public:

            /**
             * @brief Opens a file, it must exist.
             * @param _filePath Path to the file
             * @param _fileMode File mode
             * @param _silentNotFound Don't shout if there is an error
             * @return FileHandler*
             */
            FileHandler* open(const std::string& _filePath, const FileMode& _fileMode, bool _silentNotFound = false);

            /**
             * @brief Closes a file.
             * @param _file File handler
             */
            void close(FileHandler* _file);

            /**
             * @brief Reads the full file into a string.
             * @param _handler File handler
             * @return FileStr
             */
            FileStr readFullFile(FileHandler* _handler);

            /**
             * @brief Reads the full file into separated lines.
             * @param _handler File handler
             * @return FileLines
             */
            FileLines readAllLinesFile(FileHandler* _handler);

            /**
             * @brief Reads a chunk of a file from init point to end point.
             * @param _handler File handler
             * @param _initByte Init point to read. 1 -> 1 char, 2 -> 2 chars ...
             * @param _endByte End point to read 1 -> 1 char, 2 -> 2 chars ...
             * @return FileStr
             */
            FileStr readChunkFile(FileHandler* _handler, int _initByte, int _endByte);

            /**
             * @brief Reads a line in the file.
             * @param _handler File handler
             * @param _line Line to read
             * @return FileStr
             */
            FileStr readLineInFile(FileHandler* _handler, int _line);

            /**
             * @brief Reads a list of lines in the file.
             * @param _handler File handler
             * @param _lines List of lines to read
             * @return FileLines
             */
            FileLines readLinesInFile(FileHandler* _handler, std::vector<int>& _lines);

            /**
             * @brief Writes some text to the file and replaces its previous content.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             */
            void writeChunkToFile(FileHandler* _handler, const char* _content, size_t _size);

            /**
             * @brief Writes some text to the file and replaces its previous content.
             * @param _handler File handler
             * @param _size Size of the text
             */
            void writeChunkToFile(FileHandler* _handler, const std::string& _content);

            /**
             * @brief Writes some text to the file on an specific point.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _where Point in the file. 1 -> after character 1, 15 -> after character 15 ...
             */
            void appendChunkToFile(FileHandler* _handler, const char* _content, size_t _size, int _where);

            /**
             * @brief Writes some text to the file on an specific point.
             * @param _handler File handler
             * @param _content Text to write
             * @param _where Point in the file. 1 -> after character 1, 15 -> after character 15 ...
             */
            void appendChunkToFile(FileHandler* _handler, const std::string& _content, int _where);

            /**
             * @brief Writes some text at the end of the current file content.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             */
            void appendChunkToFileAtEnd(FileHandler* _handler, const char* _content, size_t _size);

            /**
             * @brief Writes some text at the end of the current file content.
             * @param _handler File handler
             * @param _content Text to write
             */
            void appendChunkToFileAtEnd(FileHandler* _handler, const std::string& _content);

            /**
             * @brief Writes some text in an specific line of the file. The previous content of the line is moved to the
             * next line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _line Line to write the text
             */
            void appendChunkInLineToFile(FileHandler* _handler, const char* _content, size_t _size, int _line);

            /**
            * @brief Writes some text in an specific line of the file. The previous content of the line is moved to the
            * next line.
            * @param _handler File handler
            * @param _content Text to write
            * @param _line Line to write the text
            */
            void appendChunkInLineToFile(FileHandler* _handler, const std::string& _content, int _line);

            /**
             * @brief Writes some text at the end of an specific line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _line Line to write the text
             */
            void appendChunkAtEndOfLine(FileHandler* _handler, const char* _content, size_t _size, int _line);

            /**
             * @brief Writes some text at the end of an specific line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _line Line to write the text
             */
            void appendChunkAtEndOfLine(FileHandler* _handler, const std::string& _content, int _line);

            /**
             * @brief Replaces all of the _old occurrences with the _new text.
             * @param _handler File handler
             * @param _old Text to be replaced
             * @param _new Replacing text
             */
            void replaceChunkInFile(FileHandler* _handler, const std::string& _old, const std::string& _new);

            /**
             * @brief Empties the file content
             * @param _handler File handler
             */
            void clearFile(FileHandler* _handler);

            /**
             * @brief Deletes the file from the file system.
             * @param _filePath Path to the file to delete
             */
            void removeFile(std::string& _filePath);

            /**
             * @brief Removes a line from the file.
             * @param _handler File handler
             * @param _line Line to remove
             */
            void removeChunkLineInFile(FileHandler* _handler, int _line);

            /**
             * @brief Removes a list of lines from the file.
             * @param _handler File handler
             * @param _lines List of lines to remove
             */
            void removeChunkLinesInFile(FileHandler* _handler, std::vector<int>& _lines);

            /**
             * @brief Removes an amount of text from the file
             * @param _handler File handler
             * @param _initByte Init point to read. 1 -> 1 char, 2 -> 2 chars ...
             * @param _endByte End point to read 1 -> 1 char, 2 -> 2 chars ...
             */
            void removeChunk(FileHandler* _handler, int _initByte, int _endByte);

            /**
             * @brief Creates a new file in the file system
             * @param _filePath Path to the new file
             * @return FileHandler*
             */
            FileHandler* createFile(const std::string& _filePath);

            /**
             * @brief Renames or moves a file in the file system.
             * @param _filePath Path to the file
             * @param _newName New name or path for the file
             */
            void moveOrRenameFile(const std::string& _filePath, const std::string& _newName);

            /**
             * @brief Returns if the file exists or not.
             * @param _pathToFile Path to file
             * @return bool
             */
            bool fileExists(const std::string& _pathToFile);
    };

}

#endif //RDE_FILES_SYSTEM_H
