#include "input.h"
#include "base.h"
#include <unistd.h>

static Event event_buffer[EVENT_BUFFER_SIZE] = {0};
static int event_count = 0;

void update_input(void) {
    u8 c;
    if (read(STDIN_FILENO, &c, 1) <= 0) return;
    
    Event e = { .type = KEY, .keycode = KEY_UNKNOWN };

    if (c == '\033') {
        u8 seq[5];
        if (read(STDIN_FILENO, &seq[0], 1) == 0) return;
        if (read(STDIN_FILENO, &seq[1], 1) == 0) return;

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                (void)read(STDIN_FILENO, &seq[2], 1);
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '3': { e.keycode = KEY_DELETE; } break;
                        case '5': { e.keycode = KEY_PAGE_UP; } break;
                        case '6': { e.keycode = KEY_PAGE_DOWN; } break;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': { e.keycode = KEY_UP; } break;
                    case 'B': { e.keycode = KEY_DOWN; } break;
                    case 'C': { e.keycode = KEY_RIGHT; } break;
                    case 'D': { e.keycode = KEY_LEFT; } break;
                    case 'H': { e.keycode = KEY_HOME; } break;
                    case 'F': { e.keycode = KEY_END; } break;
                }
            }
        } else {
            e.keycode = KEY_ESC;
        }
    } else {
        e.keycode = c;
    }

    if (event_count < EVENT_BUFFER_SIZE) {
        event_buffer[event_count++] = e;
    }
}

Event pop_event(void) {
    if (event_count == 0) return (Event) { .type = KEY, .keycode = KEY_UNKNOWN };

    Event e = event_buffer[0];
    for (int i = 0; i < event_count - 1; i++) {
        event_buffer[i] = event_buffer[i + 1];
    }

    event_count--;

    return e;
}
