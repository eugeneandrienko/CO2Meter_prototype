BIN=co2meter_prototype

TOOLS_PATH=/usr
TOOLS_PREFIX=arm-none-eabi-
TOOLS_VERSION=4.9.3

CFLAGS=-c -mcpu=cortex-m3 -mthumb -Wall -O0 -mapcs-frame -D__thumb2__=1
CFLAGS+=-msoft-float -gdwarf-2 -mno-sched-prolog -fno-hosted -mtune=cortex-m3
CFLAGS+=-march=armv7-m -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections
CFLAGS+=-I./cmsis -I./stm32_lib -I. -std=c99
CFLAGS+=-Wpedantic -Wbad-function-cast -Wcast-align -Wextra
ASFLAGS=-mcpu=cortex-m3 -I./cmsis -I./stm32_lib -gdwarf-2 -gdwarf-2
LDFLAGS=-static -mcpu=cortex-m3 -mthumb -mthumb-interwork -Wl,--start-group
LDFLAGS+=-nostdlib -lg -lstdc++ -lsupc++ -lgcc -lm
LDFLAGS+=-Ttext=0x8000000
LDFLAGS+=-Wl,--end-group -Xlinker -Map -Xlinker $(BIN).map -Xlinker
LDFLAGS+=-T ./stm32_lib/device_support/gcc/stm32f100rb_flash.ld -o $(BIN).elf

CC=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)gcc-$(TOOLS_VERSION)
AS=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)as
SIZE=$(TOOLS_PATH)/bin/$(TOOLS_PREFIX)size

CMSIS_SOURCES=./cmsis/core_cm3.c
STM32_LIB_SOURCES=./stm32_lib/system_stm32f10x.c ./stm32_lib/stm32f10x_it.c
STM32_LIB_SOURCES+=./stm32_lib/stm32f10x_rcc.c ./stm32_lib/stm32f10x_gpio.c
SOURCES=main.c strings.c numbers.c
SOURCES+=./drivers/uart.c ./drivers/system_clock.c
SOURCES+=./drivers/mhz19b.c ./drivers/display.c

OBJECTS=$(SOURCES:.c=.o)
CMSIS_OBJECTS=core_cm3.o
STM32_LIB_OBJECTS=system_stm32f10x.o stm32f10x_it.o startup_stm32f10x_md_vl.o
STM32_LIB_OBJECTS+=stm32f10x_rcc.o stm32f10x_gpio.o

PVS_CFG=PVS-Studio.cfg
PVS_I_OBJECTS=$(SOURCES:.c=.o.PVS-Studio.i)
PVS_LOG_OBJECTS=$(SOURCES:.c=.o.PVS-Studio.log)
PVS_LOG=PVS-Studio.RESULTS

