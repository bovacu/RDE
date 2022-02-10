// Created by borja on 31/12/21.

#include "core/systems/profiling/Profiler.h"

namespace engine {

    std::unordered_map<ProfilerState, State> Profiler::states;
    std::unordered_map<ProfilerState, State> Profiler::lastStates;
    struct sysinfo Profiler::systemInfo;

    std::unordered_map<ProfilerState, std::string> State::stateToNameDict {
            {ProfilerState::UPDATE, "Update"},
            {ProfilerState::IMGUI, "ImGui"},
            {ProfilerState::RENDERING, "Rendering"},
            {ProfilerState::FIXED_UPDATE, "FixedUpdate"},
            {ProfilerState::INPUT, "Input"},
            {ProfilerState::BOTTLE_NECK, "BottleNeck"}
    };

    void Profiler::beginFrame(float _dt) {
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

    int Profiler::parseLine(char* line){
        // This assumes that a digit will be found and the line ends in " Kb".
        int i = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[i-3] = '\0';
        i = atoi(p);
        return i;
    }

    int Profiler::getValue(){ //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmRSS:", 6) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }

    u_long* Profiler::getTotalVirtualMemory() {
        static u_long _data[2];

        sysinfo (&systemInfo);

        u_long totalPhysMem = systemInfo.totalram;
        totalPhysMem *= systemInfo.mem_unit;

        _data[0] = totalPhysMem;
        _data[1] = getValue();

        return _data;
    }


#if !IS_MOBILE()
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
#endif
}
