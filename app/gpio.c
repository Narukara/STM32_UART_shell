#include "stm32f10x_gpio.h"

#include "nar_string.h"
#include "uart.h"

#define NUM_OF_OP 3
static const char* OP_KW[] = {
    "init",
    "read",
    "write",
};

#define NUM_OF_PORT 3
static const char* PORT_KW[] = {"a", "b", "c"};
static GPIO_TypeDef* PORT[] = {GPIOA, GPIOB, GPIOC};

// needs to be improved
static const char* NUMS[] = {
    "0", "1", "2",  "3",  "4",  "5",  "6",  "7",
    "8", "9", "10", "11", "12", "13", "14", "15",
};
static u16 PIN[] = {
    GPIO_Pin_0,  GPIO_Pin_1,  GPIO_Pin_2,  GPIO_Pin_3,
    GPIO_Pin_4,  GPIO_Pin_5,  GPIO_Pin_6,  GPIO_Pin_7,
    GPIO_Pin_8,  GPIO_Pin_9,  GPIO_Pin_10, GPIO_Pin_11,
    GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15,
};

static const char* MODE_KW[] = {
    "ia", "if", "ipd", "ipu", "ood", "opp", "afod", "afpp",
};
static GPIOMode_TypeDef MODE[] = {
    GPIO_Mode_AIN,    GPIO_Mode_IN_FLOATING, GPIO_Mode_IPD,   GPIO_Mode_IPU,
    GPIO_Mode_Out_OD, GPIO_Mode_Out_PP,      GPIO_Mode_AF_OD, GPIO_Mode_AF_PP,
};

static const char* SPEED_KW[] = {"2", "10", "50"};
static GPIOSpeed_TypeDef SPEED[] = {
    GPIO_Speed_2MHz,
    GPIO_Speed_10MHz,
    GPIO_Speed_50MHz,
};

u8 gpio_handler() {
    u8 which_op = match_word(OP_KW, NUM_OF_OP);
    if (which_op >= NUM_OF_OP)
        goto error;
    u8 which_port = match_word(PORT_KW, NUM_OF_PORT);
    if (which_port >= NUM_OF_PORT)
        goto error;
    u8 which_pin = match_word(NUMS, 16);
    if (which_pin >= 16)
        goto error;
    if (which_op == 0) {
        // init
        u8 which_mode = match_word(MODE_KW, 8);
        if (which_mode >= 8)
            goto error;
        u8 which_speed = match_word(SPEED_KW, 3);
        if (which_speed >= 3)
            goto error;
        GPIO_Init(PORT[which_port], &(GPIO_InitTypeDef){
                                        .GPIO_Pin = PIN[which_pin],
                                        .GPIO_Mode = MODE[which_mode],
                                        .GPIO_Speed = SPEED[which_speed],
                                    });
        uart_send("ok");
    } else if (which_op == 1) {
        // read
        if (GPIO_ReadInputDataBit(PORT[which_port], PIN[which_pin])) {
            uart_send("1");
        } else {
            uart_send("0");
        }
    } else {
        // write
        u8 which_0_1 = match_word(NUMS, 2);
        if (which_0_1 >= 2)
            goto error;
        GPIO_WriteBit(PORT[which_port], PIN[which_pin], which_0_1);
        uart_send("ok");
    }
    return 0;
error:
    uart_send("parameter error");
    return 1;
}