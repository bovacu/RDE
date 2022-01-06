// Created by borja on 31/12/21.

#include "Profiler.h"

namespace engine {

    std::unordered_map<ProfilerState, State> Profiler::states;
    std::unordered_map<ProfilerState, State> Profiler::lastStates;

    std::unordered_map<ProfilerState, std::string> State::stateToNameDict {
            {ProfilerState::UPDATE, "Update"},
            {ProfilerState::IMGUI, "ImGui"},
            {ProfilerState::RENDERING, "Rendering"},
            {ProfilerState::FIXED_UPDATE, "FixedUpdate"},
            {ProfilerState::INPUT, "Input"},
            {ProfilerState::BOTTLE_NECK, "BottleNeck"}
    };

    void Profiler::beginFrame() {
        states.clear();
    }

    void Profiler::begin(ProfilerState _state) {
        State _s;
        _s.init = std::chrono::system_clock::now();
        _s.state = _state;
        _s.active = true;
        states[_state] = _s;
    }

    void Profiler::end(ProfilerState _state) {
        states[_state].end = std::chrono::system_clock::now();
    }

    void Profiler::endFrame() {
        lastStates = states;
    }

    std::unordered_map<ProfilerState, State>& Profiler::getStates() {
        return lastStates;
    }


    ScrollingBuffer::ScrollingBuffer(int max_size) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }

    void ScrollingBuffer::AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }

    void ScrollingBuffer::Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }

    RollingBuffer::RollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }

    void RollingBuffer::AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
}
