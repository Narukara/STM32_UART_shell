#include "stm32f10x.h"

u8 cmd_handler(const char* cmd, char* output) {
    if (cmd[0] == 'a') {
        return 0;
    } else {
        return 1;
    }
}