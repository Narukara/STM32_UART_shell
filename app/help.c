#include "nar_string.h"

extern const u8 NUM_OF_CMDS;
extern const struct keyword CMDS_LIST[];

// here to add cmd info
static const char* info[] = {
    "help [cmd]",
    "echo [msg]",
    "rcc [periph] en/dis",
    "gpio init [port] [num] [mode] [speed]\r\n"
    "gpio read [port] [num]\r\n"
    "gpio write [port] [num] 1/0",
};

u8 help_handler(const char* param, char* output) {
    struct range r = word_catch(param);
    u8 l = r.end - r.begin;
    if (l) {
        u8 cmd = word_match(CMDS_LIST, NUM_OF_CMDS, param + r.begin, l);
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