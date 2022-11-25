// Created by borja on 30/3/22.


#ifndef RDE_H
#define RDE_H

#include "core/Engine.h"
#include "core/Core.h"
#include "core/util/Util.h"

#include "core/render/RenderManager.h"
#include "core/render/elements/TextureAtlasManager.h"
#include "core/render/elements/ShaderManager.h"
#include "core/systems/inputSystem/input/Input.h"

#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/systems/fileSystem/FileManager.h"
#include "core/systems/console/Console.h"
#include "core/systems/eventSystem/EventBus.h"
#include "core/systems/soundSystem/SoundManager.h"

#include "core/procedural/CellularAutomataMapGenerator.h"

#include "core/systems/ecsSystem/ECSManager.h"

#include "core/systems/uiSystem/Canvas.h"

#include "core/graph/components/Transform.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIPanel.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UIMask.h"

#include "core/systems/localization/LocalizationManager.h"

#endif //RDE_H
