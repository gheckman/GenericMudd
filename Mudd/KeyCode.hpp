#pragma once

enum KeyCode
{
    NONE = 0x00,

    ASCII_MAX = 0x7f,

    KEY_TAG_1 = 0xe0,
    KEY_TAG_2 = 0x00,

    SIGINT = 0x03,

    BACKSPACE = 0x08,
    ENTER = 0x0d,

    // First getch returns 0xe0
    PAGE_UP     =  0x149, // 73
    HOME        =  0x147, // 71
    END         =  0x14f, // 79
    PAGE_DOWN   =  0x151, // 81
    ARROW_UP    =  0x148, // 72
    ARROW_LEFT  =  0x14b, // 75
    ARROW_DOWN  =  0x150, // 80
    ARROW_RIGHT =  0x14d, // 77
    F11         =  0x185, // 133
    F12         =  0x186, // 134

    // First getch returns 0x00
    F1  = 0x23b, // 59
    F2  = 0x23c, // 60
    F3  = 0x23d, // 61
    F4  = 0x23e, // 62
    F5  = 0x23f, // 63
    F6  = 0x240, // 64
    F7  = 0x241, // 65
    F8  = 0x242, // 66
    F9  = 0x243, // 67
    F10 = 0x244, // 68
    KEYBOARD_UP    = 0x248, // 72
    KEYBOARD_LEFT  = 0x24b, // 75
    KEYBOARD_DOWN  = 0x250, // 80
    KEYBOARD_RIGHT = 0x24d, // 77
    
    MIN_KEY = NONE,
    MAX_KEY = KEYBOARD_DOWN,
};
