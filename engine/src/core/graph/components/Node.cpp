//
// Created by borja on 11/16/22.
//

#include "core/graph/components/Node.h"

namespace GDE {

    DisabledConfig DisabledConfig::DisabledConfigEvent       { .disabledForEvent        = true };
    DisabledConfig DisabledConfig::DisabledConfigUpdate      { .disabledForUpdate       = true };
    DisabledConfig DisabledConfig::DisabledConfigFixedUpdate { .disabledForFixedUpdate  = true };
    DisabledConfig DisabledConfig::DisabledConfigRender      { .disabledForRender       = true };
    DisabledConfig DisabledConfig::DisabledConfigLateUpdate  { .disabledForLateUpdate   = true };

    DisabledConfig DisabledConfig::EnabledConfigEvent        { .disabledForEvent        = false };
    DisabledConfig DisabledConfig::EnabledConfigUpdate       { .disabledForUpdate       = false };
    DisabledConfig DisabledConfig::EnabledConfigFixedUpdate  { .disabledForFixedUpdate  = false };
    DisabledConfig DisabledConfig::EnabledConfigRender       { .disabledForRender       = false };
    DisabledConfig DisabledConfig::EnabledConfigLateUpdate   { .disabledForLateUpdate   = false };

}