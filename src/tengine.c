#include "terminal.h"
#include "base.h"
#include "color.h"
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdarg.h>

struct termios orig_termios;
static u16 width, height;

static ColorRGB* back_fg_color_buffer = NULL;
static ColorRGB* back_bg_color_buffer = NULL;
static char* back_char_buffer = NULL;

static ColorRGB* front_fg_color_buffer = NULL;
static ColorRGB* front_bg_color_buffer = NULL;
static char* front_char_buffer = NULL;

void terminal_kill(void) {
    terminal_cursor_show();
    terminal_alt_exit();
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    free(back_fg_color_buffer);
    free(back_bg_color_buffer);
    free(back_char_buffer);
    free(front_fg_color_buffer);
    free(front_bg_color_buffer);
    free(front_char_buffer);
}

static void terminal_resize_handler(int signum) {
    if (signum != SIGWINCH) return;

    struct winsize size;
    (void)ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    height = size.ws_row;
    width = size.ws_col;

    front_fg_color_buffer = realloc(front_fg_color_buffer, sizeof(ColorRGB) * width * height);
    front_bg_color_buffer = realloc(front_bg_color_buffer, sizeof(ColorRGB) * width * height);
    front_char_buffer = realloc(front_char_buffer, sizeof(char) * width * height);

    back_fg_color_buffer = realloc(back_fg_color_buffer, sizeof(ColorRGB) * width * height);
    back_bg_color_buffer = realloc(back_bg_color_buffer, sizeof(ColorRGB) * width * height);
    back_char_buffer = realloc(back_char_buffer, sizeof(char) * width * height);
}

void terminal_init(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(terminal_kill);

    signal(SIGWINCH, terminal_resize_handler);

    front_fg_color_buffer = malloc(sizeof(ColorRGB));
    front_bg_color_buffer = malloc(sizeof(ColorRGB));
    front_char_buffer = malloc(sizeof(char));

    back_fg_color_buffer = malloc(sizeof(ColorRGB));
    back_bg_color_buffer = malloc(sizeof(ColorRGB));
    back_char_buffer = malloc(sizeof(char));

    terminal_resize_handler(SIGWINCH); // this is dirty but it works

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    terminal_alt_enter();
    terminal_cursor_hide();
    terminal_clear();
    terminal_flush();
}

void terminal_clear(void) {
    printf("\033[2J\033[H");
}

void terminal_move(u16 x, u16 y) {
    printf("\033[%d;%dH", y, x);
}

void terminal_cursor_show(void) {
    printf("\033[?25h");
}

void terminal_cursor_hide(void) {
    printf("\033[?25l");
}

void terminal_get_size(u16* w, u16* h) {
    *h = height;
    *w = width;
}

void terminal_alt_enter(void) {
    printf("\033[?1049h");
}

void terminal_alt_exit(void) {
    printf("\033[?1049l");
}

void terminal_flush(void) {
    fflush(stdout);
}

void terminal_set_color(ColorRGB fg, ColorRGB bg, int x, int y) {
    back_fg_color_buffer[y * width + x] = fg;
    back_bg_color_buffer[y * width + x] = bg;
}

void terminal_write(char c, ColorRGB fg, ColorRGB bg, int x, int y) {
    back_char_buffer[y * width + x] = c;
    back_fg_color_buffer[y * width + x] = fg;
    back_bg_color_buffer[y * width + x] = bg;
}

void terminal_present(void) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (back_char_buffer[y * width + x] != front_char_buffer[y * width + x] || color_rgb_different(back_fg_color_buffer[y * width + x], front_fg_color_buffer[y * width + x]) || color_rgb_different(back_bg_color_buffer[y * width + x], front_bg_color_buffer[y * width + x])) {
                terminal_move(x, y);
                front_char_buffer[y * width + x] = back_char_buffer[y * width + x];
                front_fg_color_buffer[y * width + x] = back_fg_color_buffer[y * width + x];
                front_bg_color_buffer[y * width + x] = back_bg_color_buffer[y * width + x];
                back_char_buffer[y * width + x] = '\0';
                back_fg_color_buffer[y * width + x] = (ColorRGB) { .r = 1, .g = 1, .b = 1};
                ColorRGB fg = front_fg_color_buffer[y * width + x];
                ColorRGB bg = front_bg_color_buffer[y * width + x];
                fg.r *= 255.0;
                fg.g *= 255.0;
                fg.b *= 255.0;
                bg.r *= 255.0;
                bg.g *= 255.0;
                bg.b *= 255.0;

                printf("\033[38;2;%d;%d;%d;48;2;%d;%d;%dm%c", (int)fg.r, (int)fg.g, (int)fg.b, (int)bg.r, (int)bg.g, (int)bg.b, front_char_buffer[y * width + x]);
            }
        }
    }

    terminal_flush();
}
