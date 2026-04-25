#include "terminal.h"
#include "base.h"
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdarg.h>

struct termios orig_termios;
static u16 width, height;

void terminal_kill(void) {
    terminal_cursor_show();
    terminal_alt_exit();
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void terminal_resize_handler(int signum) {
    if (signum != SIGWINCH) return;

    struct winsize size;
    (void)ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    height = size.ws_row;
    width = size.ws_col;
}

void terminal_init(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(terminal_kill);

    signal(SIGWINCH, terminal_resize_handler);

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
