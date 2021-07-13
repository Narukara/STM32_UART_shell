#include "nar_string.h"
#include "stm32f10x_rcc.h"

#define NUM_OF_PERIPH 6
static const char* PERIPH_KW[] = {
    "gpioa", "gpiob", "gpioc", "tim1", "spi1", "adc1",
};
static const u32 PERIPH[] = {
    RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC,
    RCC_APB2Periph_TIM1,  RCC_APB2Periph_SPI1,  RCC_APB2Periph_ADC1,
};

static const char* STATUS_KW[] = {"en", "dis"};
static const FunctionalState STATUS[] = {ENABLE, DISABLE};

u8 rcc_handler() {
    u8 which_p = match_word(PERIPH_KW, NUM_OF_PERIPH);
    if (which_p < 254) {
        u8 which_s = match_word(STATUS_KW, 2);
        if (which_s < 254) {
            RCC_APB2PeriphClockCmd(PERIPH[which_p], STATUS[which_s]);
            set_output("ok");
            return 0;
        }
    }
    set_output("parameter error");
    return 1;
}