// Created by borja on 31/12/21.

#include "core/systems/profiling/Profiler.h"

namespace GDE {

    std::unordered_map<ProfilerState, State> Profiler::states;
    std::unordered_map<ProfilerState, State> Profiler::lastStates;

#if IS_LINUX()
    struct sysinfo Profiler::systemInfo;
#endif

    std::unordered_map<ProfilerState, std::string> State::stateToNameDict {
            {ProfilerState::UPDATE, "Update"},
            {ProfilerState::IMGUI, "ImGui"},
            {ProfilerState::RENDERING, "Rendering"},
            {ProfilerState::FIXED_UPDATE, "FixedUpdate"},
            {ProfilerState::INPUT, "Input"},
            {ProfilerState::BOTTLE_NECK, "BottleNeck"}
    };

    void Profiler::beginFrame(float _dt) {
        #if DEBUG
        states.clear();
        #endif
    }

    void Profiler::begin(ProfilerState _state) {
        #if DEBUG
        State _s;
        _s.init = std::chrono::system_clock::now();
        _s.state = _state;
        _s.active = true;
        states[_state] = _s;
        #endif
    }

    void Profiler::end(ProfilerState _state) {
        #if DEBUG
        states[_state].end = std::chrono::system_clock::now();
        #endif
    }

    void Profiler::endFrame() {
        #if DEBUG
        lastStates = states;
        #endif
    }

    std::unordered_map<ProfilerState, State>& Profiler::getStates() {
        return lastStates;
    }

    int Profiler::parseLine(char* line){
        // This assumes that a digit will be found and the line ends in " Kb".
        int i = (int)strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[i-3] = '\0';
        i = atoi(p);
        return i;
    }

    int Profiler::getValue(){ //Note: this value is in KB!
        #if IS_WINDOWS()
        FILE* file;
        fopen_s(&file, "/proc/self/status", "r");
        #else
        FILE* file = fopen("/proc/self/status", "r");
        #endif

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
#if IS_LINUX()
        static u_long _data[2];

        sysinfo (&systemInfo);

        u_long totalPhysMem = systemInfo.totalram;
        totalPhysMem *= systemInfo.mem_unit;

        _data[0] = totalPhysMem;
        _data[1] = getValue();

        return _data;
#else
        return 0;
#endif
    }


//#if !IS_MOBILE()
//ScrollingBuffer::ScrollingBuffer(int max_size) {
//        MaxSize = max_size;
//        Offset  = 0;
//        Data.reserve(MaxSize);
//    }
//
//    void ScrollingBuffer::AddPoint(float x, float y) {
//        if (Data.size() < MaxSize)
//            Data.push_back(ImVec2(x,y));
//        else {
//            Data[Offset] = ImVec2(x,y);
//            Offset =  (Offset + 1) % MaxSize;
//        }
//    }
//
//    void ScrollingBuffer::Erase() {
//        if (Data.size() > 0) {
//            Data.shrink(0);
//            Offset  = 0;
//        }
//    }
//
//    RollingBuffer::RollingBuffer() {
//        Span = 10.0f;
//        Data.reserve(2000);
//    }
//
//    void RollingBuffer::AddPoint(float x, float y) {
//        float xmod = fmodf(x, Span);
//        if (!Data.empty() && xmod < Data.back().x)
//            Data.shrink(0);
//        Data.push_back(ImVec2(xmod, y));
//    }
//#endif
}
