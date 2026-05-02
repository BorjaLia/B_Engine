#include "KeyCodes.h"

namespace Engine
{
    const char* KeyToString(Key key)
    {
        switch (key)
        {
            // Letters
        case Key::A: return "A"; case Key::B: return "B"; case Key::C: return "C";
        case Key::D: return "D"; case Key::E: return "E"; case Key::F: return "F";
        case Key::G: return "G"; case Key::H: return "H"; case Key::I: return "I";
        case Key::J: return "J"; case Key::K: return "K"; case Key::L: return "L";
        case Key::M: return "M"; case Key::N: return "N"; case Key::O: return "O";
        case Key::P: return "P"; case Key::Q: return "Q"; case Key::R: return "R";
        case Key::S: return "S"; case Key::T: return "T"; case Key::U: return "U";
        case Key::V: return "V"; case Key::W: return "W"; case Key::X: return "X";
        case Key::Y: return "Y"; case Key::Z: return "Z";

            // Numbers
        case Key::Zero:  return "0"; case Key::One:   return "1";
        case Key::Two:   return "2"; case Key::Three: return "3";
        case Key::Four:  return "4"; case Key::Five:  return "5";
        case Key::Six:   return "6"; case Key::Seven: return "7";
        case Key::Eight: return "8"; case Key::Nine:  return "9";

            // Function Keys
        case Key::F1: return "F1"; case Key::F2: return "F2"; case Key::F3: return "F3";
        case Key::F4: return "F4"; case Key::F5: return "F5"; case Key::F6: return "F6";
        case Key::F7: return "F7"; case Key::F8: return "F8"; case Key::F9: return "F9";
        case Key::F10: return "F10"; case Key::F11: return "F11"; case Key::F12: return "F12";

            // Directionals
        case Key::Up:    return "UP";   case Key::Down:  return "DOWN";
        case Key::Left:  return "LEFT"; case Key::Right: return "RIGHT";

            // Specials & Modifiers
        case Key::Space:       return "SPACE";     case Key::Escape:      return "ESC";
        case Key::Enter:       return "ENTER";     case Key::Tab:         return "TAB";
        case Key::Backspace:   return "BACKSPACE"; case Key::Insert:      return "INSERT";
        case Key::Delete:      return "DELETE";    case Key::PrintScreen: return "PRT_SCR";
        case Key::PageUp:      return "PAGE_UP";   case Key::PageDown:    return "PAGE_DOWN";
        case Key::Home:        return "HOME";      case Key::End:         return "END";
        case Key::CapsLock:    return "CAPS_LOCK"; case Key::ScrollLock:  return "SCR_LOCK";
        case Key::NumLock:     return "NUM_LOCK";  case Key::Pause:       return "PAUSE";

        case Key::LeftShift:   return "L_SHIFT";   case Key::RightShift:   return "R_SHIFT";
        case Key::LeftControl: return "L_CTRL";    case Key::RightControl: return "R_CTRL";
        case Key::LeftAlt:     return "L_ALT";     case Key::RightAlt:     return "R_ALT";
        case Key::LeftSuper:   return "L_SUPER";   case Key::RightSuper:   return "R_SUPER";

            // Numpad
        case Key::Kp0: return "NUM_0"; case Key::Kp1: return "NUM_1"; case Key::Kp2: return "NUM_2";
        case Key::Kp3: return "NUM_3"; case Key::Kp4: return "NUM_4"; case Key::Kp5: return "NUM_5";
        case Key::Kp6: return "NUM_6"; case Key::Kp7: return "NUM_7"; case Key::Kp8: return "NUM_8";
        case Key::Kp9: return "NUM_9";
        case Key::KpDecimal:  return "NUM_DECIMAL"; case Key::KpDivide:   return "NUM_DIVIDE";
        case Key::KpMultiply: return "NUM_MULTIPLY";case Key::KpSubtract: return "NUM_SUBTRACT";
        case Key::KpAdd:      return "NUM_ADD";     case Key::KpEnter:    return "NUM_ENTER";
        case Key::KpEqual:    return "NUM_EQUAL";

            // Symbols
        case Key::Apostrophe:  return "'";  case Key::Comma:        return ",";
        case Key::Minus:       return "-";  case Key::Period:       return ".";
        case Key::Slash:       return "/";  case Key::Semicolon:    return ";";
        case Key::Equal:       return "=";  case Key::LeftBracket:  return "[";
        case Key::Backslash:   return "\\"; case Key::RightBracket: return "]";
        case Key::GraveAccent: return "`";

        case Key::Unknown:
        default: return "UNKNOWN";
        }
    }

    const char* MouseButtonToString(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::Left:        return "MOUSE_LEFT";
        case MouseButton::Right:       return "MOUSE_RIGHT";
        case MouseButton::Middle:      return "MOUSE_MIDDLE";
        case MouseButton::SideForward: return "MOUSE_FORWARD";
        case MouseButton::SideBack:    return "MOUSE_BACK";
        default:                       return "MOUSE_UNKNOWN";
        }
    }
}