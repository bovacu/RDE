#pragma once

#ifndef ENGINE2_0_WINDOW_EVENT_H
#define ENGINE2_0_WINDOW_EVENT_H

#include "core/render/window/event/Event.h"

namespace engine {

    /// This class represents all of the events that involves resizing the window.
    class WindowResizedEvent    : public Event {
        private:
            unsigned int width, height;

        public:
            WindowResizedEvent(unsigned int _width, unsigned int _height) : width(_width), height(_height) {  }

            /// Returns the new width that resizing event is setting.
            /// @return New width of the window.
            [[nodiscard]] inline unsigned int getWidth() const { return this->width; };

            /// Returns the new height that resizing event is setting.
            /// @return New height of the window.
            [[nodiscard]] inline unsigned int getHeight() const { return this->height; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (width = " << this->width << ", height = " << this->height << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::WindowMoved; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "WindowMoved"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves moving the window.
    class WindowMovedEvent    : public Event {
        private:
            unsigned int x, y;

        public:
            WindowMovedEvent(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}

            /// Returns the new position on x that moving event is setting.
            /// @return New x of the window.
            [[nodiscard]] inline unsigned int getX() const { return this->x; };

            /// Returns the new position on y that moving event is setting.
            /// @return New y of the window.
            [[nodiscard]] inline unsigned int getY() const { return this->y; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (x = " << this->x << ", y = " << this->y << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::WindowResized; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "WindowResized"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves minimizing the window.
    class WindowMinimizedEvent  : public Event {
        private:
            int minimized;

        public:
            explicit WindowMinimizedEvent(int _minimized) : minimized(_minimized) {  }
            [[nodiscard]] int getMinimized() const { return this->minimized; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << " -> (minimized = " << this->minimized;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::WindowMinimized; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "WindowMinimized"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves closing the window.
    class WindowClosedEvent     : public Event {
        public:

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::WindowClosed; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "WindowClosed"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves framing the window.
    class WindowFrameEvent      : public Event {
        public:
            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::GameFrame; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "GameFrame"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves updating the window.
    class WindowUpdateEvent     : public Event {
        public:
            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::GameUpdate; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "GameUpdate"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };

    /// This class represents all of the events that involves rendering the window.
    class WindowRenderEvent     : public Event {
        public:
            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName();
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::GameRender; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "GameRender"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryGame; }
    };
}

#endif //ENGINE2_0_WINDOW_EVENT_H
