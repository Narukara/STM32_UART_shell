#include "nar_string.h"
#include "stm32f10x.h"

/**
 *      _  _   c  m  d _  _ /0
 *      |      |       |
 *      p   p+begin  p+end
 *
 *      _  _   _   _   _ /0
 *      |
 *      p
 *      begin = end = 0
 *
 */
struct range word_catch(const char* p) {
    struct range r = {0, 0};
    u8 catch = 0, i = 0;
    for (; p[i] != 0; i++) {
        if (catch) {
            if (p[i] == ' ') {
                r.end = i;
                return r;
            }
        } else {
            if (p[i] != ' ') {
                r.begin = i;
                catch = 1;
            }
        }
    }
    if (catch) {
        r.end = i;
    }
    return r;
}

u8 string_length(const char* p) {
    u8 i = 0;
    while (p[i] != 0)
        i++;
    return i;
}

/**
 * WARNING: make sure length(p) <= length(q),
 * and p should be lowcase
 * @return 0 if same (ignore case)
 */
u8 word_cmp(const char* p, const char* q) {
    for (u8 i = 0; p[i] != 0; i++) {
        if (q[i] >= 'A' && q[i] <= 'Z') {
            if (p[i] != q[i] + 32) {
                return 1;
            }
        } else {
            if (p[i] != q[i]) {
                return 1;
            }
        }
    }
    return 0;
}

void string_copy(const char* s, char* target) {
    u8 i = 0;
    for (; s[i] != 0; i++) {
        target[i] = s[i];
    }
    target[i] = 0;
}

/**
 * @return 255 if no match
 */
u8 word_match(const struct keyword* words,
              u8 num_of_words,
              const char* target,
              u8 length) {
    for (u8 i = 0; i < num_of_words; i++) {
        if (length == words[i].length) {
            if (word_cmp(words[i].word, target) == 0) {
                return i;
            }
        }
    }
    return 255;
}