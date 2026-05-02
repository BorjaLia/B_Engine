#pragma once

namespace Engine
{
    /// Represents physical keyboard keys.
    /// @ingroup Inputs
    enum class Key
    {
        Unknown = 0,

        // Letters
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Numbers (Top row)
        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

        // Function Keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Directionals
        Up, Down, Left, Right,

        // Specials & Modifiers
        Space, Escape, Enter, Tab, Backspace, Insert, Delete,
        PageUp, PageDown, Home, End,
        CapsLock, ScrollLock, NumLock, PrintScreen, Pause,
        LeftShift, LeftControl, LeftAlt, LeftSuper,  // Super = Windows / Cmd Key
        RightShift, RightControl, RightAlt, RightSuper,

        // Numpad
        Kp0, Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8, Kp9,
        KpDecimal, KpDivide, KpMultiply, KpSubtract, KpAdd, KpEnter, KpEqual,

        // Symbols
        Apostrophe, Comma, Minus, Period, Slash, Semicolon, Equal, LeftBracket,
        Backslash, RightBracket, GraveAccent,

        Count // Useful to know the total number of mapped keys
    };

    /// Represents physical mouse buttons.
    enum class MouseButton
    {
        Left = 0,
        Right,
        Middle,
        SideForward, // Extra lateral mouse buttons
        SideBack
    };

    /// Converts a Key enum to its user-friendly string representation.
    const char* KeyToString(Key key);

    /// Converts a MouseButton enum to its user-friendly string representation.
    const char* MouseButtonToString(MouseButton button);
}