#include "stm32f10x.h"

#include "echo.h"
#include "gpio.h"
#include "help.h"
#include "nar_string.h"
#include "rcc.h"
#include "shell.h"

// here to add cmd
const u8 NUM_OF_CMDS = 4;
const struct keyword CMDS_LIST[] = {
    {"help", 4},
    {"echo", 4},
    {"rcc", 3},
    {"gpio", 4},
};
static const u8 (*handler[])(const char*, char*) = {
    help_handler,
    echo_handler,
    rcc_handler,
    gpio_handler,
};

u8 cmd_handler(const char* cmd, char* output) {
    struct range r = word_catch(cmd);
    u8 l = r.end - r.begin;
    if (l) {
        u8 h = word_match(CMDS_LIST, NUM_OF_CMDS, cmd + r.begin, l);
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