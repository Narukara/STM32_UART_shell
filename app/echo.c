#include "nar_string.h"

/**
 * >echo xxx yyy
 * xxx yyy
 * >
 */
u8 echo_handler(const char* param, char* output) {
    u8 begin = 0;
    while (param[begin] == ' ')
        begin++;
    string_copy(param + begin, output);
    return 0;
}