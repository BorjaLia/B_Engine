#include "RaylibInput.h"

#include <raylib.h>
#include "../../events/InputEvents.h"
#include "../../events/EventBus.h"

namespace Engine
{
    // Helper to map Engine keys to Raylib keys
    static int ToRaylibKey(Key key)
    {
        switch (key)
        {
            // Letters
        case Key::A: return KEY_A; case Key::B: return KEY_B; case Key::C: return KEY_C;
        case Key::D: return KEY_D; case Key::E: return KEY_E; case Key::F: return KEY_F;
        case Key::G: return KEY_G; case Key::H: return KEY_H; case Key::I: return KEY_I;
        case Key::J: return KEY_J; case Key::K: return KEY_K; case Key::L: return KEY_L;
        case Key::M: return KEY_M; case Key::N: return KEY_N; case Key::O: return KEY_O;
        case Key::P: return KEY_P; case Key::Q: return KEY_Q; case Key::R: return KEY_R;
        case Key::S: return KEY_S; case Key::T: return KEY_T; case Key::U: return KEY_U;
        case Key::V: return KEY_V; case Key::W: return KEY_W; case Key::X: return KEY_X;
        case Key::Y: return KEY_Y; case Key::Z: return KEY_Z;

            // Numbers
        case Key::Zero:  return KEY_ZERO;  case Key::One:   return KEY_ONE;
        case Key::Two:   return KEY_TWO;   case Key::Three: return KEY_THREE;
        case Key::Four:  return KEY_FOUR;  case Key::Five:  return KEY_FIVE;
        case Key::Six:   return KEY_SIX;   case Key::Seven: return KEY_SEVEN;
        case Key::Eight: return KEY_EIGHT; case Key::Nine:  return KEY_NINE;

            // Functions
        case Key::F1: return KEY_F1; case Key::F2: return KEY_F2;
        case Key::F3: return KEY_F3; case Key::F4: return KEY_F4;
        case Key::F5: return KEY_F5; case Key::F6: return KEY_F6;
        case Key::F7: return KEY_F7; case Key::F8: return KEY_F8;
        case Key::F9: return KEY_F9; case Key::F10: return KEY_F10;
        case Key::F11: return KEY_F11; case Key::F12: return KEY_F12;

            // Directionals
        case Key::Up:   return KEY_UP;   case Key::Down:  return KEY_DOWN;
        case Key::Left: return KEY_LEFT; case Key::Right: return KEY_RIGHT;

            // Specials & Modifiers
        case Key::Space:     return KEY_SPACE;     case Key::Escape:       return KEY_ESCAPE;
        case Key::Enter:     return KEY_ENTER;     case Key::Tab:          return KEY_TAB;
        case Key::Backspace: return KEY_BACKSPACE; case Key::Insert:       return KEY_INSERT;
        case Key::Delete:    return KEY_DELETE;    case Key::PrintScreen:  return KEY_PRINT_SCREEN;
        case Key::PageUp:    return KEY_PAGE_UP;   case Key::PageDown:     return KEY_PAGE_DOWN;
        case Key::Home:      return KEY_HOME;      case Key::End:          return KEY_END;
        case Key::CapsLock:  return KEY_CAPS_LOCK; case Key::ScrollLock:   return KEY_SCROLL_LOCK;
        case Key::NumLock:   return KEY_NUM_LOCK;  case Key::Pause:        return KEY_PAUSE;

        case Key::LeftShift:   return KEY_LEFT_SHIFT;   case Key::RightShift:   return KEY_RIGHT_SHIFT;
        case Key::LeftControl: return KEY_LEFT_CONTROL; case Key::RightControl: return KEY_RIGHT_CONTROL;
        case Key::LeftAlt:     return KEY_LEFT_ALT;     case Key::RightAlt:     return KEY_RIGHT_ALT;
        case Key::LeftSuper:   return KEY_LEFT_SUPER;   case Key::RightSuper:   return KEY_RIGHT_SUPER;

            // Numpad
        case Key::Kp0: return KEY_KP_0; case Key::Kp1: return KEY_KP_1; case Key::Kp2: return KEY_KP_2;
        case Key::Kp3: return KEY_KP_3; case Key::Kp4: return KEY_KP_4; case Key::Kp5: return KEY_KP_5;
        case Key::Kp6: return KEY_KP_6; case Key::Kp7: return KEY_KP_7; case Key::Kp8: return KEY_KP_8;
        case Key::Kp9: return KEY_KP_9;
        case Key::KpDecimal:  return KEY_KP_DECIMAL; case Key::KpDivide: return KEY_KP_DIVIDE;
        case Key::KpMultiply: return KEY_KP_MULTIPLY; case Key::KpSubtract: return KEY_KP_SUBTRACT;
        case Key::KpAdd:      return KEY_KP_ADD;      case Key::KpEnter:    return KEY_KP_ENTER;
        case Key::KpEqual:    return KEY_KP_EQUAL;

            // Symbols
        case Key::Apostrophe:   return KEY_APOSTROPHE;     case Key::Comma:        return KEY_COMMA;
        case Key::Minus:        return KEY_MINUS;          case Key::Period:       return KEY_PERIOD;
        case Key::Slash:        return KEY_SLASH;          case Key::Semicolon:    return KEY_SEMICOLON;
        case Key::Equal:        return KEY_EQUAL;          case Key::LeftBracket:  return KEY_LEFT_BRACKET;
        case Key::Backslash:    return KEY_BACKSLASH;      case Key::RightBracket: return KEY_RIGHT_BRACKET;
        case Key::GraveAccent:  return KEY_GRAVE;

        default: return 0; // Unknown
        }
    }

    // Helper to map Engine MouseButtons to Raylib ones
    static int ToRaylibMouse(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::Left:        return MOUSE_BUTTON_LEFT;
        case MouseButton::Right:       return MOUSE_BUTTON_RIGHT;
        case MouseButton::Middle:      return MOUSE_BUTTON_MIDDLE;
        case MouseButton::SideForward: return MOUSE_BUTTON_FORWARD;
        case MouseButton::SideBack:    return MOUSE_BUTTON_BACK;
        default: return -1;
        }
    }

    bool RaylibInput::IsKeyDown(Key key) const { return ::IsKeyDown(ToRaylibKey(key)); }
    bool RaylibInput::IsKeyPressed(Key key) const { return ::IsKeyPressed(ToRaylibKey(key)); }
    bool RaylibInput::IsKeyReleased(Key key) const { return ::IsKeyReleased(ToRaylibKey(key)); }

    bool RaylibInput::IsMouseButtonDown(MouseButton button) const
    {
        return ::IsMouseButtonDown(ToRaylibMouse(button));
    }

    bool RaylibInput::IsMouseButtonPressed(MouseButton button) const
    {
        return ::IsMouseButtonPressed(ToRaylibMouse(button));
    }

    bool RaylibInput::IsMouseButtonReleased(MouseButton button) const
    {
        return ::IsMouseButtonReleased(ToRaylibMouse(button));
    }

    Vector2f RaylibInput::GetMousePosition() const
    {
        // Invert Y-axis to match the engine's Cartesian coordinate system
        return Vector2f(::GetMousePosition().x, -::GetMousePosition().y);
    }

    std::optional<Vector2f> RaylibInput::GetMouseDelta() const
    {
        ::Vector2 delta = ::GetMouseDelta();
        if (delta.x == 0 && delta.y == 0) return std::nullopt;

        return Vector2f(delta.x, -delta.y);
    }

    float RaylibInput::GetMouseWheel()
    {
        return ::GetMouseWheelMove();
    }
}