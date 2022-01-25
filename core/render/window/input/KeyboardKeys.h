#pragma once

#ifndef KEYBOARD_KEYS_H
#define KEYBOARD_KEYS_H

#include <cstdint>
#include <ostream>
#include <SDL2/SDL_scancode.h>

namespace engine {

    /// -----------------------------------------------------------------
    /// |			CODES FOR KEY BUTTONS, THEY ARE						|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------
    typedef enum class KeyCode : uint16_t {
        // From SDL2
        Space = SDL_SCANCODE_SPACE,
        Apostrophe = SDL_SCANCODE_APOSTROPHE,            /* ' */
        Comma = SDL_SCANCODE_COMMA,                 /* , */
        Minus = SDL_SCANCODE_MINUS,                 /* - */
        Period = SDL_SCANCODE_PERIOD,                /* . */
        Slash = SDL_SCANCODE_SLASH,                 /* / */

        D0 = SDL_SCANCODE_0,                    /* 0 */
        D1 = SDL_SCANCODE_1,                    /* 1 */
        D2 = SDL_SCANCODE_2,                    /* 2 */
        D3 = SDL_SCANCODE_3,                    /* 3 */
        D4 = SDL_SCANCODE_4,                    /* 4 */
        D5 = SDL_SCANCODE_5,                    /* 5 */
        D6 = SDL_SCANCODE_6,                    /* 6 */
        D7 = SDL_SCANCODE_7,                    /* 7 */
        D8 = SDL_SCANCODE_8,                    /* 8 */
        D9 = SDL_SCANCODE_9,                    /* 9 */

        Semicolon = SDL_SCANCODE_SEMICOLON,             /* ; */
        Equal = SDL_SCANCODE_EQUALS,                 /* = */

        A = SDL_SCANCODE_A,
        B = SDL_SCANCODE_B,
        C = SDL_SCANCODE_C,
        D = SDL_SCANCODE_D,
        E = SDL_SCANCODE_E,
        F = SDL_SCANCODE_F,
        G = SDL_SCANCODE_G,
        H = SDL_SCANCODE_H,
        I = SDL_SCANCODE_I,
        J = SDL_SCANCODE_J,
        K = SDL_SCANCODE_K,
        L = SDL_SCANCODE_L,
        M = SDL_SCANCODE_M,
        N = SDL_SCANCODE_N,
        O = SDL_SCANCODE_O,
        P = SDL_SCANCODE_P,
        Q = SDL_SCANCODE_Q,
        R = SDL_SCANCODE_R,
        S = SDL_SCANCODE_S,
        T = SDL_SCANCODE_T,
        U = SDL_SCANCODE_U,
        V = SDL_SCANCODE_V,
        W = SDL_SCANCODE_W,
        X = SDL_SCANCODE_X,
        Y = SDL_SCANCODE_Y,
        Z = SDL_SCANCODE_Z,

        LeftBracket = SDL_SCANCODE_LEFTBRACKET,               /* [ */
        Backslash = SDL_SCANCODE_BACKSLASH,                 /* \ */
        RightBracket = SDL_SCANCODE_RIGHTBRACKET,              /* ] */
        GraveAccent = SDL_SCANCODE_GRAVE,               /* ` */

        Escape = SDL_SCANCODE_ESCAPE,
        Enter = SDL_SCANCODE_KP_ENTER,
        Tab = SDL_SCANCODE_TAB,
        Backspace = SDL_SCANCODE_BACKSPACE,
        Insert = SDL_SCANCODE_INSERT,
        Delete = SDL_SCANCODE_DELETE,
        Right = SDL_SCANCODE_RIGHT,
        Left = SDL_SCANCODE_LEFT,
        Down = SDL_SCANCODE_DOWN,
        Up = SDL_SCANCODE_UP,
        PageUp = SDL_SCANCODE_PAGEUP,
        PageDown = SDL_SCANCODE_PAGEDOWN,
        Home = SDL_SCANCODE_HOME,
        End = SDL_SCANCODE_END,
        CapsLock = SDL_SCANCODE_CAPSLOCK,
        ScrollLock = SDL_SCANCODE_SCROLLLOCK,
        NumLock = SDL_SCANCODE_NUMLOCKCLEAR,
        PrintScreen = SDL_SCANCODE_PRINTSCREEN,
        Pause = SDL_SCANCODE_PAUSE,
        F1 = SDL_SCANCODE_F1,
        F2 = SDL_SCANCODE_F2,
        F3 = SDL_SCANCODE_F3,
        F4 = SDL_SCANCODE_F4,
        F5 = SDL_SCANCODE_F5,
        F6 = SDL_SCANCODE_F6,
        F7 = SDL_SCANCODE_F7,
        F8 = SDL_SCANCODE_F8,
        F9 = SDL_SCANCODE_F9,
        F10 = SDL_SCANCODE_F10,
        F11 = SDL_SCANCODE_F11,
        F12 = SDL_SCANCODE_F12,
        F13 = SDL_SCANCODE_F13,
        F14 = SDL_SCANCODE_F14,
        F15 = SDL_SCANCODE_F15,
        F16 = SDL_SCANCODE_F16,
        F17 = SDL_SCANCODE_F17,
        F18 = SDL_SCANCODE_F18,
        F19 = SDL_SCANCODE_F19,
        F20 = SDL_SCANCODE_F20,
        F21 = SDL_SCANCODE_F21,
        F22 = SDL_SCANCODE_F22,
        F23 = SDL_SCANCODE_F23,
        F24 = SDL_SCANCODE_F24,

        /* Keypad */
        KP0 = SDL_SCANCODE_KP_0,
        KP1 = SDL_SCANCODE_KP_1,
        KP2 = SDL_SCANCODE_KP_2,
        KP3 = SDL_SCANCODE_KP_3,
        KP4 = SDL_SCANCODE_KP_4,
        KP5 = SDL_SCANCODE_KP_5,
        KP6 = SDL_SCANCODE_KP_6,
        KP7 = SDL_SCANCODE_KP_7,
        KP8 = SDL_SCANCODE_KP_8,
        KP9 = SDL_SCANCODE_KP_9,

//        KPDecimal = 330,
//        KPDivide = 331,
//        KPMultiply = 332,
//        KPSubtract = 333,
//        KPAdd = 334,
//        KPEnter = 335,
//        KPEqual = 336,

        LeftShift = SDL_SCANCODE_LSHIFT,
        LeftControl = SDL_SCANCODE_LCTRL,
        LeftAlt = SDL_SCANCODE_LALT,
        RightShift = SDL_SCANCODE_RSHIFT,
        RightControl = SDL_SCANCODE_RCTRL,
        RightAlt = SDL_SCANCODE_RALT,
        Menu = SDL_SCANCODE_MENU
    } Key;

    inline std::ostream &operator<<(std::ostream &os, KeyCode keyCode) {
        os << static_cast<int32_t>(keyCode);
        return os;
    }
}

#define KEY_SPACE           ::engine::Key::Space
#define KEY_APOSTROPHE      ::engine::Key::Apostrophe    /* ' */
#define KEY_COMMA           ::engine::Key::Comma         /* , */
#define KEY_MINUS           ::engine::Key::Minus         /* - */
#define KEY_PERIOD          ::engine::Key::Period        /* . */
#define KEY_SLASH           ::engine::Key::Slash         /* / */
#define KEY_0               ::engine::Key::D0
#define KEY_1               ::engine::Key::D1
#define KEY_2               ::engine::Key::D2
#define KEY_3               ::engine::Key::D3
#define KEY_4               ::engine::Key::D4
#define KEY_5               ::engine::Key::D5
#define KEY_6               ::engine::Key::D6
#define KEY_7               ::engine::Key::D7
#define KEY_8               ::engine::Key::D8
#define KEY_9               ::engine::Key::D9
#define KEY_SEMICOLON       ::engine::Key::Semicolon     /* ; */
#define KEY_EQUAL           ::engine::Key::Equal         /* = */
#define KEY_A               ::engine::Key::A
#define KEY_B               ::engine::Key::B
#define KEY_C               ::engine::Key::C
#define KEY_D               ::engine::Key::D
#define KEY_E               ::engine::Key::E
#define KEY_F               ::engine::Key::F
#define KEY_G               ::engine::Key::G
#define KEY_H               ::engine::Key::H
#define KEY_I               ::engine::Key::I
#define KEY_J               ::engine::Key::J
#define KEY_K               ::engine::Key::K
#define KEY_L               ::engine::Key::L
#define KEY_M               ::engine::Key::M
#define KEY_N               ::engine::Key::N
#define KEY_O               ::engine::Key::O
#define KEY_P               ::engine::Key::P
#define KEY_Q               ::engine::Key::Q
#define KEY_R               ::engine::Key::R
#define KEY_S               ::engine::Key::S
#define KEY_T               ::engine::Key::T
#define KEY_U               ::engine::Key::U
#define KEY_V               ::engine::Key::V
#define KEY_W               ::engine::Key::W
#define KEY_X               ::engine::Key::X
#define KEY_Y               ::engine::Key::Y
#define KEY_Z               ::engine::Key::Z
#define KEY_LEFT_BRACKET    ::engine::Key::LeftBracket   /* [ */
#define KEY_BACKSLASH       ::engine::Key::Backslash     /* \ */
#define KEY_RIGHT_BRACKET   ::engine::Key::RightBracket  /* ] */
#define KEY_GRAVE_ACCENT    ::engine::Key::GraveAccent   /* ` */
#define KEY_WORLD_1         ::engine::Key::World1        /* non-US #1 */
#define KEY_WORLD_2         ::engine::Key::World2        /* non-US #2 */

/* Function keys */
#define KEY_ESCAPE          ::engine::Key::Escape
#define KEY_ENTER           ::engine::Key::Enter
#define KEY_TAB             ::engine::Key::Tab
#define KEY_BACKSPACE       ::engine::Key::Backspace
#define KEY_INSERT          ::engine::Key::Insert
#define KEY_DELETE          ::engine::Key::Delete
#define KEY_RIGHT           ::engine::Key::Right
#define KEY_LEFT            ::engine::Key::Left
#define KEY_DOWN            ::engine::Key::Down
#define KEY_UP              ::engine::Key::Up
#define KEY_PAGE_UP         ::engine::Key::PageUp
#define KEY_PAGE_DOWN       ::engine::Key::PageDown
#define KEY_HOME            ::engine::Key::Home
#define KEY_END             ::engine::Key::End
#define KEY_CAPS_LOCK       ::engine::Key::CapsLock
#define KEY_SCROLL_LOCK     ::engine::Key::ScrollLock
#define KEY_NUM_LOCK        ::engine::Key::NumLock
#define KEY_PRINT_SCREEN    ::engine::Key::PrintScreen
#define KEY_PAUSE           ::engine::Key::Pause
#define KEY_F1              ::engine::Key::F1
#define KEY_F2              ::engine::Key::F2
#define KEY_F3              ::engine::Key::F3
#define KEY_F4              ::engine::Key::F4
#define KEY_F5              ::engine::Key::F5
#define KEY_F6              ::engine::Key::F6
#define KEY_F7              ::engine::Key::F7
#define KEY_F8              ::engine::Key::F8
#define KEY_F9              ::engine::Key::F9
#define KEY_F10             ::engine::Key::F10
#define KEY_F11             ::engine::Key::F11
#define KEY_F12             ::engine::Key::F12
#define KEY_F13             ::engine::Key::F13
#define KEY_F14             ::engine::Key::F14
#define KEY_F15             ::engine::Key::F15
#define KEY_F16             ::engine::Key::F16
#define KEY_F17             ::engine::Key::F17
#define KEY_F18             ::engine::Key::F18
#define KEY_F19             ::engine::Key::F19
#define KEY_F20             ::engine::Key::F20
#define KEY_F21             ::engine::Key::F21
#define KEY_F22             ::engine::Key::F22
#define KEY_F23             ::engine::Key::F23
#define KEY_F24             ::engine::Key::F24
#define KEY_F25             ::engine::Key::F25

/* Keypad */
#define KEY_KP_0            ::engine::Key::KP0
#define KEY_KP_1            ::engine::Key::KP1
#define KEY_KP_2            ::engine::Key::KP2
#define KEY_KP_3            ::engine::Key::KP3
#define KEY_KP_4            ::engine::Key::KP4
#define KEY_KP_5            ::engine::Key::KP5
#define KEY_KP_6            ::engine::Key::KP6
#define KEY_KP_7            ::engine::Key::KP7
#define KEY_KP_8            ::engine::Key::KP8
#define KEY_KP_9            ::engine::Key::KP9
#define KEY_KP_DECIMAL      ::engine::Key::KPDecimal
#define KEY_KP_DIVIDE       ::engine::Key::KPDivide
#define KEY_KP_MULTIPLY     ::engine::Key::KPMultiply
#define KEY_KP_SUBTRACT     ::engine::Key::KPSubtract
#define KEY_KP_ADD          ::engine::Key::KPAdd
#define KEY_KP_ENTER        ::engine::Key::KPEnter
#define KEY_KP_EQUAL        ::engine::Key::KPEqual

#define KEY_LEFT_SHIFT      ::engine::Key::LeftShift
#define KEY_LEFT_CONTROL    ::engine::Key::LeftControl
#define KEY_LEFT_ALT        ::engine::Key::LeftAlt
#define KEY_LEFT_SUPER      ::engine::Key::LeftSuper
#define KEY_RIGHT_SHIFT     ::engine::Key::RightShift
#define KEY_RIGHT_CONTROL   ::engine::Key::RightControl
#define KEY_RIGHT_ALT       ::engine::Key::RightAlt
#define KEY_RIGHT_SUPER     ::engine::Key::RightSuper
#define KEY_MENU            ::engine::Key::Menu

#endif // KEYBOARD_KEYS_H
