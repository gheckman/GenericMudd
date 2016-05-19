#pragma once

#if defined(_WIN32)
#    define WINDOWS_LEAN_AND_MEAN
#    define VC_EXTRALEAN
#    include <winsock2.h> // boost gets mad without this
#    include <windows.h>
#else
    // LINUX? MAC?
#endif

#include "KeyCode.hpp"

#include <string>

class Console
{
public:
    enum { WIDTH = 120, HEIGHT = 64 };

    Console() :
        output(GetStdHandle(STD_OUTPUT_HANDLE)),
        input(GetStdHandle(STD_INPUT_HANDLE))
    {}

    void SetCursor(int x, int y) const;
    void GetCursor(int& x, int& y) const;

    void ClearChar() const;
    void ClearChar(int x, int y) const;
    void ClearString(int amount) const;
    void ClearString(int x, int y, int amount) const;
    void ClearLine() const;
    void ClearLine(int y) const;
    void Clear() const;

    void WriteChar(char c) const;
    void WriteChar(int x, int y, char c) const;
    void WriteString(int amount, char c) const;
    void WriteString(int x, int y, int amount, char c) const;
    void WriteString(std::string s) const;
    void WriteString(int x, int y, std::string s) const;
    void WriteLine(char c) const;
    void WriteLine(int y, char c) const;

    KeyCode ReadKey();

private:
    HANDLE output;
    HANDLE input;
};