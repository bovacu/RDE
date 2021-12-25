#pragma once

#ifndef KEYBOARD_KEYS_H
#define KEYBOARD_KEYS_H

#include <cstdint>
#include <ostream>

namespace engine {

    /// -----------------------------------------------------------------
    /// |			CODES FOR KEY BUTTONS, THEY ARE						|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------
    typedef enum class KeyCode : uint16_t {
        // From glfw3.h
        Space = 32,
        Apostrophe = 39,            /* ' */
        Comma = 44,                 /* , */
        Minus = 45,                 /* - */
        Period = 46,                /* . */
        Slash = 47,                 /* / */

        D0 = 48,                    /* 0 */
        D1 = 49,                    /* 1 */
        D2 = 50,                    /* 2 */
        D3 = 51,                    /* 3 */
        D4 = 52,                    /* 4 */
        D5 = 53,                    /* 5 */
        D6 = 54,                    /* 6 */
        D7 = 55,                    /* 7 */
        D8 = 56,                    /* 8 */
        D9 = 57,                    /* 9 */

        Semicolon = 59,             /* ; */
        Equal = 61,                 /* = */

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        LeftBracket = 91,               /* [ */
        Backslash = 92,                 /* \ */
        RightBracket = 93,              /* ] */
        GraveAccent = 96,               /* ` */

        World1 = 161,                   /* non-US #1 */
        World2 = 162,                   /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,

        /* Keypad */
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,

        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
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
