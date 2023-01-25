CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)gcc

CFLAGS = -Wall -Wextra -Werror -Wno-main -ffreestanding
CFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
# CFLAGS += -mcpu=cortex-m3
LDFLAGS = -nostartfiles -nostdlib -Wl,-Map,image.map -T link.ld -lgcc

.SUFFIXES: .elf .bin
.PHONY: all clean

all: image.bin

.c.o:
	$(CC) $(CFLAGS) $(CFLAGS_$@) -c $< -o $@

image.elf: main.o handlers.o stdlib.o semihosting.o printf.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.elf.bin:
	$(CROSS_COMPILE)objcopy -O binary $< $@

clean:
	rm -f *.o *.elf *.bin
