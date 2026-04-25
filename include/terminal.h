#pragma once

#include "base.h"

void terminal_init(void);
void terminal_kill(void);
void terminal_clear(void);
void terminal_move(u16 x, u16 y);
void terminal_cursor_show(void);
void terminal_cursor_hide(void);
void terminal_get_size(u16* w, u16* h);
void terminal_alt_enter(void);
void terminal_alt_exit(void);
void terminal_flush(void);
