CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m33 -mthumb -Wall -Os -ffreestanding \
       -I../base/simplicity_sdk_2025.6.0/platform/CMSIS/Core/Include \
       -I../base/simplicity_sdk_2025.6.0/platform/Device/SiliconLabs/EFR32BG22/Include \
       -DEFR32BG22C224F512IM40
LDSCRIPT = efr32bg22.ld

SRCS = startup.s main.c

all: blink.elf

blink.elf: $(SRCS)
	$(CC) $(CFLAGS) -T$(LDSCRIPT) $(SRCS) -nostdlib -Wl,--gc-sections -o $@

clean:
	rm -f *.o blink.elf

flash: blink.elf
	@echo "Run JLinkExe or other tool to flash blink.elf"
