#include "nar_string.h"
#include "stm32f10x.h"

// from main.c
extern const char input_buffer[];
extern char output_buffer[];
static const char* input_now;

u8 string_length(const char* p) {
    u8 i = 0;
    while (p[i] != 0)
        i++;
    return i;
}

/**
 * WARNING: make sure length(s) < length(output_buffer)
 */
void set_output(const char* s) {
    u8 i = 0;
    for (; s[i] != 0; i++) {
        output_buffer[i] = s[i];
    }
    output_buffer[i] = 0;
}

void word_match_reset() {
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
u8 word_match(const char* words[], u8 num_of_words) {
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