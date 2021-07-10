#include "stm32f10x.h"

#include "echo.h"
#include "nar_string.h"

u8 cmd_handler(const char* cmd, char* output) {
    struct range r = word_catch(cmd);
    u8 l = r.end - r.begin;
    if (l) {
        switch (l) {
            case 4:
                if (string_cmp("echo", cmd + r.begin) == 0) {
                    return echo_handler(cmd + r.end, output);
                }

            default:
                string_copy("cmd not found", output);
                return 1;
        }
    } else {
        // nothing input
        output[0] = 0;
        return 0;
    }
}