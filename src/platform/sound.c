/*
* sound.c — Implementation of terminal bell alerts.
 *
 * Commit: feat(platform): implement terminal bell output via write()
 *
 * Depends on: sound.h, <unistd.h>
 */
#include "traffic3d/platform/sound.h"
#include <unistd.h>

void sound_beep(void) {
    /*
     * Write the Bell character (0x07) directly to stdout.
     * We use write() instead of printf() to bypass notcurses'
     * pixel buffer and ensure the terminal actually rings.
     */
    const char bell = '\a';
    (void)write(STDOUT_FILENO, &bell, 1);
}
