#include "nar_string.h"
#include "stm32f10x_gpio.h"

static const struct keyword port[] = {
    {"a", 1},
    {"b", 1},
    {"c", 1},
};

u8 gpio_handler(const char* param, char* output) {}