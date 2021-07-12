#ifndef STRING_H
#define STRING_H

#include "stm32f10x.h"

u8 string_length(const char* p);
void set_output(const char* s);
void word_match_reset();
u8 word_match(const char* words[], u8 num_of_words);
u8 dec_to_u8(const char* p);

#endif