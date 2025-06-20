#!/bin/sh
# Example flashing script using JLink
ELF=blink.elf
if [ ! -f "$ELF" ]; then
    echo "Build first with 'make'" >&2
    exit 1
fi
JLINK_CMD_FILE=flash.jlink
cat > $JLINK_CMD_FILE <<EOC
si 1
speed 4000
target EFR32BG22C224F512IM40
r
loadfile $ELF
r
g
exit
EOC
jlink < $JLINK_CMD_FILE
rm -f $JLINK_CMD_FILE
