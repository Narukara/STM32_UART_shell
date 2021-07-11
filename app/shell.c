#include "stm32f10x.h"

#include "echo.h"
#include "gpio.h"
#include "nar_string.h"
#include "rcc.h"
#include "shell.h"

static u8 help_handler(const char* param, char* output);

// here to add cmd
#define NUM_OF_CMDS 4
static const struct keyword cmds[] = {
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
        u8 h = word_match(cmds, NUM_OF_CMDS, cmd + r.begin, l);
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

// here to add cmd info
static const char* info[] = {
    "help [cmd]",
    "echo [msg]",
    "rcc [periph] en/dis",
    "gpio init [port] [num] [mode] [speed]\r\n"
    "gpio read [port] [num]\r\n"
    "gpio write [port] [num] 1/0",
};

static u8 help_handler(const char* param, char* output) {
    struct range r = word_catch(param);
    u8 l = r.end - r.begin;
    if (l) {
        u8 cmd = word_match(cmds, NUM_OF_CMDS, param + r.begin, l);
        if (cmd != 255) {
            string_copy(info[cmd], output);
        } else {
            string_copy("No such command", output);
        }
    } else {
        string_copy("All cmd: help echo rcc gpio", output);  // here to add cmd
    }
    return 0;
}