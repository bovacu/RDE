// Created by borja on 31/12/21.


#ifndef RDE_PROFILER_H
#define RDE_PROFILER_H

#include "core/Core.h"

#include <chrono>
#include <unordered_map>
#include <ostream>
#include <string>
#include "core/graph/ImGuiCore.h"

#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
    typedef unsigned long u_long;
#endif

#ifdef __EMSCRIPTEN__
    typedef unsigned long u_long;
#endif

#if IS_LINUX()
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

namespace RDE {

    enum ProfilerState {
        INPUT,
        RENDERING,
        UPDATE,
        FIXED_UPDATE,
        LATE_UPDATE,
        IMGUI,
        BOTTLE_NECK
    };

    struct State {
        static std::unordered_map<ProfilerState, std::string> stateToNameDict;
        std::chrono::system_clock::time_point init;
        std::chrono::system_clock::time_point end;
        ProfilerState state;
        bool active = false;

		RDE_FUNC_ND std::string toString() const {
            auto _diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - init);
            std::string _toString = State::stateToNameDict[state] + ": " + std::to_string(_diff.count());
            return _toString;
        }
    };

	RDE_FUNC inline std::ostream& operator<<(std::ostream & _ostream, const State& _state) {
        auto _diff = std::chrono::duration_cast<std::chrono::milliseconds>(_state.end - _state.init);
        _ostream << State::stateToNameDict[_state.state] << ": " << std::to_string(_diff.count());
        return _ostream;
    }

#if !IS_MOBILE()
    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;

//        ScrollingBuffer(int max_size = 2000);
//        void AddPoint(float x, float y);
//        void Erase();
    };

    struct RollingBuffer {
        float Span = 0;
        ImVector<ImVec2> Data;
//        RollingBuffer();
//        void AddPoint(float x, float y);
    };
#endif

    class Profiler {
        private:
            static std::unordered_map<ProfilerState, State> states;
            static std::unordered_map<ProfilerState, State> lastStates;
            static struct sysinfo systemInfo;

            static int parseLine(char* line);
            static int getValue();

        public:
			RDE_FUNC_STATIC void beginFrame(float _dt);
			RDE_FUNC_STATIC void begin(ProfilerState _state);
			RDE_FUNC_STATIC void end(ProfilerState _state);
			RDE_FUNC_STATIC void endFrame();
			RDE_FUNC_ND_STATIC std::unordered_map<ProfilerState, State>& getStates();

			RDE_FUNC_ND_STATIC u_long* getTotalVirtualMemory();
    };

}

#endif //RDE_PROFILER_H
