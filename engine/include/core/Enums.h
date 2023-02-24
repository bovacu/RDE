#ifndef RDE_ENUMS_H
#define RDE_ENUMS_H


namespace RDE {

	/**
	 * @brief Status of the mouse entering and exiting.
	*/
	enum RDE_MOUSE_STATUS_ {
		RDE_MOUSE_STATUS_ENTERED,
		RDE_MOUSE_STATUS_EXITED
	};

	/**
	 * @brief Type of rendering image for UI sprites.
	 */
	enum RDE_IMAGE_RENDERING_TYPE_ {
		RDE_IMAGE_RENDERING_TYPE_NORMAL              = 0,
		RDE_IMAGE_RENDERING_TYPE_NINE_SLICE          = 1,
		RDE_IMAGE_RENDERING_TYPE_PARTIAL_VERTICAL    = 2,
		RDE_IMAGE_RENDERING_TYPE_PARTIAL_HORIZONTAL  = 3,
		RDE_IMAGE_RENDERING_TYPE_PARTIAL_RADIAL      = 4
	};

	/**
	 * @brief Anchor types for UI nodes fixing.
	 */
	enum RDE_UI_ANCHOR_ {
		RDE_UI_ANCHOR_MIDDLE              = 1 << 1,
		RDE_UI_ANCHOR_LEFT                = 1 << 2,
		RDE_UI_ANCHOR_RIGHT               = 1 << 3,
		RDE_UI_ANCHOR_TOP                 = 1 << 4,
		RDE_UI_ANCHOR_BOTTOM              = 1 << 5,
		RDE_UI_ANCHOR_LEFT_BOTTOM         = 1 << 6,
		RDE_UI_ANCHOR_LEFT_TOP            = 1 << 7,
		RDE_UI_ANCHOR_RIGHT_BOTTOM        = 1 << 8,
		RDE_UI_ANCHOR_RIGHT_TOP           = 1 << 9
	};

	/**
	 * @brief Stretch types for UI nodes resizing.
	*/
	enum RDE_UI_STRETCH_ {
		RDE_UI_STRETCH_NO_STRETCH          = 1 << 10,
		RDE_UI_STRETCH_VERTICAL_STRETCH    = 1 << 11,
		RDE_UI_STRETCH_HORIZONTAL_STRETCH  = 1 << 12,
		RDE_UI_STRETCH_FULL_STRETCH        = 1 << 13
	};

	/**
	 * @brief Available plaftorms thar RDE runs on.
	 */
	enum RDE_PLATFORM_TYPE_ {
		RDE_PLATFORM_TYPE_LINUX,
		RDE_PLATFORM_TYPE_WINDOWS,
		RDE_PLATFORM_TYPE_MAC,
		RDE_PLATFORM_TYPE_ANDROID,
		RDE_PLATFORM_TYPE_IOS,
		RDE_PLATFORM_TYPE_UNSUPPORTED
	};

	/**
	 * @brief This is the order of rendering of different Renderizable elements.
	*/
	enum RDE_BATCH_PRIORITY_ {
		RDE_BATCH_PRIORITY_SPRITE = 0,
		RDE_BATCH_PRIORITY_TEXT = 1
	};

	/**
	 * @brief Types of renderizable nodes.
	 */
	enum RDE_RENDERIZABLE_TYPE_ {
		RDE_RENDERIZABLE_TYPE_NONE        = 0,
		RDE_RENDERIZABLE_TYPE_SPRITE      = 1,
		RDE_RENDERIZABLE_TYPE_TEXT        = 2,
		RDE_RENDERIZABLE_TYPE_UI_IMAGE    = 3,
		RDE_RENDERIZABLE_TYPE_UI_TEXT     = 4
	};

	/**
		* @brief Uniforms that use a GLfloat as a data value.
		*/
	enum RDE_UNIFORM_FV_ {
		RDE_UNIFORM_FV_1,
		RDE_UNIFORM_FV_2,
		RDE_UNIFORM_FV_3,
		RDE_UNIFORM_FV_4,
		RDE_UNIFORM_FV_MATRIX_2,
		RDE_UNIFORM_FV_MATRIX_3,
		RDE_UNIFORM_FV_MATRIX_4,
		RDE_UNIFORM_FV_MATRIX_2x3,
		RDE_UNIFORM_FV_MATRIX_3x2,
		RDE_UNIFORM_FV_MATRIX_4x2,
		RDE_UNIFORM_FV_MATRIX_2x4,
		RDE_UNIFORM_FV_MATRIX_4x3,
		RDE_UNIFORM_FV_MATRIX_3x4
	};

	/**
	 * @brief Uniforms that use a GLint as a data value.
	*/
	enum RDE_UNIFORM_IV_ {
		RDE_UNIFORM_IV_1,
		RDE_UNIFORM_IV_2,
		RDE_UNIFORM_IV_3,
		RDE_UNIFORM_IV_4
	};

	/**
	 * @brief Uniforms that use a GLuint as a data value.
	*/
	enum RDE_UNIFORM_UIV_ {
		RDE_UNIFORM_UIV_1,
		RDE_UNIFORM_UIV_2,
		RDE_UNIFORM_UIV_3,
		RDE_UNIFORM_UIV_4
	};

	/**
	 * @brief Supported image formats.
	*/
	enum RDE_IMAGE_EXTENSION_ {
		RDE_IMAGE_EXTENSION_PNG,
		RDE_IMAGE_EXTENSION_JPG,
		RDE_IMAGE_EXTENSION_BMP
	};

	/**
	 * @brief All the different types of events that the engine can manage.
	*/
	enum RDE_EVENT_TYPE_ : unsigned {
		RDE_EVENT_TYPE_NONE, RDE_EVENT_TYPE_WINDOW_CLOSED, RDE_EVENT_TYPE_WINDOW_RESIZED, RDE_EVENT_TYPE_WINDOW_MOVED, RDE_EVENT_TYPE_WINDOW_FOCUSED, RDE_EVENT_TYPE_WINDOW_LOST_FOCUS, 
		
		RDE_EVENT_TYPE_GAME_FRAME, RDE_EVENT_TYPE_GAME_UPDATE, RDE_EVENT_TYPE_GAME_RENDER,
		
		RDE_EVENT_TYPE_KEY_PRESSED, RDE_EVENT_TYPE_KEY_DOWN, RDE_EVENT_TYPE_KEY_RELEASED, RDE_EVENT_TYPE_KEY_TYPED, RDE_EVENT_TYPE_TEXT_TYPED, 
		
		RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED, RDE_EVENT_TYPE_MOUSE_BUTTON_DOWN, RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED, RDE_EVENT_TYPE_MOUSE_SCROLLED, RDE_EVENT_TYPE_MOUSE_MOVED,
		
		RDE_EVENT_TYPE_WINDOW_MINIMIZED, RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED,

		RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED, RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN, RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP,

		RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN, RDE_EVENT_TYPE_MOBILE_TOUCH_UP, RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED
	};

	/**
	 * @brief Masking the different categories of the events. This makes really easy to get which event type is each event.
	 * Getting it is as easy as making (EventCategoryXXX & customEventCategoryPassed) == EventCategoryXXX.
	 *
	 * This is, for example, customEventCategoryPassed = 00000110, that means, an event which is both EventCategoryInput
	 * and EventCategoryKeyboard. doing 00000110 & 00000100 and later 00000110 & 00000010 gives as that the current
	 * is both types.
	*/
	enum RDE_EVENT_CATEGORY_ : unsigned {
		RDE_EVENT_CATEGORY_NONE,
		RDE_EVENT_CATEGORY_GAME           		= 1u << 0u, /// 00000001
		RDE_EVENT_CATEGORY_INPUT          		= 1u << 1u, /// 00000010
		RDE_EVENT_CATEGORY_KEYBOARD       		= 1u << 2u, /// 00000100
		RDE_EVENT_CATEGORY_MOUSE          		= 1u << 3u, /// 00001000
		RDE_EVENT_CATEGORY_MOUSE_BUTTON    		= 1u << 4u, /// 00010000
		RDE_EVENT_CATEGORY_CONTROLLER_BUTTON    = 1u << 5u, /// 00100000
		RDE_EVENT_CATEGORY_MOBILE_INPUT    		= 1u << 6u  /// 00100000
	};

	/**
	 * @brief Result of a file operation.
	*/
	enum RDE_FILE_RESULT_ {
		RDE_FILE_RESULT_OK,
		RDE_FILE_RESULT_NOT_FOUND,
		RDE_FILE_RESULT_LINE_OUT_OF_SCOPE,
		RDE_FILE_RESULT_NOT_ALL_LINES_IN_SCOPE
	};

	/**
	 * @brief Different types of file operations.
	*/
	enum RDE_FILE_MODE_ {
		RDE_FILE_MODE_READ,
		RDE_FILE_MODE_WRITE,
		RDE_FILE_MODE_APPEND,
		RDE_FILE_MODE_READ_AND_WRITE,
		RDE_FILE_MODE_READ_AND_APPEND,
	};

	/**
	 * @brief Different type of inputs depending on the platform.
	*/
	enum RDE_INPUT_TYPE_ {
		RDE_INPUT_TYPE_WINDOW,
		RDE_INPUT_TYPE_MOUSE,
		RDE_INPUT_TYPE_KEYBOARD,
		RDE_INPUT_TYPE_CONTROLLER,
		RDE_INPUT_TYPE_MOBILE
	};

	/**
	 * @brief Available languages by default of the engine.
	*/
	enum RDE_LANGUAGE_ { 
		RDE_LANGUAGE_EN_US, 
		RDE_LANGUAGE_EN_GB, 
		RDE_LANGUAGE_EN_CA, 
		RDE_LANGUAGE_ES_MX, 
		RDE_LANGUAGE_ES_ES, 
		RDE_LANGUAGE_PT_BR, 
		RDE_LANGUAGE_FR, 
		RDE_LANGUAGE_ZH, 
		RDE_LANGUAGE_RU, 
		RDE_LANGUAGE_DE, 
		RDE_LANGUAGE_IT, 
		RDE_LANGUAGE_JP, 
		RDE_LANGUAGE_MAX 
	};
	
	/**
	 * @brief Types of physics body properties.
	 */
	enum RDE_PHYSICS_BODY_TYPE_ {
		RDE_PHYSICS_BODY_TYPE_STATIC,
		RDE_PHYSICS_BODY_TYPE_KINEMATIC,
		RDE_PHYSICS_BODY_TYPE_DYNAMIC
	};

	/**
	 * @brief Types of physics shape properties.
	 */
	enum RDE_PHYSICS_SHAPE_TYPE_ {
		RDE_PHYSICS_SHAPE_TYPE_BOX,
		RDE_PHYSICS_SHAPE_TYPE_CIRCLE,
		RDE_PHYSICS_SHAPE_TYPE_POLYGON,
		RDE_PHYSICS_SHAPE_TYPE_SEGMENT
	};

	/**
	 * @brief Type of UI nodes that need to be updated.
	 */
	enum RDE_UI_UPDATABLE_NODE_ {
		RDE_UI_UPDATABLE_NODE_NONE     = 0,
		RDE_UI_UPDATABLE_NODE_INPUT    = 1,
		RDE_UI_UPDATABLE_NODE_SLIDER   = 2
	};

	/**
	 * @brief The status of the battery.
	*/
	enum RDE_BATTERY_ {
		RDE_BATTERY_UKNOWN,
		RDE_BATTERY_ON_BATTERY,
		RDE_BATTERY_NO_BATTERY,
		RDE_BATTERY_CHARGING,
		RDE_BATTERY_CHARGED
	};
}


#endif // RDE_ENUMS_H