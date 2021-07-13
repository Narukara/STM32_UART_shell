#include "stm32f10x.h"

#include "gpio.h"
#include "help.h"
#include "nar_string.h"
#include "rcc.h"
#include "shell.h"
#include "spi.h"

// here to add cmd
const u8 NUM_OF_CMDS = 4;
const char* CMDS_KW[] = {
    "help",
    "rcc",
    "gpio",
    "spi",
};
static const u8 (*CMD_handler[])() = {
    help_handler,
    rcc_handler,
    gpio_handler,
    spi_handler,
};

u8 cmd_handler() {
    match_reset();
    u8 which = match_word(CMDS_KW, NUM_OF_CMDS);
    if (which < 254) {
        return CMD_handler[which]();
    } else if (which == 254) {
        set_output("\e[A");
        return 0;
    } else {
        set_output("cmd not found");
        return 1;
    }
}