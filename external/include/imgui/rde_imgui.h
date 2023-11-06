#ifndef RDE_FUNC
#if _WIN32
	#define RDE_FUNC __declspec(dllexport)
	#define RDE_FUNC_STATIC __declspec(dllexport) static
	#define RDE_FUNC_EXTERNAL extern "C" RDE_FUNC
#else
	#define RDE_FUNC 
	#define RDE_FUNC_STATIC static
	#define RDE_FUNC_EXTERNAL
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y;
} rde_ImVec2;

typedef int rde_ImGuiWindowFlags;       // -> enum ImGuiWindowFlags_     // Flags: for Begin(), BeginChild()
typedef int rde_ImGuiDockNodeFlags;     // -> enum ImGuiDockNodeFlags_   // Flags: for DockSpace()
typedef int rde_ImGuiSliderFlags;     // -> enum ImGuiSliderFlags_ 
typedef unsigned int rde_ImGuiID;// A unique ID used by widgets (typically the result of hashing a stack of string)
#ifndef rde_ImTextureID
typedef void* rde_ImTextureID;
#endif

typedef enum
{
    rde_ImGuiWindowFlags_None                   = 0,
    rde_ImGuiWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
    rde_ImGuiWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
    rde_ImGuiWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
    rde_ImGuiWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    rde_ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    rde_ImGuiWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
    rde_ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
    rde_ImGuiWindowFlags_NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    rde_ImGuiWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
    rde_ImGuiWindowFlags_NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    rde_ImGuiWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
    rde_ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    rde_ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    rde_ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    rde_ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    rde_ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    rde_ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    rde_ImGuiWindowFlags_NoNavInputs            = 1 << 18,  // No gamepad/keyboard navigation within the window
    rde_ImGuiWindowFlags_NoNavFocus             = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    rde_ImGuiWindowFlags_UnsavedDocument        = 1 << 20,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    rde_ImGuiWindowFlags_NoDocking              = 1 << 21,  // Disable docking of this window

    rde_ImGuiWindowFlags_NoNav                  = rde_ImGuiWindowFlags_NoNavInputs | rde_ImGuiWindowFlags_NoNavFocus,
    rde_ImGuiWindowFlags_NoDecoration           = rde_ImGuiWindowFlags_NoTitleBar | rde_ImGuiWindowFlags_NoResize | rde_ImGuiWindowFlags_NoScrollbar | rde_ImGuiWindowFlags_NoCollapse,
    rde_ImGuiWindowFlags_NoInputs               = rde_ImGuiWindowFlags_NoMouseInputs | rde_ImGuiWindowFlags_NoNavInputs | rde_ImGuiWindowFlags_NoNavFocus,

    // [Internal]
    rde_ImGuiWindowFlags_NavFlattened           = 1 << 23,  // [BETA] On child window: allow gamepad/keyboard navigation to cross over parent border to this child or between sibling child windows.
    rde_ImGuiWindowFlags_ChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
    rde_ImGuiWindowFlags_Tooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    rde_ImGuiWindowFlags_Popup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
    rde_ImGuiWindowFlags_Modal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    rde_ImGuiWindowFlags_ChildMenu              = 1 << 28,  // Don't use! For internal use by BeginMenu()
    rde_ImGuiWindowFlags_DockNodeHost           = 1 << 29,  // Don't use! For internal use by Begin()/NewFrame()
} rde_ImGuiWindowFlags_;

typedef enum
{
    rde_ImGuiDockNodeFlags_None                         = 0,
    rde_ImGuiDockNodeFlags_KeepAliveOnly                = 1 << 0,   //       // Don't display the dockspace node but keep it alive. Windows docked into this dockspace node won't be undocked.
    rde_ImGuiDockNodeFlags_NoDockingOverCentralNode     = 1 << 2,   //       // Disable docking over the Central Node, which will be always kept empty.
    rde_ImGuiDockNodeFlags_PassthruCentralNode          = 1 << 3,   //       // Enable passthru dockspace: 1) DockSpace() will render a ImGuiCol_WindowBg background covering everything excepted the Central Node when empty. Meaning the host window should probably use SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When Central Node is empty: let inputs pass-through + won't display a DockingEmptyBg background. See demo for details.
    rde_ImGuiDockNodeFlags_NoDockingSplit               = 1 << 4,   //       // Disable other windows/nodes from splitting this node.
    rde_ImGuiDockNodeFlags_NoResize                     = 1 << 5,   // Saved // Disable resizing node using the splitter/separators. Useful with programmatically setup dockspaces.
    rde_ImGuiDockNodeFlags_AutoHideTabBar               = 1 << 6,   //       // Tab bar will automatically hide when there is a single window in the dock node.
    rde_ImGuiDockNodeFlags_NoUndocking                  = 1 << 7,   //       // Disable undocking this node.
} rde_ImGuiDockNodeFlags_;

typedef enum
{
    rde_ImGuiSliderFlags_None                   = 0,
    rde_ImGuiSliderFlags_AlwaysClamp            = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
    rde_ImGuiSliderFlags_Logarithmic            = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
    rde_ImGuiSliderFlags_NoRoundToFormat        = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
    rde_ImGuiSliderFlags_NoInput                = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget
    rde_ImGuiSliderFlags_InvalidMask_           = 0x7000000F,   // [Internal] We treat using those bits as being potentially a 'float power' argument from the previous API that has got miscast to this enum, and will trigger an assert if needed.
} rde_ImGuiSliderFlags_;

RDE_FUNC void rde_imgui_init(void* _sdl_window, void* _opengl_context);

RDE_FUNC void rde_imgui_new_frame();
RDE_FUNC void rde_imgui_draw();
RDE_FUNC void rde_imgui_handle_events(void* _sdl_event);

RDE_FUNC rde_ImGuiID rde_imgui_get_id(const char* _str_id);
RDE_FUNC void rde_imgui_push_id(int _id);
RDE_FUNC void rde_imgui_pop_id();
RDE_FUNC void rde_imgui_dockspace(rde_ImGuiID _id, rde_ImVec2 _size, rde_ImGuiDockNodeFlags _flags);

RDE_FUNC bool rde_imgui_begin(const char* _name, bool* _open, rde_ImGuiWindowFlags _flags);
RDE_FUNC void rde_imgui_end();

RDE_FUNC void rde_imgui_new_line();
RDE_FUNC void rde_imgui_separator();

RDE_FUNC void rde_imgui_text(const char* _fmt, ...);
RDE_FUNC bool rde_imgui_radio_button(const char* _label, int* _v, int _v_button);

RDE_FUNC bool rde_imgui_drag_float(const char* _label, float* _v, float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_float_2(const char* _label, float _v[2], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_float_3(const char* _label, float _v[3], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_float_4(const char* _label, float _v[4], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags);

RDE_FUNC bool rde_imgui_drag_int(const char* _label, int* _v, float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_int_2(const char* _label, int _v[2], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_int_3(const char* _label, int _v[3], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags);
RDE_FUNC bool rde_imgui_drag_int_4(const char* _label, int _v[4], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags);

RDE_FUNC bool rde_imgui_slider_angle(const char* _label, float* _v_rad, float _v_degrees_min, float _v_degrees_max, rde_ImGuiSliderFlags _flags);

RDE_FUNC bool rde_imgui_checkbox(const char* _label, bool* _v);

RDE_FUNC bool rde_imgui_button_default(const char* _label);
RDE_FUNC bool rde_imgui_button(const char* _label, rde_ImVec2 _size);

RDE_FUNC void rde_imgui_progress_bar(float _progess);

RDE_FUNC void rde_imgui_shutdown();

#ifdef __cplusplus
}
#endif