#ifndef STRING_H
#define STRING_H

#include "stm32f10x.h"

struct range {
    u8 begin;
    u8 end;
};

struct range word_catch(const char* p);
u8 string_length(const char* p);
u8 string_cmp(const char* p, const char* q);
void string_copy(const char* s, char* target);

#endif