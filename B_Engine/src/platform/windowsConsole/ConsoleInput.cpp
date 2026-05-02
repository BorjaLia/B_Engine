#include "ConsoleInput.h"

#include <vector>

namespace Engine
{
    // Helper to translate Engine Keys to Windows Virtual-Keys
    static int ToWin32Key(Key key)
    {
        switch (key)
        {
            // Letters and numbers in Win32 match their uppercase ASCII values
        case Key::A: return 'A'; case Key::B: return 'B'; case Key::C: return 'C';
        case Key::D: return 'D'; case Key::E: return 'E'; case Key::F: return 'F';
        case Key::G: return 'G'; case Key::H: return 'H'; case Key::I: return 'I';
        case Key::J: return 'J'; case Key::K: return 'K'; case Key::L: return 'L';
        case Key::M: return 'M'; case Key::N: return 'N'; case Key::O: return 'O';
        case Key::P: return 'P'; case Key::Q: return 'Q'; case Key::R: return 'R';
        case Key::S: return 'S'; case Key::T: return 'T'; case Key::U: return 'U';
        case Key::V: return 'V'; case Key::W: return 'W'; case Key::X: return 'X';
        case Key::Y: return 'Y'; case Key::Z: return 'Z';

        case Key::Zero: return '0';  case Key::One: return '1';
        case Key::Two: return '2';   case Key::Three: return '3';
        case Key::Four: return '4';  case Key::Five: return '5';
        case Key::Six: return '6';   case Key::Seven: return '7';
        case Key::Eight: return '8'; case Key::Nine: return '9';

            // Specials
        case Key::Space: return VK_SPACE;
        case Key::Escape: return VK_ESCAPE;
        case Key::Enter: return VK_RETURN;
        case Key::Up: return VK_UP;
        case Key::Down: return VK_DOWN;
        case Key::Left: return VK_LEFT;
        case Key::Right: return VK_RIGHT;
        case Key::LeftShift: return VK_LSHIFT;
        case Key::RightShift: return VK_RSHIFT;

            // Functions
        case Key::F3: return VK_F3;
        case Key::F4: return VK_F4;
        case Key::F5: return VK_F5;
        case Key::F6: return VK_F6;
        case Key::F7: return VK_F7;

        default: return 0;
        }
    }

    static int ToWin32Mouse(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::Left: return VK_LBUTTON;
        case MouseButton::Right: return VK_RBUTTON;
        case MouseButton::Middle: return VK_MBUTTON;
        case MouseButton::SideBack: return VK_XBUTTON1;
        case MouseButton::SideForward: return VK_XBUTTON2;
        default: return 0;
        }
    }

    ConsoleInput::ConsoleInput()
    {
        consoleWindow = GetConsoleWindow();
        virtualSize = { 1280, 720 };

        // CONSOLE CLICK SECRET:
        // We must tell Windows that this console is an interactive canvas, not a text editor.
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hStdin, &mode);

        // Turn off Quick Edit mode so clicking doesn't freeze the application process
        mode &= ~ENABLE_QUICK_EDIT_MODE;

        // Enable extended capture and mouse events
        mode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;

        SetConsoleMode(hStdin, mode);
    }

    bool ConsoleInput::IsKeyDown(Key key) const
    {
        int vk = ToWin32Key(key);
        if (vk == 0) return false;
        // The most significant bit (0x8000) indicates if the key is pressed RIGHT NOW
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    bool ConsoleInput::IsKeyPressed(Key key) const
    {
        return IsKeyDown(key) && !previousKeyStates[static_cast<int>(key)];
    }

    bool ConsoleInput::IsKeyReleased(Key key) const
    {
        return !IsKeyDown(key) && previousKeyStates[static_cast<int>(key)];
    }

    bool ConsoleInput::IsMouseButtonDown(MouseButton button) const
    {
        int vk = ToWin32Mouse(button);
        if (vk == 0) return false;
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    bool ConsoleInput::IsMouseButtonPressed(MouseButton button) const
    {
        return IsMouseButtonDown(button) && !previousMouseStates[static_cast<int>(button)];
    }

    bool ConsoleInput::IsMouseButtonReleased(MouseButton button) const
    {
        return !IsMouseButtonDown(button) && previousMouseStates[static_cast<int>(button)];
    }

    Vector2f ConsoleInput::GetMousePosition() const
    {
        // 1. Get cursor position in global screen coordinates
        POINT pt;
        if (!GetCursorPos(&pt)) return { 0.0f, 0.0f };

        // 2. Convert coordinates to local window space (0,0 is the top-left of the client area)
        if (!ScreenToClient(consoleWindow, &pt)) return { 0.0f, 0.0f };

        // 3. Get the REAL size in pixels of the client area (where we render)
        RECT clientRect;
        GetClientRect(consoleWindow, &clientRect);

        float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
        float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

        // Prevent division by zero if the window is minimized
        if (clientWidth <= 0.0f || clientHeight <= 0.0f) return { 0.0f, 0.0f };

        // 4. Map from window pixels to the engine's virtual resolution (e.g. 1280x720)
        float ratioX = static_cast<float>(pt.x) / clientWidth;
        float ratioY = static_cast<float>(pt.y) / clientHeight;

        float finalX = ratioX * virtualSize.x;
        float finalY = ratioY * virtualSize.y;

        // 5. Y-Axis adjustment for the engine (Y-Up)
        // Since Windows returns Y-Down (0 at top), we invert it to match our logic
        return Vector2f(finalX, -finalY);
    }

    std::optional<Vector2f> ConsoleInput::GetMouseDelta() const
    {
        Vector2f current = GetMousePosition();
        Vector2f delta = { current.x - previousMousePos.x, current.y - previousMousePos.y };

        if (delta.x == 0.0f && delta.y == 0.0f) return std::nullopt;
        return delta;
    }

    float ConsoleInput::GetMouseWheel()
    {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD numEvents = 0;

        // Check if there are accumulated events in the console buffer
        GetNumberOfConsoleInputEvents(hStdin, &numEvents);

        float frameWheelDelta = 0.0f;

        if (numEvents > 0)
        {
            std::vector<INPUT_RECORD> records(numEvents);
            DWORD numRead = 0;
            ReadConsoleInputA(hStdin, records.data(), numEvents, &numRead);

            for (DWORD i = 0; i < numRead; ++i)
            {
                if (records[i].EventType == MOUSE_EVENT)
                {
                    if (records[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
                    {
                        // The API returns a large number; the standard is 1 wheel click = 120.
                        // We use HIWORD to extract that value and normalize it to 1.0f or -1.0f
                        short wheelData = static_cast<short>(HIWORD(records[i].Event.MouseEvent.dwButtonState));
                        frameWheelDelta += static_cast<float>(wheelData) / 120.0f;
                    }
                }
            }
        }

        return frameWheelDelta;
    }
}