// Created by borja on 14/2/22.


#ifndef RDE_FILES_SYSTEM_H
#define RDE_FILES_SYSTEM_H

#include <vector>
#include <unordered_map>
#include "core/Enums.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

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
		RDE_FILE_RESULT_ result = RDE_FILE_RESULT_OK;
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
			RDE_FILE_MODE_ mode;

            /**
             * @brief Path to the file.
             */
            std::string originalPath;

        private:
			FileHandler(SDL_RWops* _file, const RDE_FILE_MODE_& _mode, const std::string& _originalPath) : file(_file), mode(_mode), originalPath(_originalPath) {}

            /**
             * @brief Converts engine's file mode to SDL file mode.
             * @param _mode
             * @return const char*
             */
			static const char* modeToChar(const RDE_FILE_MODE_& _mode) {
                switch (_mode) {
					case RDE_FILE_MODE_READ: return "rb";
					case RDE_FILE_MODE_WRITE: return "w";
					case RDE_FILE_MODE_APPEND: return "a";
					case RDE_FILE_MODE_READ_AND_WRITE: return "r+";
					case RDE_FILE_MODE_READ_AND_APPEND: return "a+";
                }

                return "rb";
            }
    };




    class FileManager {

		friend class ConfigManager;

        private:
            /**
             * @brief Checks the file mode and if it doesn't match, it changes it to the expected.
             * @param _handler File handler
             * @param _expected Expected file mode
             */
			void checkFileMode(FileHandler* _handler, const RDE_FILE_MODE_& _expected);

        public:

            /**
             * @brief Opens a file, it must exist.
             * @param _filePath Path to the file
             * @param _fileMode File mode
             * @param _silentNotFound Don't shout if there is an error
             * @return FileHandler*
             */
			RDE_FUNC_ND FileHandler* open(const std::string& _filePath, const RDE_FILE_MODE_& _fileMode, bool _silentNotFound = false);

            /**
             * @brief Closes a file.
             * @param _file File handler
             */
			RDE_FUNC void close(FileHandler* _file);

            /**
             * @brief Reads the full file into a string.
             * @param _handler File handler
             * @return FileStr
             */
			RDE_FUNC_ND FileStr readFullFile(FileHandler* _handler);

            /**
             * @brief Reads the full file into separated lines.
             * @param _handler File handler
             * @return FileLines
             */
			RDE_FUNC_ND FileLines readAllLinesFile(FileHandler* _handler);

            /**
             * @brief Reads a chunk of a file from init point to end point.
             * @param _handler File handler
             * @param _initByte Init point to read. 1 -> 1 char, 2 -> 2 chars ...
             * @param _endByte End point to read 1 -> 1 char, 2 -> 2 chars ...
             * @return FileStr
             */
			RDE_FUNC_ND FileStr readChunkFile(FileHandler* _handler, int _initByte, int _endByte);

            /**
             * @brief Reads a line in the file.
             * @param _handler File handler
             * @param _line Line to read
             * @return FileStr
             */
			RDE_FUNC_ND FileStr readLineInFile(FileHandler* _handler, int _line);

            /**
             * @brief Reads a list of lines in the file.
             * @param _handler File handler
             * @param _lines List of lines to read
             * @return FileLines
             */
			RDE_FUNC_ND FileLines readLinesInFile(FileHandler* _handler, std::vector<int>& _lines);

            /**
             * @brief Writes some text to the file and replaces its previous content.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             */
			RDE_FUNC void writeChunkToFile(FileHandler* _handler, const char* _content, size_t _size);

            /**
             * @brief Writes some text to the file and replaces its previous content.
             * @param _handler File handler
             * @param _size Size of the text
             */
			RDE_FUNC void writeChunkToFile(FileHandler* _handler, const std::string& _content);

            /**
             * @brief Writes some text to the file on an specific point.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _where Point in the file. 1 -> after character 1, 15 -> after character 15 ...
             */
			RDE_FUNC void appendChunkToFile(FileHandler* _handler, const char* _content, size_t _size, int _where);

            /**
             * @brief Writes some text to the file on an specific point.
             * @param _handler File handler
             * @param _content Text to write
             * @param _where Point in the file. 1 -> after character 1, 15 -> after character 15 ...
             */
			RDE_FUNC void appendChunkToFile(FileHandler* _handler, const std::string& _content, int _where);

            /**
             * @brief Writes some text at the end of the current file content.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             */
			RDE_FUNC void appendChunkToFileAtEnd(FileHandler* _handler, const char* _content, size_t _size);

            /**
             * @brief Writes some text at the end of the current file content.
             * @param _handler File handler
             * @param _content Text to write
             */
			RDE_FUNC void appendChunkToFileAtEnd(FileHandler* _handler, const std::string& _content);

            /**
             * @brief Writes some text in an specific line of the file. The previous content of the line is moved to the
             * next line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _line Line to write the text
             */
			RDE_FUNC void appendChunkInLineToFile(FileHandler* _handler, const char* _content, size_t _size, int _line);

            /**
            * @brief Writes some text in an specific line of the file. The previous content of the line is moved to the
            * next line.
            * @param _handler File handler
            * @param _content Text to write
            * @param _line Line to write the text
            */
			RDE_FUNC void appendChunkInLineToFile(FileHandler* _handler, const std::string& _content, int _line);

            /**
             * @brief Writes some text at the end of an specific line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _size Size of the text
             * @param _line Line to write the text
             */
			RDE_FUNC void appendChunkAtEndOfLine(FileHandler* _handler, const char* _content, size_t _size, int _line);

            /**
             * @brief Writes some text at the end of an specific line.
             * @param _handler File handler
             * @param _content Text to write
             * @param _line Line to write the text
             */
			RDE_FUNC void appendChunkAtEndOfLine(FileHandler* _handler, const std::string& _content, int _line);

            /**
             * @brief Replaces all of the _old occurrences with the _new text.
             * @param _handler File handler
             * @param _old Text to be replaced
             * @param _new Replacing text
             */
			RDE_FUNC void replaceChunkInFile(FileHandler* _handler, const std::string& _old, const std::string& _new);

            /**
             * @brief Empties the file content
             * @param _handler File handler
             */
			RDE_FUNC void clearFile(FileHandler* _handler);

            /**
             * @brief Deletes the file from the file system.
             * @param _filePath Path to the file to delete
             */
			RDE_FUNC void removeFile(std::string& _filePath);

            /**
             * @brief Removes a line from the file.
             * @param _handler File handler
             * @param _line Line to remove
             */
			RDE_FUNC void removeChunkLineInFile(FileHandler* _handler, int _line);

            /**
             * @brief Removes a list of lines from the file.
             * @param _handler File handler
             * @param _lines List of lines to remove
             */
			RDE_FUNC void removeChunkLinesInFile(FileHandler* _handler, std::vector<int>& _lines);

            /**
             * @brief Removes an amount of text from the file
             * @param _handler File handler
             * @param _initByte Init point to read. 1 -> 1 char, 2 -> 2 chars ...
             * @param _endByte End point to read 1 -> 1 char, 2 -> 2 chars ...
             */
			RDE_FUNC void removeChunk(FileHandler* _handler, int _initByte, int _endByte);

            /**
             * @brief Creates a new file in the file system
             * @param _filePath Path to the new file
             * @return FileHandler*
             */
			RDE_FUNC_ND FileHandler* createFile(const std::string& _filePath);

            /**
             * @brief Renames or moves a file in the file system.
             * @param _filePath Path to the file
             * @param _newName New name or path for the file
             */
			RDE_FUNC void moveOrRenameFile(const std::string& _filePath, const std::string& _newName);

            /**
             * @brief Returns if the file exists or not.
             * @param _pathToFile Path to file
             * @return bool
             */
			RDE_FUNC_ND bool fileExists(const std::string& _pathToFile);
    };

}

#endif //RDE_FILES_SYSTEM_H
