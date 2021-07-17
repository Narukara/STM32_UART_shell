#include "stm32f10x.h"

#include "adc.h"
#include "gpio.h"
#include "help.h"
#include "i2c.h"
#include "nar_string.h"
#include "rcc.h"
#include "shell.h"
#include "spi.h"
#include "uart.h"

// here to add cmd
const u8 NUM_OF_CMDS = 6;
const char* CMDS_KW[] = {"help", "rcc", "gpio", "spi", "i2c", "adc"};
static u8 (*const CMD_handler[])() = {
    help_handler, rcc_handler, gpio_handler,
    spi_handler,  i2c_handler, adc_handler,
};

u8 cmd_handler() {
    match_reset();
    u8 which = match_word(CMDS_KW, NUM_OF_CMDS);
    if (which < NUM_OF_CMDS) {
        return CMD_handler[which]();
    } else if (which == 254) {
        // no input
        uart_send("\e[A");
        return 0;
    } else {
        // bad input
        uart_send("cmd not found");
        return 1;
    }
}