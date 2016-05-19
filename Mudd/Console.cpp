#include "Console.hpp"

#include <conio.h>
#include <iostream>

#include <boost/algorithm/clamp.hpp>

void Console::SetCursor(int x, int y) const
{
    auto sx = static_cast<SHORT>(boost::algorithm::clamp(x, 0, WIDTH - 1));
    auto sy = static_cast<SHORT>(boost::algorithm::clamp(y, 0, HEIGHT - 1));
    COORD pos = { sx, sy };
    SetConsoleCursorPosition(output, pos);
}

void Console::GetCursor(int& x, int& y) const
{
    CONSOLE_SCREEN_BUFFER_INFO outInfo;
    GetConsoleScreenBufferInfo(output, &outInfo);
    x = outInfo.dwCursorPosition.X;
    y = outInfo.dwCursorPosition.Y;
}

void Console::ClearChar() const
{
    int x, y;
    GetCursor(x, y);
    ClearChar(x, y);
}

void Console::ClearChar(int x, int y) const
{
    ClearString(x, y, 1);
}

void Console::ClearString(int amount) const
{
    int x, y;
    GetCursor(x, y);
    ClearString(x, y, amount);
}

void Console::ClearString(int x, int y, int amount) const
{
    WriteString(x, y, amount, ' ');
}

void Console::ClearLine() const
{
    int x, y;
    GetCursor(x, y);
    ClearLine(y);
}

void Console::ClearLine(int y) const
{
    ClearString(0, y, WIDTH);
}

void Console::Clear() const
{
    for (int i = 0; i < HEIGHT; ++i)
        ClearLine(i);
}

void Console::WriteChar(char c) const
{
    int x, y;
    GetCursor(x, y);
    WriteChar(x, y, c);
}

void Console::WriteChar(int x, int y, char c) const
{
    WriteString(x, y, 1, c);
}

void Console::WriteString(int amount, char c) const
{
    int x, y;
    GetCursor(x, y);
    WriteString(x, y, std::string(amount, c));
}

void Console::WriteString(int x, int y, int amount, char c) const
{
    WriteString(x, y, std::string(amount, c));
}

void Console::WriteString(std::string s) const
{
    int x, y;
    GetCursor(x, y);
    WriteString(x, y, s);
}

void Console::WriteString(int x, int y, std::string s) const
{
    auto sx = static_cast<SHORT>(boost::algorithm::clamp(x, 0, WIDTH - 1));
    auto sy = static_cast<SHORT>(boost::algorithm::clamp(y, 0, HEIGHT - 1));
    auto ss = s.substr(0, WIDTH - x);

    int oldX, oldY;
    GetCursor(oldX, oldY);
    SetCursor(sx, sy);
    std::cout << ss;
    SetCursor(oldX, oldY);
}

void Console::WriteLine(char c) const
{
    int x, y;
    GetCursor(x, y);
    WriteLine(y, c);
}

void Console::WriteLine(int y, char c) const
{
    WriteString(0, y, WIDTH, c);
}

KeyCode Console::ReadKey()
{
    auto keyValue = _getch();

    switch (keyValue)
    {
        case KeyCode::KEY_TAG_1:
            keyValue = (0x1 << 8) + _getch();
            break;
        case KeyCode::KEY_TAG_2:
            keyValue = (0x2 << 8) + _getch();
            break;
        default:
            break;
    }

    return static_cast<KeyCode>(keyValue);
}
