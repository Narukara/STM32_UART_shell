#ifndef STRING_H
#define STRING_H

#include "stm32f10x.h"

struct error_num {
    u8 num;
    u8 is_ok;  // 0 is ok
};

void match_reset();
u8 match_word(const char* words[], u8 num_of_words);
struct error_num match_dec();
struct error_num match_hex();
void u8_to_hex(u8 num, char* hex);
void u12_to_dec(u16 num, char* dec);

#endif