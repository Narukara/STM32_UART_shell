#include <stdlib.h>

#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"

#include "nar_string.h"
#include "uart.h"

void i2c_init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7,
                         .GPIO_Mode = GPIO_Mode_AF_OD,
                         .GPIO_Speed = GPIO_Speed_10MHz,
                     });
    I2C_Init(I2C1, &(I2C_InitTypeDef){
                       .I2C_Mode = I2C_Mode_I2C,
                       .I2C_DutyCycle = I2C_DutyCycle_2,
                       .I2C_OwnAddress1 = 0x00,
                       .I2C_Ack = I2C_Ack_Enable,
                       .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
                       .I2C_ClockSpeed = 400000,
                   });
    I2C_Cmd(I2C1, ENABLE);
}

/**
 * @param slave_addr 7 bit address << 1
 */
void i2c_write(u8 slave_addr, u8 reg_addr, u8* data, u8 size) {
    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
        ;
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ==
           ERROR)
        ;
    I2C_SendData(I2C1, reg_addr);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
        ;
    for (u8 i = 0; i < size; i++) {
        I2C_SendData(I2C1, data[i]);
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
            ;
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    // delay, avoid bug
    for (u32 t = 0; t < 10; t++)
        ;
}

/**
 * @param slave_addr 7 bit address << 1
 */
void i2c_read(u8 slave_addr, u8* data, u8 size) {
    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
        ;
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) ==
           ERROR)
        ;
    for (u8 i = 0; i < size; i++) {
        I2C_AcknowledgeConfig(I2C1, (i == size - 1) ? DISABLE : ENABLE);
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
            ;
        data[i] = I2C_ReceiveData(I2C1);
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
}

#define NUM_OF_OP 3
static const char* OP[] = {"init", "write", "read"};

u8 i2c_handler() {
    u8 which_op = match_word(OP, NUM_OF_OP);
    if (which_op >= NUM_OF_OP) {
        goto error;
    }
    if (which_op == 0) {
        // init
        i2c_init();
        uart_send("ok");
        return 0;
    } else if (which_op == 1) {
        // write
        u8 data[16];
        u8 num = 0;
        do {
            struct error_num en = match_hex();
            if (en.is_ok) {
                break;
            }
            data[num++] = en.num;
        } while (num < 16);
        if (num < 2) {
            goto error;
        }
        i2c_write(data[0] << 1, data[1], data + 2, num - 2);
        uart_send("ok");
        return 0;
    } else {
        // read
        struct error_num en = match_hex();
        if (en.is_ok) {
            goto error;
        }
        struct error_num en2 = match_dec();
        if (en2.is_ok) {
            goto error;
        }
        u8* data = malloc(sizeof(u8) * en2.num);
        if (data == NULL) {
            uart_send("No enough space");
            return 1;
        }
        i2c_read(en.num << 1, data, en2.num);
        char temp[] = "0x__ ";
        for (u8 i = 0; i < en2.num; i++) {
            u8_to_hex(data[i], temp + 2);
            uart_send(temp);
        }
        free(data);
        return 0;
    }
error:
    uart_send("parameter error");
    return 1;
}