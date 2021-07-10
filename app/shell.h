#ifndef SHELL_H
#define SHELL_H

#include "stm32f10x.h"

u8 cmd_handler(const char* cmd, char* output);

#endif