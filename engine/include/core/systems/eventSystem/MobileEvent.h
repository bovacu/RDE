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
			RDE_FUNC_EXPLICIT MobileTouchDownEvent(const Vec2F& _touchPos, float _pressure, TimeStamp _initOfTouch, int _fingerID)
                : touchPos(_touchPos), pressure(_pressure), initOfTouch(_initOfTouch), fingerID(_fingerID) {  }

            /**
             * @brief Returns the position where the click happened.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getTouchPos() const { return touchPos; }

            /**
             * @brief Returns the pressure used to click on screen.
             * @return float
             */
			RDE_FUNC_ND float getPressure() const { return pressure; }

            /**
             * @brief Returns when the touch started.
             * @return TimeStamp
             */
			RDE_FUNC_ND TimeStamp getInitOfTouch() const { return initOfTouch; }

            /**
             * @brief Returns ID of the finger used to click.
             * @return int
             */
			RDE_FUNC_ND int getFingerID() const { return fingerID; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": FingerID: " << fingerID << "Pos: " << touchPos << ", Pressure: " << pressure << ", Init Of Touch: " << initOfTouch;
                return _sst.str();
            }

			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MobileTouchDown"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOBILE_INPUT; }
    };

    // With a combination of this event and the touchDown we can get the distance traveled and how much did it take
    class MobileTouchUpEvent : public Event {
        private:
            Vec2F touchPos;
            TimeStamp endOfTouch;
            int fingerID;

        public:
			RDE_FUNC_EXPLICIT MobileTouchUpEvent(const Vec2F& _touchPos, TimeStamp _endOfTouch, int _fingerID)
                : touchPos(_touchPos), endOfTouch(_endOfTouch), fingerID(_fingerID) {  }

            /**
             * @brief Returns where the touch ended.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getTouchPos() const { return touchPos; }

            /**
             * @brief Returns when the touch ended.
             * @return TimeStamp
             */
			RDE_FUNC_ND TimeStamp getEndOfTouch() const { return endOfTouch; }

            /**
             * @brief Returns ID of the finger used to click.
             * @return int
             */
			RDE_FUNC_ND int getFingerID() const { return fingerID; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": FingerID: " << fingerID << " Position: " << touchPos << ", End Of Touch: " << endOfTouch;
                return _sst.str();
            }

			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOBILE_TOUCH_UP; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MobileTouchUp"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOBILE_INPUT; }
    };

    // This gives various points from the start of the touch until up, may be used to create trails
    class MobileTouchMovedEvent : public Event {
        private:
        Vec2F init, end;

        public:
			RDE_FUNC_EXPLICIT MobileTouchMovedEvent(const Vec2F& _init, const Vec2F &_end) : init(_init), end(_end) {  }

        	/**
        	 * @brief Returns the point where the move started.
        	 * @return Vec2F
        	 */
			RDE_FUNC_ND Vec2F getInit() const { return init; }

            /**
             * @brief Returns the point where the move ended.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getEnd() const { return end; }

			RDE_FUNC_ND std::string toString() const override {
        	    std::stringstream _sst;
        	    _sst << getName() << "Init: " << init << ", End: " << end;
        	    return _sst.str();
        	}

			RDE_FUNC_ND_STATIC RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MobileTouchMoved"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOBILE_INPUT; }
    };

}

#endif //RDE_MOBILE_EVENT_H
