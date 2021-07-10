#include "stm32f10x.h"

#include "echo.h"
#include "help.h"
#include "nar_string.h"

static u8 (*handler[])(const char*, char*) = {help_handler, echo_handler};

u8 cmd_match(const char* cmd, u8 length) {
    switch (length) {
        case 4:
            if (string_cmp("help", cmd) == 0) {
                return 0;
            }
            if (string_cmp("echo", cmd) == 0) {
                return 1;
            }
            break;
    }
    return 255;
}

u8 cmd_handler(const char* cmd, char* output) {
    struct range r = word_catch(cmd);
    u8 l = r.end - r.begin;
    if (l) {
        u8 h = cmd_match(cmd + r.begin, l);
        if (h != 255) {
            return handler[h](cmd + r.end, output);
        } else {
            string_copy("cmd not found", output);
            return 1;
        }
    } else {
        // nothing input
        output[0] = 0;
        return 0;
    }
}