#ifndef STRING_H
#define STRING_H

#include "stm32f10x.h"

struct range {
    u8 begin;
    u8 end;
};

struct keyword {
    char* word;
    u8 length;
};

struct range word_catch(const char* p);
u8 string_length(const char* p);
u8 word_cmp(const char* p, const char* q);
void string_copy(const char* s, char* target);
u8 word_match(const struct keyword* words,
              u8 num_of_words,
              const char* target,
              u8 length);

#endif