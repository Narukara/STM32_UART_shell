#include "nar_string.h"
#include "uart.h"

// from shell.c
extern const u8 NUM_OF_CMDS;
extern const char* CMDS_KW[];

// here to add cmd info
static const char* ALL_CMDS_info = "All cmd: help rcc gpio spi i2c";
static const char* CMD_info[] = {
    "help [cmd]",
    "rcc [periph] en/dis\r\n"
    "e.g. rcc GPIOB en",
    "gpio init [port] [num] [mode] [speed]\r\n"
    "     read [port] [num]\r\n"
    "     write [port] [num] 0/1\r\n"
    "e.g. gpio init C 13 opp 2",
    "spi init\r\n"
    "    cs 0/1\r\n"
    "    send [hex]\r\n"
    "e.g. spi send 0x1a",
    "i2c init\r\n"
    "    write [addr] [data]...\r\n"
    "    read [addr] [num]\r\n"
    "e.g. i2c write 0x3c 0x80 0x8d\r\n"
    "     i2c read 0x3c 2",
};

u8 help_handler() {
    u8 which = match_word(CMDS_KW, NUM_OF_CMDS);
    if (which < NUM_OF_CMDS) {
        uart_send(CMD_info[which]);
        return 0;
    } else if (which == 254) {
        uart_send(ALL_CMDS_info);
        return 0;
    } else {
        uart_send("No such command");
        return 1;
    }
}