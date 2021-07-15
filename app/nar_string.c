#include "stm32f10x.h"

#include "nar_string.h"

// from main.c
extern const char input_buffer[];
static const char* input_now;

/**
 * @return length of string p
 */
u8 string_length(const char* p) {
    u8 i = 0;
    while (p[i] != 0)
        i++;
    return i;
}

/**
 * Only used in shell.c
 */
void match_reset() {
    input_now = input_buffer;
}

/**
 * p should be lowcase
 * @return 1 if same (ignore case)
 */
static u8 word_cmp(const char* p, const char* q) {
    for (u8 i = 0;; i++) {
        if (q[i] >= 'A' && q[i] <= 'Z') {
            if (p[i] != q[i] + 32) {
                return 0;
            }
        } else {
            if (p[i] != q[i]) {
                return (p[i] == 0 && q[i] == ' ');
            } else if (p[i] == 0) {
                return 1;
            }
        }
    }
}

struct range {
    u8 begin;
    u8 end;
};

/**
 *      _  _   c  m  d _  _ /0
 *      |      |       |
 *     now  now+begin  now+end
 *
 *      _  _   _   _   _ /0
 *      |
 *     now    begin = end = 0
 *
 */
static struct range word_catch() {
    struct range r = {0, 0};
    u8 found = 0, i = 0;
    while (1) {
        if (input_now[i] == 0) {
            if (found) {
                r.end = i;
            }
            return r;
        }
        if (found) {
            if (input_now[i] == ' ') {
                r.end = i;
                return r;
            }
        } else {
            if (input_now[i] != ' ') {
                r.begin = i;
                found = 1;
            }
        }
        i++;
    }
}

/**
 * @return 254 if no input, 255 if no match
 */
u8 match_word(const char* words[], u8 num_of_words) {
    struct range r = word_catch();
    if (r.end == 0) {
        return 254;
    }
    const char* temp = input_now + r.begin;
    for (u8 j = 0; j < num_of_words; j++) {
        if (word_cmp(words[j], temp)) {
            input_now += r.end;
            return j;
        }
    }
    return 255;
}

u8 dec_to_u8(const char* p) {
    u8 r = 0;
    for (u8 i = 0; p[i] >= '0' && p[i] <= '9'; i++) {
        r *= 10;
        r += p[i] - 48;
    }
    return r;
}

#define is_hex(a)                                                \
    (((a) >= '0' && (a) <= '9') || ((a) >= 'a' && (a) <= 'f') || \
     ((a) >= 'A' && (a) <= 'F'))

/**
 * Match input in the form of 0xab, where ab is a hexadecimal number
 */
struct error_num match_hex() {
    struct error_num en = {0, 0};
    struct range r = word_catch();
    u8 l = r.end - r.begin;
    if (l != 4 || input_now[r.begin] != '0' || input_now[r.begin + 1] != 'x' ||
        !is_hex(input_now[r.begin + 2]) || !is_hex(input_now[r.begin + 3])) {
        en.is_ok = 1;
        return en;
    }
    char a = input_now[r.begin + 2];
    if (a <= '9') {
        en.num |= (a - '0');
    } else if (a <= 'F') {
        en.num |= (a - 55);
    } else {
        en.num |= (a - 87);
    }
    en.num <<= 4;
    a = input_now[r.begin + 3];
    if (a <= '9') {
        en.num |= (a - '0');
    } else if (a <= 'F') {
        en.num |= (a - 55);
    } else {
        en.num |= (a - 87);
    }
    input_now += r.end;
    return en;
}

void u8_to_hex(u8 num, char* hex) {
    char* t = "0123456789abcdef";
    hex[0] = t[(num & 0xf0) >> 4];
    hex[1] = t[num & 0x0f];
}