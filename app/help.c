#include "nar_string.h"
#include "shell.h"

char* info[] = {"help [cmd]", "echo [msg]"};

u8 help_handler(const char* param, char* output) {
    struct range r = word_catch(param);
    u8 l = r.end - r.begin;
    if (l) {
        u8 cmd = cmd_match(param + r.begin, l);
        if (cmd != 255) {
            string_copy(info[cmd], output);
        } else {
            string_copy("No such command", output);
        }
    } else {
        string_copy("All cmd: help echo", output);
    }
    return 0;
}