
COMPILE_OPTS = -c -g -mcpu=cortex-m3 -mthumb -Wall -DSYSCLK_72MHZ	-g3 



TCHAIN_PREFIX=arm-none-eabi-
INCLUDE_DIRS = -I . -I stm/inc -I ..
F103_DIR = stm32f103

CC = $(TCHAIN_PREFIX)gcc
CFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

AS = $(TCHAIN_PREFIX)gcc
ASFLAGS = $(COMPILE_OPTS) -c

LD = $(TCHAIN_PREFIX)gcc
LDFLAGS = -Wl,--gc-sections,-Map=$@.map,-cref,-u,Reset_Handler -T $(F103_DIR)/stm32f103.ld


OBJCP = $(TCHAIN_PREFIX)objcopy
OBJCPFLAGS = -O binary

OBJDUMP = $(TCHAIN_PREFIX)objdump
OBJDUMPFLAGS = -h -S -C -D

OBJSIZE = $(TCHAIN_PREFIX)size
OBJSIZEFLAGS = --format=SysV

MAIN_OUT = bin/firmware
MAIN_OUT_ELF = $(MAIN_OUT).elf
MAIN_OUT_BIN = $(MAIN_OUT).bin
MAIN_OUT_LSS = $(MAIN_OUT).lss

SRC_OBJS = \
src/systick.o \
src/main.o \
src/r503.o \
src/uart.o \

# all

all: $(MAIN_OUT_ELF) $(MAIN_OUT_BIN) $(MAIN_OUT_LSS)


$(MAIN_OUT_ELF): $(F103_DIR)/system_stm32f10x.o $(F103_DIR)/startup_stm32f10x.o $(SRC_OBJS) 
	$(LD) $(LDFLAGS) $(F103_DIR)/system_stm32f10x.o $(F103_DIR)/startup_stm32f10x.o $(SRC_OBJS) --output $@



$(MAIN_OUT_BIN): $(MAIN_OUT_ELF)
	$(OBJCP) $(OBJCPFLAGS) $< $@
	$(OBJSIZE) $(OBJSIZEFLAGS) $(MAIN_OUT_ELF)

$(MAIN_OUT_LSS): $(MAIN_OUT_ELF)
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
	

clean:
	-rm src/*.o bin/*.bin bin/*.elf bin/*.map bin/*.lss