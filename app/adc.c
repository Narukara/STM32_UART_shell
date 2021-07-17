#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"

#include "nar_string.h"
#include "uart.h"

void adc_init(uint8_t channel) {
    static u8 inited = 0;
    if (inited) {
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
    } else {
        inited = 1;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div8);  // 72M / 8 = 9M
        ADC_Init(ADC1, &(ADC_InitTypeDef){
                           .ADC_Mode = ADC_Mode_Independent,
                           .ADC_ScanConvMode = DISABLE,
                           .ADC_ContinuousConvMode = DISABLE,
                           .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
                           .ADC_DataAlign = ADC_DataAlign_Right,
                           .ADC_NbrOfChannel = 1,
                       });
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
        ADC_Cmd(ADC1, ENABLE);

        ADC_ResetCalibration(ADC1);
        while (ADC_GetResetCalibrationStatus(ADC1) == SET)
            ;
        ADC_StartCalibration(ADC1);
        while (ADC_GetCalibrationStatus(ADC1) == SET)
            ;
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    }
}

#define NUM_OF_OP 2
static const char* OP[] = {"init", "read"};

static const uint8_t CHANNELS[] = {
    ADC_Channel_0,  ADC_Channel_1,  ADC_Channel_2,  ADC_Channel_3,
    ADC_Channel_4,  ADC_Channel_5,  ADC_Channel_6,  ADC_Channel_7,
    ADC_Channel_8,  ADC_Channel_9,  ADC_Channel_10, ADC_Channel_11,
    ADC_Channel_12, ADC_Channel_13, ADC_Channel_14, ADC_Channel_15,
    ADC_Channel_16, ADC_Channel_17,
};

u8 adc_handler() {
    u8 which_op = match_word(OP, NUM_OF_OP);
    if (which_op >= NUM_OF_OP) {
        goto error;
    }
    if (which_op == 0) {
        // init
        struct error_num en = match_dec();
        if (en.is_ok || en.num > 17) {
            goto error;
        }
        adc_init(CHANNELS[en.num]);
        uart_send("ok");
    } else {
        // read
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
            ;
        char result[] = "0000";
        u12_to_dec(ADC_GetConversionValue(ADC1), result);
        uart_send(result);
    }
    return 0;
error:
    uart_send("parameter error");
    return 1;
}