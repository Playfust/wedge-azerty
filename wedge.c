/* vim: set ts=4 sw=4 tw=100 et : */

/*
 * MIT License
 * 
 * Copyright (c) 2021 Riccardo Paolo Bestetti
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>

#define die(str) { \
    perror(str); \
    exit(EXIT_FAILURE); \
};

#if 0
    #define dbg(...) fprintf(stderr, __VA_ARGS__)
#else
    #define dbg(...)
#endif

static const struct key_map map[] = {
    { '0', KEY_0 },
    { '1', KEY_AMPERSAND },
    { '2', KEY_EACUTE },
    { '3', KEY_QUOTEDBL },
    { '4', KEY_APOSTROPHE },
    { '5', KEY_LEFTPAREN },
    { '6', KEY_MINUS },
    { '7', KEY_EGRAVE },
    { '8', KEY_UNDERSCORE },
    { '9', KEY_CCEDILLA },

    { 'A', KEY_Q },  // A → A en AZERTY
    { 'B', KEY_B },
    { 'C', KEY_C },
    { 'D', KEY_D },
    { 'E', KEY_E },
    { 'F', KEY_F },
    { 'G', KEY_G },
    { 'H', KEY_H },
    { 'I', KEY_I },
    { 'J', KEY_J },
    { 'K', KEY_K },
    { 'L', KEY_L },
    { 'M', KEY_SEMICOLON },  // M = ; en US
    { 'N', KEY_N },
    { 'O', KEY_O },
    { 'P', KEY_P },
    { 'Q', KEY_A },  // Q → A en AZERTY
    { 'R', KEY_R },
    { 'S', KEY_S },
    { 'T', KEY_T },
    { 'U', KEY_U },
    { 'V', KEY_V },
    { 'W', KEY_Z }, 
    { 'X', KEY_X },
    { 'Y', KEY_Y },
    { 'Z', KEY_W },

    { '.', KEY_COMMA },  // . = , en AZERTY
    { ',', KEY_SEMICOLON }, 
    { ':', KEY_DOT }, 
    { ' ', KEY_SPACE },
};



/* mod_map : modification pour AZERTY -> mettre Shift (1) sur ASCII 48..57 (0..9) */
int mod_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0,          /* 0-7 */
    0, 0, 0, 0,                      /* 8-11 */
    0, 0, 0, 0,                      /* 12-15 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 16-23 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 24-31 */
    0, 0, 0, 0, 0, 0, 0, 0,          /* 32-39 */
    0, 0, 0, 0,                      /* 40-43 */
    0, 0, 0, 0,                      /* 44-47 */
    /* 48..57 -> '0'..'9' : ajouter Shift (1) pour AZERTY */
    1, 1, 1, 1,                      /* 48-51 : '0','1','2','3' */
    1, 1, 1, 1,                      /* 52-55 : '4','5','6','7' */
    1, 1, 0, 0, 0, 0, 0, 0,          /* 56-63 : '8','9', ... */
    0, 1, 1, 1,                      /* 64-67 */
    1, 1, 1, 1,                      /* 68-71 */
    1, 1, 1, 1,                      /* 72-75 */
    1, 1, 1, 1,                      /* 76-79 */
    1, 1, 1, 1,                      /* 80-83 */
    1, 1, 1, 1,                      /* 84-87 */
    1, 1, 1, 0,                      /* 88-91 */
    0, 0, 0, 0,                      /* 92-95 */
    0, 0, 0, 0,                      /* 96-99 */
    0, 0, 0, 0,                      /* 100-103 */
    0, 0, 0, 0,                      /* 104-107 */
    0, 0, 0, 0,                      /* 108-111 */
    0, 0, 0, 0,                      /* 112-115 */
    0, 0, 0, 0,                      /* 116-119 */
    0, 0, 0, 0,                      /* 120-123 */
    0, 0, 0, 0,                      /* 124-127 */
    KEY_LEFTSHIFT                    /* the mod key (laisser tel quel) */
};

int setup_uinput()
{
    struct uinput_setup usetup;
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (fd < 0)
        die("error: open");

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) 
        die("error: ioctl1");

    for (size_t i = 0; i < sizeof(key_map) / sizeof (key_map[0]); i++)
        if (ioctl(fd, UI_SET_KEYBIT, key_map[i]) < 0)
            die("error: ioctl2a");

    if (ioctl(fd, UI_SET_KEYBIT, mod_map[128]) < 0)
        die("error: ioctl2b");

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_VIRTUAL;
    usetup.id.vendor = 0x5697;
    usetup.id.product = 0x0001;
    strcpy(usetup.name, "bestowedge");

    if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0)
        die("error: ioctl3");

    if (ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl4");

    return fd;
}

void emit_key(int fd, int code, int val)
{
   struct input_event ie;

   ie.type = EV_KEY;
   ie.code = code;
   ie.value = val;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

void emit_syn(int fd)
{
   struct input_event ie;

   ie.type = EV_SYN;
   ie.code = SYN_REPORT;
   ie.value = 0;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

void emit_key_event(int fd, int code, int press)
{
    dbg("ev code %d (p? %d)", code, press);
    emit_key(fd, code, press);
    emit_syn(fd);
}

int main()
{
    int fd = setup_uinput();

    int ch;
    while ((ch = getchar()) != EOF)
    {
        dbg("proc '%c' (%d)\n", ch, ch);

        if (mod_map[ch])
            emit_key_event(fd, mod_map[128], 1);

        emit_key_event(fd, key_map[ch], 1);
        emit_key_event(fd, key_map[ch], 0);

        if (mod_map[ch])
            emit_key_event(fd, mod_map[128], 0);
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

