#include "nar_string.h"
#include "stm32f10x_rcc.h"

u32 periph[] = {
    RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC,
    RCC_APB2Periph_TIM1,  RCC_APB2Periph_SPI1,  RCC_APB2Periph_ADC1,
};
#define NUM_OF_PERIPH 6
struct keyword periph_keyword[] = {
    {"gpioa", 5}, {"gpiob", 5}, {"gpioc", 5},
    {"tim1", 4},  {"spi1", 4},  {"adc1", 4},
};

FunctionalState status[] = {ENABLE, DISABLE};
struct keyword en_dis[] = {{"en", 2}, {"dis", 3}};

u8 rcc_handler(const char* param, char* output) {
    struct range r = word_catch(param);
    u8 l = r.end - r.begin;
    if (l) {
        u8 which_p =
            word_match(periph_keyword, NUM_OF_PERIPH, param + r.begin, l);
        if (which_p != 255) {
            param += r.end;
            r = word_catch(param);
            l = r.end - r.begin;
            if (l) {
                u8 which_s = word_match(en_dis, 2, param + r.begin, l);
                if (which_s != 255) {
                    RCC_APB2PeriphClockCmd(periph[which_p], status[which_s]);
                    string_copy("ok", output);
                    return 0;
                }
            }
        }
    }
    // error
    string_copy("parameter error", output);
    return 1;
}