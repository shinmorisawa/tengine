#pragma once

#define EVENT_BUFFER_SIZE 128

typedef enum Keycode {
    KEY_UNKNOWN = 0,
    KEY_ESC = 27,
    KEY_ENTER = 10,
    KEY_BACKSPACE = 127,

    KEY_UP = 1000,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,

    KEY_F1 = 1200,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_HOME = 1300,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_DELETE
} Keycode;

typedef enum EventType {
    KEY,
    RESIZE,
    MOUSE
} EventType;

typedef struct Event {
    EventType type;
    Keycode keycode;
} Event;

void update_input(void);
Event pop_event(void);
