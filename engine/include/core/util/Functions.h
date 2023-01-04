// Created by borja on 23/12/21.


#ifndef RDE_FUNCTIONS_H
#define RDE_FUNCTIONS_H

#include "core/Core.h"
#include "core/util/Vec.h"
#include "core/platform/PlatformHeaderSDL.h"
#include "core/platform/PlatformHeaderSDLImage.h"

#include <sstream>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <vcruntime.h>
#include <vector>

namespace RDE {

    class Window; 
    class CPUTexture; 
    class ViewPort;

    namespace Util {

        #ifndef UTIL_MATH
        #define UTIL_MATH
        namespace Math {

            Vec2F worldToScreenCoords(const ViewPort* _viewport, const Vec2F& _position);
            Vec2F screenToWorldCoords(const ViewPort* _viewport, const Vec2F& _position);
            Vec2F worldToScreenSize(const ViewPort* _viewport, const Vec2F& _size);
            Vec2F worldToScreenCoordsUI(const ViewPort* _viewport, const Vec2F& _position);
            Vec2F worldToScreenSizeUI(const ViewPort* _viewport, const Vec2F& _size);
            void worldToScreenSize(const ViewPort* _viewport, float& _x, float& _y);
            void screenToWorldCoords(const ViewPort* _viewport, float& _x, float& _y);

            template<typename T>
            inline T clamp(T _value, T _minValue, T _maxValue) {
                return (_value < _minValue) ? _minValue : (_value > _maxValue) ? _maxValue : _value;
            }

            inline int clampI(int _value, int _min, int _max) {
                return clamp<int>(_value, _min, _max);
            }

            inline float clampF(float _value, float _min, float _max) {
                return clamp<float>(_value, _min, _max);
            }

            inline double clampD(double _value, double _min, double _max) {
                return clamp<double>(_value, _min, _max);
            }

            inline long clampL(long _value, long _min, long _max) {
                return clamp<long>(_value, _min, _max);
            }

            inline bool approximatelyEqual(float _a, float _b) {
                return std::abs( _a - _b ) <= EPSILON;
            }

            inline bool approximatelyEqual(float _a, float _b, float _epsilon) {
                return std::abs( _a - _b ) <= _epsilon;
            }

            inline bool approximatelyEqual(const Vec2F& _a, const Vec2F &_b) {
                return std::abs( _a.x - _b.x ) <= EPSILON && std::abs(_a.y - _b.y) <= EPSILON;
            }

            inline bool approximatelyEqual(const Vec2F& _a, const Vec2F &_b, float _epsilon) {
                return std::abs( _a.x - _b.x ) <= _epsilon && std::abs(_a.y - _b.y) <= _epsilon;
            }

            inline float radiansToDegrees(float _radians) {
                return _radians * (180.f / PI);
            }

            inline float degreesToRadians(float _degrees) {
                return _degrees * (PI / 180.0f);
            }
        }
        #endif

        #ifndef UTIL_EASING
        #define UTIL_EASING
        namespace Easing {
            /// Linear
            inline float inLinear(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return _changeInValue * _currentTime / _duration + _startValue;
            }

            /// Quadratic
            inline float inQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return _changeInValue * _currentTime * _currentTime + _startValue;
            }

            inline float outQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return -_changeInValue * _currentTime * (_currentTime - 2) + _startValue;
            }

            inline float inOutQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1) return _changeInValue / 2 * _currentTime * _currentTime + _startValue;
                _currentTime--;
                return -_changeInValue / 2 * (_currentTime * (_currentTime - 2) - 1) + _startValue;
            }

            /// Cubic
            inline float inCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return _changeInValue * _currentTime * _currentTime * _currentTime + _startValue;
            }

            inline float outCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                _currentTime--;
                return _changeInValue * (_currentTime * _currentTime * _currentTime + 1) + _startValue;
            }

            inline float inOutCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1) return _changeInValue / 2 * _currentTime * _currentTime * _currentTime + _startValue;
                _currentTime -= 2;
                return _changeInValue / 2 * (_currentTime * _currentTime * _currentTime + 2) + _startValue;
            }

            /// Quartic
            inline float inQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return _changeInValue * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
            }

            inline float outQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                _currentTime--;
                return -_changeInValue * (_currentTime * _currentTime * _currentTime * _currentTime - 1) + _startValue;
            }

            inline float inOutQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1)
                    return _changeInValue / 2 * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
                _currentTime -= 2;
                return -_changeInValue / 2 * (_currentTime * _currentTime * _currentTime * _currentTime - 2) + -_startValue;
            }

            /// Quintic
            inline float inQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return _changeInValue * _currentTime * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
            }

            inline float outQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                _currentTime--;
                return _changeInValue * (_currentTime * _currentTime * _currentTime * _currentTime * _currentTime + 1) +
                       _startValue;
            }

            inline float inOutQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1)
                    return _changeInValue / 2 * _currentTime * _currentTime * _currentTime * _currentTime * _currentTime +
                           _startValue;
                _currentTime -= 2;
                return _changeInValue / 2 * (_currentTime * _currentTime * _currentTime * _currentTime * _currentTime + 2) +
                       _startValue;
            }

            /// Sinusoidal
            inline float inSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return -_changeInValue * (float) std::cos(_currentTime / _duration * (PI / 2)) + _changeInValue + _startValue;
            }

            inline float outSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return _changeInValue * (float) std::sin(_currentTime / _duration * (PI / 2)) + _startValue;
            }

            inline float inOutSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return -_changeInValue / 2 * ((float) std::cos(PI * _currentTime / _duration) - 1) + _startValue;
            }

            /// Exponential
            inline float inExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return _changeInValue * (float) std::pow(2, 10 * (_currentTime / _duration - 1)) + _startValue;
            }

            inline float outExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                return _changeInValue * (-(float) std::pow(2, -10 * _currentTime / _duration) + 1) + _startValue;
            }

            inline float inOutExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1) return _changeInValue / 2 * (float) std::pow(2, 10 * (_currentTime - 1)) + _startValue;
                _currentTime--;
                return _changeInValue / 2 * (-(float) std::pow(2, -10 * _currentTime) + 2) + _startValue;
            }

            /// Exponential
            inline float inCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                return -_currentTime * ((float) std::sqrt(1 - _currentTime * _currentTime) - 1) + _startValue;
            }

            inline float outCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration;
                _currentTime--;
                return _changeInValue * (float) std::sqrt(1 - _currentTime * _currentTime) + _startValue;
            }

            inline float inOutCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
                _currentTime /= _duration / 2;
                if (_currentTime < 1)
                    return -_changeInValue / 2 * ((float) std::sqrt(1 - _currentTime * _currentTime) - 1) + _startValue;
                _currentTime -= 2;
                return _changeInValue / 2 * ((float) std::sqrt(1 - _currentTime * _currentTime) + 1) + _startValue;
            }
        }
        #endif

        #ifndef UTIL_STRING
        #define UTIL_STRING
        namespace String {
            inline std::string getFileNameFromPath(const std::string& _path) {
                std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
                std::string::size_type const p(base_filename.find_last_of('.'));
                return base_filename.substr(0, p);
            }

            inline std::string getPathFromFilePath(const std::string& _path) {
                return _path.substr(0, _path.find_last_of("/\\") + 1);
            }

            inline std::string getFileExtension(const std::string& _path) {
                std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
                return base_filename.substr(base_filename.find_last_of('.') + 1, base_filename.size());
            }

            inline std::string& toLower(std::string& _string) {
                std::transform(_string.begin(), _string.end(), _string.begin(), [](unsigned char c){ return std::tolower(c); });
                return _string;
            }

            inline std::string& toUpper(std::string& _string) {
                std::transform(_string.begin(), _string.end(), _string.begin(), [](unsigned char c){ return std::toupper(c); });
                return _string;
            }

            inline std::vector<std::string> split(const std::string& _str, const std::string& _delimiter) {
                size_t pos = 0;
                std::string token;
                std::string _cpyStr = _str;
                std::vector<std::string> _splits;

                while ((pos = _cpyStr.find(_delimiter)) != std::string::npos) {
                    token = _cpyStr.substr(0, pos);
                    _splits.push_back(token);
                    _cpyStr.erase(0, pos + _delimiter.length());
                }

                _splits.push_back(_cpyStr);
                return _splits;
            }

            inline std::string splitGetIndex(const std::string& _str, const std::string& _delimiter, int _index) {
                return split(_str, _delimiter)[_index];
            }

            inline void replaceAll( std::string& _str, const std::string& _search, const std::string& _replace) {
                for( size_t pos = 0; ; pos += _replace.length() ) {
                    pos = _str.find( _search, pos );
                    if( pos == std::string::npos ) break;

                    _str.erase( pos, _search.length() );
                    _str.insert( pos, _replace );
                }
            }

            template <typename Arg1>
            inline std::string __append(std::stringstream& _ss, Arg1&& arg1) {
                _ss << arg1;
                return _ss.str();
            }

            template <typename Arg1, typename... Args>
            inline std::string __append(std::stringstream& _ss, Arg1&& arg1, Args&&... args) {
                _ss << arg1;
                return __append(_ss, args...);
            }

            template <typename Arg1, typename... Args>
            inline std::string appendToString(Arg1&& arg1, Args&&... args) {
                std::stringstream _ss;
                return __append(_ss, arg1, args...);
            }
        }
        #endif

        #ifndef UTIL_LOG
        #define UTIL_LOG
        namespace Log {
            inline void __printTime(const char* _init, const char* _end) {
                std::time_t t = std::time(nullptr);   // get time now

                #if IS_WINDOWS()
                std::tm _now {};
                localtime_s(&_now, &t);
                std::tm* now = &_now;
                #else
                std::tm* now = std::localtime(&t);
                #endif

                #if IS_DESKTOP()
                std::cout << _init << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] " << _end;
                #endif
            }

            template <typename Arg1>
            inline void __printer(const char* _init, const char* _end, Arg1&& arg1) {
                #if IS_MOBILE()
                std::stringstream _stream;
                _stream << arg1;

                if(strcmp("\033[1;m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                } else if (strcmp("\033[1;33m", _init) == 0) {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;31m", _init) == 0) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;32m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                }
                #elif IS_DESKTOP()
                std::cout << arg1 << _end << std::endl;
                #endif
            }


            template <typename Arg1, typename... Args>
            inline void __printer(const char* _init, const char* _end, Arg1&& arg1, Args&&... args) {
                #if IS_MOBILE()
                std::stringstream _stream;
                _stream << arg1;

                using expander = int[];
                (void)expander{0, (void(_stream << args), 0)...};

                if(strcmp("\033[1;m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                } else if (strcmp("\033[1;33m", _init) == 0) {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;31m", _init) == 0) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;32m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                }
                #elif IS_DESKTOP()
                std::cout << arg1;
                __printer(_init, _end, args...);
                #endif
            }



            inline void printTimed(const char* _init, const char* _end) {
                #ifdef ENGINE_DEBUG
                    std::time_t t = std::time(nullptr);   // get time now

                    #if IS_WINDOWS()
                    std::tm _now {};
                    localtime_s(&_now, &t);
                    std::tm* now = &_now;
                    #else
                    std::tm* now = std::localtime(&t);
                    #endif

                    #if IS_DESKTOP()
                    std::cout << _init << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "] " << _end;
                    #endif
                #endif
            }

            template <typename Arg1>
            inline void printerd(const char* _init, const char* _end, Arg1&& arg1) {
                #ifdef ENGINE_DEBUG
                    #if IS_MOBILE()
                    std::stringstream _stream;
                    _stream << arg1;

                    if(strcmp("\033[1;m", _init) == 0) {
                        SDL_Log("%s", _stream.str().c_str());
                    } else if (strcmp("\033[1;33m", _init) == 0) {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                    } else if (strcmp("\033[1;31m", _init) == 0) {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                    } else if (strcmp("\033[1;32m", _init) == 0) {
                        SDL_Log("%s", _stream.str().c_str());
                    }
                    #elif IS_DESKTOP()
                    std::cout << _init << arg1 << _end << std::endl;
                    #endif
                #endif
            }


            template <typename Arg1, typename... Args>
            inline void printerd(const char* _init, const char* _end, Arg1&& arg1, Args&&... args) {
                #ifdef ENGINE_DEBUG
                #if IS_MOBILE()
                std::stringstream _stream;
                _stream << arg1;

                using expander = int[];
                (void)expander{0, (void(_stream << args), 0)...};

                if(strcmp("\033[1;m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                } else if (strcmp("\033[1;33m", _init) == 0) {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;31m", _init) == 0) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", _stream.str().c_str());
                } else if (strcmp("\033[1;32m", _init) == 0) {
                    SDL_Log("%s", _stream.str().c_str());
                }
                #elif IS_DESKTOP()
                std::cout << _init << arg1;
                __printer(_init, _end, args...);
                #endif
                #endif
            }

            template <typename Arg1, typename... Args>
            void info(Arg1&& _arg1, Args&&... args) {
                #if IS_WINDOWS() || IS_MOBILE()
                std::cout << "[INFO] ";
                __printer("", "", _arg1, args...);
                #else
                __printer("\033[1;m", "\033[0m", _arg1, args...);
                #endif
            }

            template <typename Arg1, typename... Args>
            void warn(Arg1&& _arg1, Args&&... args) {
                #if IS_WINDOWS() || IS_MOBILE()
                std::cout << "[WARN] ";
                __printer("", "", _arg1, args...);
                #else
                __printer("\033[1;m", "\033[0m", _arg1, args...);
                #endif
            }

            template <typename Arg1, typename... Args>
            void error(Arg1&& _arg1, Args&&... args) {
                #if IS_WINDOWS() || IS_MOBILE()
                std::cout << "[ERROR] ";
                __printer("", "", _arg1, args...);
                #else
                __printer("\033[1;31m", "\033[0m", _arg1, args...);
                #endif
            }

            template <typename Arg1, typename... Args>
            void success(Arg1&& _arg1, Args&&... args) {
                #if IS_WINDOWS() || IS_MOBILE()
                std::cout << "[SUCCESS] ";
                __printer("", "", _arg1, args...);
                #else
                __printer("\033[1;32m", "\033[0m", _arg1, args...);
                #endif

            }

            template <typename Arg1, typename... Args>
            void debug(Arg1&& _arg1, Args&&... args) {
                #if IS_WINDOWS() || IS_MOBILE()
                std::cout << "[DEBUG] ";
                __printer("", "", _arg1, args...);
                #else
                __printer("\033[1;m", "\033[0m", _arg1, args...);
                #endif
            }
        }
        #endif

        #ifndef UTIL_TEXTURE
        #define UTIL_TEXTURE
        namespace Texture {
            /**
             * @brief SDL2 loads the texture backwards so this returns it to its normal state.
             * @param surface SDL2 surface needed to load the texture.
             * @return int
             */
            inline int invertSDLSurface(SDL_Surface* surface) {
                #define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
                #define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

                Uint8 *t;
                Uint8 *a, *b;
                Uint8 *last;
                Uint16 pitch;

                if( SDL_LOCKIFMUST(surface) < 0 )
                    return -2;

                /* do nothing unless at least two lines */
                if(surface->h < 2) {
                    SDL_UNLOCKIFMUST(surface);
                    return 0;
                }

                /* get a place to store a line */
                pitch = surface->pitch;
                t = (Uint8*)malloc(pitch);

                if(t == nullptr) {
                    SDL_UNLOCKIFMUST(surface);
                    return -2;
                }

                /* get first line; it's about to be trampled */
                memcpy(t,surface->pixels,pitch);

                /* now, shuffle the rest so it's almost correct */
                a = (Uint8*)surface->pixels;
                last = a + pitch * (surface->h - 1);
                b = last;

                while(a < b) {
                    memcpy(a,b,pitch);
                    a += pitch;
                    memcpy(b,a,pitch);
                    b -= pitch;
                }

                /* in this shuffled state, the bottom slice is too far down */
                memmove( b, b+pitch, last-b );

                /* now we can put back that first row--in the last place */
                memcpy(last,t,pitch);

                /* everything is in the right place; close up. */
                free(t);
                SDL_UNLOCKIFMUST(surface);

                return 0;
            }

            /**
             * @brief Generates and SDL2 surface to work with the texture when loading it. Remember to free the surface!
             * @param _imageFile Raw data of the SpriteSheet.
             * @param _pathToFile Path to the file.
             * @return SDL_Surface*
             */
            inline SDL_Surface* getSDLSurface(const std::string& _pathToFile) {
                SDL_RWops* _imageFile = SDL_RWFromFile(_pathToFile.c_str(), "rb");

                if(_imageFile == nullptr) {
                    Util::Log::error("Couldn't load ", _pathToFile);
                    return nullptr;
                }

                SDL_Surface* _surface = nullptr;

                auto _extension = Util::String::getFileExtension(_pathToFile);

                if(strcmp(_extension.c_str(), "png") == 0)
                    _surface = IMG_LoadPNG_RW(_imageFile);
                else if(strcmp(_extension.c_str(), "jpg") == 0 || strcmp(_extension.c_str(), "jpeg") == 0)
                    _surface = IMG_LoadJPG_RW(_imageFile);
                else if(strcmp(_extension.c_str(), "svg") == 0)
                    _surface = IMG_LoadSVG_RW(_imageFile);
                else if(strcmp(_extension.c_str(), "bmp") == 0)
                    _surface = IMG_LoadBMP_RW(_imageFile);
                else if(strcmp(_extension.c_str(), "ico") == 0)
                    _surface = IMG_LoadICO_RW(_imageFile);

                SDL_RWclose(_imageFile);

                return _surface;
            }
        }
        #endif

        #ifndef UTIL_GL
        #define UTIL_GL
        namespace GL {
            void checkError(const std::string& _message);
        }
        #endif

        #ifndef UTIL_GRAPHICS
        #define UTIL_GRAPHICS
        namespace Graphics {

            /**
             * @brief Returns the pixels of the current drawn screen.
             * @warn The pointer returned must be freed after use!
             * @param  _window Windows of the application
             * @return unsigned char*
             */
            unsigned char* getScreenPixels(Window& _window);

            /**
             * @brief Returns the pixels of a specific are of the current drawn screen.
             * @warn The pointer returned must be freed after use!
             * @param  _window Windows of the application
             * @param _position Center position of the screenshot to be taken. The coordinates system is centered on the screen center, as the whole engine.
             * @param _size     Size of the screenshoot, it will be x/2 to each X-axis side and y/2 to each Y-axis side.
             * @return unsigned char*
             */
            unsigned char* getAreaOfScreenPixels(Window& _window, const Vec2I& _position, const Vec2I& _size);

            /**
             * @brief Takes a screenshot of the whole application window and saves it to a path. The name and extension of the screenshot has to be provided.
             * @param _window   The application Window.
             * @param _path     Full path to where the image is saved including the name and extension of the file. Supported extensions [png, jpg, jpeg, bmp].
             */
            void takeScreenshot(Window& _window, const std::string& _path);

            /**
             * @brief Takes a screenshot of an area of window and saves it to a path. The name and extension of the screenshot has to be provided.
             * @param _window   The application Window.
             * @param _position Center position of the screenshot to be taken. The coordinates system is centered on the screen center, as the whole engine.
             * @param _size     Size of the screenshoot, it will be x/2 to each X-axis side and y/2 to each Y-axis side.
             * @param _path     Full path to where the image is saved including the name and extension of the file. Supported extensions [png, jpg, jpeg, bmp].
             */
            void takeScreenshot(Window& _window, const Vec2I& _position, const Vec2I& _size, const std::string& _path);

            /**
             * @brief Transforms an array of pixels into a CPUTexture so they can be manipulated and rendered easier.
             * @param  _size   Size of the pixels in rows and columns.
             * @param  _pixels Array of pixels.
             * @return         CPUTexture*
             * @warn  If the CPUTexture is not going to be attached to a renderer component, user need to manually free it. If attached to an IRenderizable, the engine
             *        will destroy it automatically.
             */
            CPUTexture* pixelsToCPUTexture(const Vec2I& _size, unsigned char* _pixels);

        }
        #endif
    }

}

#endif //RDE_FUNCTIONS_H
