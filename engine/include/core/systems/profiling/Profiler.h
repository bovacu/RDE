// Created by borja on 31/12/21.


#ifndef ENGINE2_0_PROFILER_H
#define ENGINE2_0_PROFILER_H

#include "core/Core.h"

#include <chrono>
#include <unordered_map>
#include <ostream>
#include <cmath>
#include "core/graph/ImGuiCore.h"

#include "sys/types.h"
#include "sys/sysinfo.h"

namespace engine {

    enum ProfilerState {
        INPUT,
        RENDERING,
        UPDATE,
        FIXED_UPDATE,
        IMGUI,
        BOTTLE_NECK
    };

    struct State {
        static std::unordered_map<ProfilerState, std::string> stateToNameDict;
        std::chrono::system_clock::time_point init;
        std::chrono::system_clock::time_point end;
        ProfilerState state;
        bool active = false;

        [[nodiscard]] std::string toString() const {
            auto _diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - init);
            std::string _toString = State::stateToNameDict[state] + ": " + std::to_string(_diff.count());
            return _toString;
        }
    };

    inline std::ostream& operator<<(std::ostream & _ostream, const State& _state) {
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
        float Span;
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
            static void beginFrame(float _dt);
            static void begin(ProfilerState _state);
            static void end(ProfilerState _state);
            static void endFrame();
            static std::unordered_map<ProfilerState, State>& getStates();

            static u_long* getTotalVirtualMemory();
    };

}

#endif //ENGINE2_0_PROFILER_H
