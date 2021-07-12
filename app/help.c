#include "nar_string.h"

// from shell.c
extern const u8 NUM_OF_CMDS;
extern const char* CMDS_KW[];

// here to add cmd info
static const char* ALL_CMDS_info = "All cmd: help rcc gpio";
static const char* CMD_info[] = {
    "help [cmd]",
    "rcc [periph] en/dis\r\n"
    "e.g. rcc GPIOB en",
    "gpio init [port] [num] [mode] [speed]\r\n"
    "gpio read [port] [num]\r\n"
    "gpio write [port] [num] 0/1\r\n"
    "e.g. gpio init C 13 opp 2",
};

u8 help_handler() {
    u8 which = word_match(CMDS_KW, NUM_OF_CMDS);
    if (which < 254) {
        set_output(CMD_info[which]);
        return 0;
    } else if (which == 254) {
        set_output(ALL_CMDS_info);
        return 0;
    } else {
        set_output("No such command");
        return 1;
    }
}