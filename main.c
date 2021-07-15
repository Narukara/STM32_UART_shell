#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

#include "shell.h"
#include "uart.h"

#define MAX_IN 64

char input_buffer[MAX_IN];

/**
 * PC13 is a LED
 */

int main() {
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1,
        ENABLE);

    // LED flash, use SysTick
    GPIO_Init(GPIOC, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_13,
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_Out_PP});
    SysTick->LOAD = 9000000;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x03;

    // config USART1
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_9,  // TX
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_AF_PP});
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_10,  // RX
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_IN_FLOATING});
    USART_Init(USART1, &(USART_InitTypeDef){
                           .USART_BaudRate = 115200,
                           .USART_WordLength = USART_WordLength_9b,
                           .USART_StopBits = USART_StopBits_1,
                           .USART_Parity = USART_Parity_Odd,
                           .USART_Mode = USART_Mode_Rx | USART_Mode_Tx});
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_Init(&(NVIC_InitTypeDef){.NVIC_IRQChannel = USART1_IRQn,
                                  .NVIC_IRQChannelCmd = ENABLE,
                                  .NVIC_IRQChannelPreemptionPriority = 1,
                                  .NVIC_IRQChannelSubPriority = 1});
    USART_Cmd(USART1, ENABLE);

    // welcome
    uart_send("\r\n\e[1m--- stm32 shell: Hello! ---\e[0m");
    uart_send("\r\n\e[33mstm32>\e[0m");

    while (1) {
    }
}

void USART1_IRQHandler(void) {
    static u8 now = 0;
    USART_ClearFlag(USART1, USART_FLAG_RXNE);
    u8 data = USART_ReceiveData(USART1);
    switch (data) {
        case 8:  // backspace
            if (now > 0) {
                uart_send("\b \b");
                now--;
            }
            break;
        case 13:  // \r, finish input
            uart_send("\r\n");
            input_buffer[now] = 0;
            now = 0;
            if (cmd_handler()) {
                uart_send("\e[31m [ERROR]\e[0m");
            }
            uart_send("\r\n\e[33mstm32>\e[0m");
            break;
        default:
            if (data >= ' ' && now < MAX_IN - 1) {
                input_buffer[now++] = data;
                uart_send_bit(data);
            }
    }
}

void SysTick_Handler(void) {
    static u8 status = 0;
    if (status) {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    }
    status = !status;
}

#ifdef USE_FULL_ASSERT

/**
 * When assert fail, accelerate LED
 */
void assert_failed(uint8_t* file, uint32_t line) {
    SysTick->CTRL = 0x00;
    SysTick->LOAD = 2000000;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x03;
    while (1)
        ;
}

#endif