CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m33 -mthumb -Wall -O2 \
       -I../base/simplicity_sdk_2025.6.0/platform/emlib/inc \
       -I../base/simplicity_sdk_2025.6.0/platform/CMSIS/Core/Include \
       -I../base/simplicity_sdk_2025.6.0/platform/Device/SiliconLabs/EFR32BG22/Include \
       -I../base/simplicity_sdk_2025.6.0/platform/common/inc \
       -DEFR32BG22C224F512IM40
LDSCRIPT = efr32bg22.ld

SRCS = main.c \
../base/simplicity_sdk_2025.6.0/platform/emlib/src/em_gpio.c \
../base/simplicity_sdk_2025.6.0/platform/emlib/src/em_cmu.c \
../base/simplicity_sdk_2025.6.0/platform/Device/SiliconLabs/EFR32BG22/Source/system_efr32bg22.c \
../base/simplicity_sdk_2025.6.0/platform/Device/SiliconLabs/EFR32BG22/Source/startup_efr32bg22.c

all: blink.elf

blink.elf:
	$(CC) $(CFLAGS) -T$(LDSCRIPT) $(SRCS) -o $@ -Wl,--gc-sections

clean:
	rm -f *.o blink.elf

flash: blink.elf
	@echo "Run JLinkExe or other tool to flash blink.elf"
