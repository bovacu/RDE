// Created by borja on 30/3/22.


#ifndef RDE_H
#define RDE_H

/**
 * @brief This header file includes all necessary headers for the use of the engine
 * 
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&##BBGGPPPPPPPGGGBB#&&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&#BGP5YJ?7!!!~~~~~~~!!77?JY5PB##&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&##G5J?!~^^:::::::::^^^:::::::^^~7YPGGB#&&@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&#BGY?!^^:::^^^^^^^^^^^^^^^^^^^^^^:::^?5PP5PB#&@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&#G5?~^::^^^^^^^^^~?Y7^^^^^^^^^^^^^^^^::~?G5!~?5GB#&@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&BGY7^:^^^^^^^^^^^:7YP57:^^^^::::^^^^^^:::!YG5~^^!?YGB#&@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&#G57^:^^^::^^:::::::7P5?~::^:^!??7~^^:::^~7PGJ7~^^^^~7YGB#@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#GPJ^:^^::...::::::^^^!J5GJ!~^^!YPGGPP5YYY5PP5?!^^^^^^^^!?5B#&@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&#B5!^^^::....::^^^^^^^^^~7J5PP555PY7!7?JJJJJ7!~^^^^^^^^^^^~7YPB&@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@&##BB#&@@@&&@@@@@@@@&#GJ!^:^:.....::^^^^^^^^^^^^^~!7??7!~^^^^^^^^^^^^^^^^^^^::::^^~JPB#@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@&#G5?7YPB##BGB#&@@@@@&#GY~:^^^:..:::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::^^~!77??JJJYPGB####&&&&@@@@@
	@@@@@@@@@@@@@@@@&BG5?~:^75GGY?J5G#@@@@@#GJ!^^^^^^^^^^^^^^^^^^^^^::^^^^^^^^^^^^^^^^^^^^^::^!?Y5PPPPPPP5555PPPPPPPPPGB&@@@
	@@@@@@@@@@@@@@&#BPJ~:^^^~!77~^~?PB#@@@&BP7^^^^^^^^^^^^^^^^^^^~!!7?JYY5555555YYJ7!~~!!!7?JYPGP5J?!!~~~^^^~7JYPPPP55PG#@@@
	@@@@@@@@@@@@@&#B5!^^^^^^^^^^^^^!J5G#@&BG5!:^^^^^^^^^^^~~7?JY5PGGGPP5YYJJJJYY5PPGPPPPPPPPPPPJ!^:::^^^^^^^^!?555PPPPPG#@@@
	@@@@@@@@@@@@&BPJ!^^^^^^^^^^^^^^^~7YPBBGPJ~::^^^~~!!7JY5PPPGGP5?7!~^^^::^^::^^~7Y5PPP55PPGP7^::^!JYYJ7~^^^~75P5PPPPGB&@@@
	@@@@@@@@@@@&BGY~^^^^^^^^^^^^^^^^^^~!?5G5J7!7?JJY55PPGGP5PPPY!^::::^^^!?JJ?7~^^^~?PGP7~?PBP!::~7J5PPJ7!!^^^!YPPPPPPB&@@@@
	@@@@@@@@@@&#GJ!^:^^^^^^^^^^^^^^^^^~~!JPP55PPPPPPPPPPGGP5P5J!::^^^^:~7YPGPY?7!^^^75P57^~JGG7^^7Y5YY?^ !?~::~J5PGGGG#&@@@@
	@@@@@@@@@&#B5!:^^^^^^^^^^^^^^^^^^^^~7YPPPPPP555YYJJ?JJ5PG5?~^^^^^^^7YYYY?::??~::75P57^^!5GY7^!5PP5Y~:!?~^^!JYY5PPG#&@@@@
	@@@@@@@@@#GY7~:^^^^^^^^^^^^^^^^^^^^^~JP5Y7~~~~~^^^^^^^?5PPJ!~^^^^^^?5P5Y?:.7?~^^75P57::^?5GY~!YPPPP55J?~:^!Y5555PG#@@@@@
	@@@@@@@@&#P?~^^^^^^^^^^^^^^^^^^^^^^^~?PPY~::^^^^^^^^^^~75G5?~^^^^^^7JY5P5YJJ7^:^?PP5J7?Y5PGPJ7!JPGPPP?~^^^!J5PPPGB&@@@@@
	@@@@@@@&#G5!^^^^^^^^^^^^^^^^^^^^^^^^^7YP57^^^^^^^^^^^^:^JGG5!^^^^^^!?Y5YYP5?~^^!JGGGPP5555PGGPJ?7??7!^^~!7J5PPGG#@@@@@@@
	@@@@@@@&G5J~:^^^^^^^^^^^^^^^^^^^^^^^^~75G?~^^^^^^^^^^^^^~7PG5?~:^^^:^!?J?7~::^75GG5J?777777?Y5GGP5YYYY55PPGGGB#&@@@@@@@@
	@@@@@@@#GJ!^^^^^^^^^^^^^^^^^^^^^^^^^^^~YGY7~^^^^^^^^^^^^^^7YGG5J7~~^^^^~~~!7J5GG5?77!777777777?JY5PPGGGGGGB#&@@@@@@@@@@@
	@@@@@@&#G?::^^^^^^^^^^^^^^^^^^^^^^^^^^~7YP5!:^^^^^^^^^^^^^^^7J5PPP55YY55PPGGGPY?77777777777777!7777???JJYPB#@@@@@@@@@@@@
	@@@@@@#BP7:^^^^^^^^^^^^^^^^^^^^^^^^^^^~~!5GJ!^:^^^^^^^^^^^^^^^~!77?5GBGGP5YJ?777777777777777777777777!!!7YGB&@@@@@@@@@@@
	@@@@@&#GY7^!77!~^^^^^^^^^^^^^^^^^^^^^^^^~7YGP7^^^^^^^^^^^^^^^^^::~?5PY??77777777777!!!!!!!!!!!!!!!!!!77JYPB&@@@@@@@@@@@@
	@@@@@&BPJY5PPGP5J!~^^^^^^^^^^^^^^^^^^^^^^^!YPPJ~:^^^^^^^^^^^^^^::?G57!!!777777!!!!!7?JY5PPPPP5555YYY55PGB&&@@@@@@@@@@@@@
	@@@@@&BGPGP7^~75PPY!~^^^^^^^^^^^^^^^^^^^^^^~7PG5?^:^^^^^^^^^^^^::7PPY?7!!!!!!!7?JYPGPP5YJJJJJJYYY5PGB#&@@@@@@@@@@@@@@@@@
	@@@@@#GBGJ~:::::!YPPY!~^^^^^^^^^^^^^^^^^^^^^~!?PP5?~^^^^^^^^^^^^^^~?5PP555555PPPP5YJ?777777777777YPGGB&@@@@@@@@@@@@@@@@@
	@@@@@&BPY~.:::::::!YPPJ!^^^^^^^^^^^^^^^^^^^^^^^~?5P57::^^^^^^^^^^^^^^~!7?J5GG5J777!!!777777!!77J5P5?75B#&@@@@@@@@@@@@@@@
	@@@@@&GJ~:::::::::::75P5?~^^^^^^^^^^^^^^^^^^^^^^^~!7~^^^^^^^^^^^^^^^^^::::~?YPPYJ?77777777??J5PPY?~^^7YPB&@@@@@@@@@@@@@@
	@@@@&BP7::::^^^^^^:::^JG5J!^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^^^^^^^!?Y5PPP55555PP55Y?!^^^^^:~YGB&@@@@@@@@@@@@@
	@@@&BGY!:::^^^^^^^^^::^75GJ!^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^~!!7????77!~~^^^^^^^^^^^7P#&@@@@@@@@@@@@
	@@@&BPJ!::^^^^^^^^^^^^::75P57^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^JPB&@@@@@@@@@@@
	@@@&G5J~::^^^^^^^^^^^^^^^~5GY7~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^~?P#@@@@@@@@@@@
	@@@&B5J~::^^^^^^^^^^^^^^:^7YP5!^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:!5B#&@@@@@@@@@
	@@@&BPY!::^^^^^^^^^^^^^^^^^!PGJ!^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:~YGB&@@@@@@@@@
	@@@&#G57:::^^^^^^^^^^^^^^^^~?5PY~:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^?5B&@@@@@@@@@
	@@@@&#P?:::^^^^^^^^^^^^^^^^^~?GP7~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^7YB&@@@@@@@@@
	@@@@@&BJ~:::^^^^^^^^^^^^^^^^:~Y5?!^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::^7YB&@@@@@@@@@
	@@@@@&#PJ~.::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:.^?5B&@@@@@@@@@
	@@@@@@&#G?::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::.~YGB&@@@@@@@@@
	@@@@@@@&BPJ~:::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:::^!5B&@@@@@@@@@@
	@@@@@@@@&#P?~::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::7YG#@@@@@@@@@@@
	@@@@@@@@@&#G5!::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::!5B#&@@@@@@@@@@@
	@@@@@@@@@@@&BPJ~::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::^JPB#@@@@@@@@@@@@@
	@@@@@@@@@@@@&#GPJ~::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::^?5G#&@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@&#BPJ~^::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:::::~?PG#&@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@&#BG57~:::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::::^7YGB#&@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@&#BPY7~^:::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:::::::~7JPB##&@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@&&#BG5?7~^:::::::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^::::::::^~!?YPB#&&@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@&&#BG5J?!~^:::::::::::^^^^^^^^^^^^^^^^^^^^^^^::::::::::::^~!7JYPB#&&&@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&&#BG5YJ?7!~~^::::::::::::::::::::::::::::::^^~!7?JY5PGB#&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&##BGGPP55YYJJ???7777777777777???JYY55PPGGB##&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/


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
