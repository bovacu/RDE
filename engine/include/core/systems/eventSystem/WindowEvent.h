#pragma once

#ifndef RDE_WINDOW_EVENT_H
#define RDE_WINDOW_EVENT_H

#include "Event.h"

namespace RDE {

    /// This class represents all of the events that involves resizing the window.
    class WindowResizedEvent    : public Event {
        private:
            unsigned int width, height;

        public:
			RDE_FUNC_EXPLICIT WindowResizedEvent(unsigned int _width, unsigned int _height) : width(_width), height(_height) {  }

            /// Returns the new width that resizing event is setting.
            /// @return New width of the window.
			RDE_FUNC_ND unsigned int getWidth() const { return width; };

            /// Returns the new height that resizing event is setting.
            /// @return New height of the window.
			RDE_FUNC_ND unsigned int getHeight() const { return height; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (width = " << width << ", height = " << height << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_WINDOW_MOVED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "WindowMoved"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };






    /// This class represents all of the events that involves moving the window.
    class WindowMovedEvent    : public Event {
        private:
            unsigned int x, y;

        public:
			RDE_FUNC_EXPLICIT WindowMovedEvent(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}

            /// Returns the new position on x that moving event is setting.
            /// @return New x of the window.
			RDE_FUNC_ND unsigned int getX() const { return x; };

            /// Returns the new position on y that moving event is setting.
            /// @return New y of the window.
			RDE_FUNC_ND unsigned int getY() const { return y; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (x = " << x << ", y = " << y << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_WINDOW_RESIZED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "WindowResized"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };



    /// This class represents all of the events that involves changing the display of the window.
    class WindowDisplayChangedEvent  : public Event {
        private:
            int windowDisplayIndex;

        public:
			RDE_FUNC_EXPLICIT WindowDisplayChangedEvent(int _windowDisplayIndex) : windowDisplayIndex(_windowDisplayIndex) {  }
			RDE_FUNC_ND int getWindowDisplayIndex() const { return windowDisplayIndex; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> window display index = " << windowDisplayIndex;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "WindowDisplayChanged"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };



    /// This class represents all of the events that involves minimizing the window.
    class WindowMinimizedEvent  : public Event {
        private:
            int minimized;

        public:
			RDE_FUNC_EXPLICIT WindowMinimizedEvent(int _minimized) : minimized(_minimized) {  }
			RDE_FUNC_ND int getMinimized() const { return minimized; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (minimized = " << minimized;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_WINDOW_MINIMIZED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "WindowMinimized"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };





    /// This class represents all of the events that involves closing the window.
    class WindowClosedEvent     : public Event {
        public:
			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_WINDOW_CLOSED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "WindowClosed"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };




    /// This class represents all of the events that involves framing the window.
    class WindowFrameEvent      : public Event {
        public:
			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_GAME_FRAME; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "GameFrame"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };





    /// This class represents all of the events that involves updating the window.
    class WindowUpdateEvent     : public Event {
        public:
			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_GAME_UPDATE; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "GameUpdate"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };





    /// This class represents all of the events that involves rendering the window.
    class WindowRenderEvent     : public Event {
        public:
			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_GAME_RENDER; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "GameRender"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_GAME; }
    };
}

#endif //RDE_WINDOW_EVENT_H
