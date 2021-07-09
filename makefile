CC = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -g
LINK = -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=build/target.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group
SRC = *.c lib/*.c lib/*.s app/*.c
HEAD = *.h lib/*.h app/*.h
INC = -I . -I lib -I app
PWD = $(shell pwd)
LD = -T lib/stm32_flash_ld.ld
MACRO = -D USE_STDPERIPH_DRIVER -D STM32F10X_LD -D USE_FULL_ASSERT
DEVICE = target/stm32f1x.cfg.bak

# 使用不同芯片：更改LD, MACRO, DEVICE

.PHONY: build clean download flash debug

build : build/target.hex build/target.bin

build/target.bin : build/target.elf
	arm-none-eabi-objcopy $< $@

build/target.hex : build/target.elf
	arm-none-eabi-objcopy $< -Oihex $@

build/target.elf : $(SRC) $(HEAD)
	$(CC) -o $@ $(SRC) $(INC) $(LD) $(MACRO) $(LINK)

clean : 
	rm build/*

download :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE) -c init -c halt -c "flash write_image erase $(PWD)/build/target.bin" -c reset -c shutdown

flash :
	stm32flash -w build/target.hex -v -g 0x0 /dev/ttyUSB0

debug :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE)