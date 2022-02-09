#pragma once

#ifndef LAYER_H
#define LAYER_H

#include "core/render/window/event/Event.h"
#include "core/util/Delta.h"

namespace engine {

    class Layer {
        protected:
            std::string debugName;

        public:
            explicit Layer(const std::string& _debugName = "Layer")  : debugName(_debugName) {  }
            virtual ~Layer() {  };

            virtual void onInit()                       = 0;
            virtual void onEvent(Event& _event)         = 0;
            virtual void onUpdate(Delta _dt)         = 0;
            virtual void onFixedUpdate(Delta _dt)    {  }
            virtual void onRender(Delta _dt)         = 0;
            virtual void onImGuiRender(Delta _dt)    {  }
            virtual void onEnd()                        {  }

            [[nodiscard]] const std::string& getName() const { return debugName; }
    };

}

#endif //LAYER_H
