#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#include "nar_string.h"
#include "uart.h"

/**
 * SPI1
 * occupied GPIO:
 * PA4  CS (Manual)
 * PA5  SCLK
 * PA6  MISO
 * PA7  MOSI
 */
void spi_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_4,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_Out_PP});
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_AF_PP});
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_6,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_IN_FLOATING});

    SPI_Init(SPI1, &(SPI_InitTypeDef){
                       .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256,
                       .SPI_CPOL = 0,
                       .SPI_CPHA = 0,
                       .SPI_DataSize = SPI_DataSize_8b,
                       .SPI_Direction = SPI_Direction_2Lines_FullDuplex,
                       .SPI_FirstBit = SPI_FirstBit_MSB,
                       .SPI_Mode = SPI_Mode_Master,
                       .SPI_NSS = SPI_NSS_Soft,
                       .SPI_CRCPolynomial = 7});
    SPI_Cmd(SPI1, ENABLE);
}

u8 spi_send(u8 data) {
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;
    return SPI_I2S_ReceiveData(SPI1);
}

#define NUM_OF_OP 3
static const char* OP[] = {"init", "cs", "send"};

static const char* CS[] = {"0", "1"};

u8 spi_handler() {
    u8 which_op = match_word(OP, NUM_OF_OP);
    if (which_op >= NUM_OF_OP) {
        goto error;
    }
    if (which_op == 0) {
        // init
        spi_init();
        uart_send("ok");
        return 0;
    } else if (which_op == 1) {
        // cs
        u8 which_cs = match_word(CS, 2);
        if (which_cs >= 2) {
            goto error;
        }
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, which_cs ? Bit_SET : Bit_RESET);
        uart_send("ok");
        return 0;
    } else {
        // send
        struct error_num en = match_hex();
        if (en.is_ok) {
            goto error;
        }
        char out[] = "receive: 0x  ";
        u8_to_hex(spi_send(en.num), out + 11);
        uart_send(out);
        return 0;
    }
error:
    uart_send("parameter error");
    return 1;
}