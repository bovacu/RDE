// Created by borja on 10/2/22.


#ifndef RDE_MOBILE_EVENT_H
#define RDE_MOBILE_EVENT_H

#include "Event.h"
#include "core/util/Vec.h"

namespace RDE {

    typedef uint32_t TimeStamp;

    /**
     * @brief Event for click on screen.
     */
    class MobileTouchDownEvent : public Event {
        private:
            Vec2F touchPos;
            float pressure;
            TimeStamp initOfTouch;
            int fingerID;

        public:
            MobileTouchDownEvent(const Vec2F& _touchPos, float _pressure, TimeStamp _initOfTouch, int _fingerID)
                : touchPos(_touchPos), pressure(_pressure), initOfTouch(_initOfTouch), fingerID(_fingerID) {  }

            /**
             * @brief Returns the position where the click happened.
             * @return Vec2F
             */
            [[nodiscard]] inline Vec2F getTouchPos() const { return touchPos; }

            /**
             * @brief Returns the pressure used to click on screen.
             * @return float
             */
            [[nodiscard]] inline float getPressure() const { return pressure; }

            /**
             * @brief Returns when the touch started.
             * @return TimeStamp
             */
            [[nodiscard]] inline TimeStamp getInitOfTouch() const { return initOfTouch; }

            /**
             * @brief Returns ID of the finger used to click.
             * @return int
             */
            [[nodiscard]] inline int getFingerID() const { return fingerID; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": FingerID: " << fingerID << "Pos: " << touchPos << ", Pressure: " << pressure << ", Init Of Touch: " << initOfTouch;
                return _sst.str();
            }

            static EventType getStaticType() { return EventType::MobileTouchDown; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "MobileTouchDown"; }
            [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMobileInput; }
    };

    // With a combination of this event and the touchDown we can get the distance traveled and how much did it take
    class MobileTouchUpEvent : public Event {
        private:
            Vec2F touchPos;
            TimeStamp endOfTouch;
            int fingerID;

        public:
            explicit MobileTouchUpEvent(const Vec2F& _touchPos, TimeStamp _endOfTouch, int _fingerID)
                : touchPos(_touchPos), endOfTouch(_endOfTouch), fingerID(_fingerID) {  }

            /**
             * @brief Returns where the touch ended.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getTouchPos() const { return touchPos; }

            /**
             * @brief Returns when the touch ended.
             * @return TimeStamp
             */
            [[nodiscard]] TimeStamp getEndOfTouch() const { return endOfTouch; }

            /**
             * @brief Returns ID of the finger used to click.
             * @return int
             */
            [[nodiscard]] int getFingerID() const { return fingerID; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": FingerID: " << fingerID << " Position: " << touchPos << ", End Of Touch: " << endOfTouch;
                return _sst.str();
            }

            static EventType getStaticType() { return EventType::MobileTouchUp; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "MobileTouchUp"; }
            [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMobileInput; }
    };

    // This gives various points from the start of the touch until up, may be used to create trails
    class MobileTouchMovedEvent : public Event {
        private:
        Vec2F init, end;

        public:
        MobileTouchMovedEvent(const Vec2F& _init, const Vec2F &_end) : init(_init), end(_end) {  }

        /**
         * @brief Returns the point where the move started.
         * @return Vec2F
         */
        [[nodiscard]] inline Vec2F getInit() const { return init; }

            /**
             * @brief Returns the point where the move ended.
             * @return Vec2F
             */
        [[nodiscard]] inline Vec2F getEnd() const { return end; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream _sst;
            _sst << getName() << "Init: " << init << ", End: " << end;
            return _sst.str();
        }

        static EventType getStaticType() { return EventType::MobileTouchMoved; }
        [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
        [[nodiscard]] const char* getName() const override { return "MobileTouchMoved"; }
        [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMobileInput; }
    };

}

#endif //RDE_MOBILE_EVENT_H
