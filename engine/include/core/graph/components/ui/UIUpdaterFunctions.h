#include "core/util/Delta.h"
#include "core/Core.h"

namespace RDE {

    class UIInput;
    class UISlider;

	RDE_FUNC void onUpdateUIInput(UIInput* _input, Delta _Dt);
	RDE_FUNC void onUpdateUISlider(UISlider* _slider, Delta _Dt);
}