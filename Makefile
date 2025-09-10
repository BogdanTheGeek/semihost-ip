# Copyright:
#   Stefan Wagner 2023
#   Bogdan Ionescu 2025

# Files and Folders
TARGET  := firmware
SOURCE  := src
BIN     := bin
LIB	  := lib

# Microcontroller Settings
F_CPU   := 24000000
MODEL   := py32f002bx5
LDSCRIPT:= ld/$(MODEL).ld
CPUARCH := -mcpu=cortex-m0plus -mthumb

# Toolchain
PREFIX  := arm-none-eabi
CC      := $(PREFIX)-gcc
OBJCOPY := $(PREFIX)-objcopy
OBJDUMP := $(PREFIX)-objdump
OBJSIZE := $(PREFIX)-size

FSPATH := $(LIB)/uip
FSFILES := $(shell find $(FSPATH)/fs/ -type f)

LIBFILES := $(wildcard $(LIB)/*.c) $(filter-out $(FSPATH)/fsdata.c, $(wildcard $(LIB)/uip/*.c))
LIBHEADERS := $(wildcard $(LIB)/*.h) $(wildcard $(LIB)/uip/*.h)

PORT := 4290
TTY  := ./tty
PYOCDFLAGS := -t py32f030x4 -f 24m --elf $(BIN)/$(TARGET).elf


# Compiler Flags
CFLAGS  := -g -Os -flto $(CPUARCH) -DF_CPU=$(F_CPU) -I$(SOURCE) -I. -I$(LIB) -I$(LIB)/uip
CFLAGS  += -fdata-sections -ffunction-sections -fno-builtin -fno-common -Wall -D$(MODEL) -Wno-pointer-sign -Wno-unused-label
LDFLAGS := -T$(LDSCRIPT) #-static -lc -lm -nostartfiles -nostdlib -lgcc
LDFLAGS += -Wl,--gc-sections,--build-id=none --specs=nano.specs --specs=nosys.specs -Wl,--print-memory-usage
CFILES  := $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*.S) $(LIBFILES)
HFILES  := $(wildcard ./*.h) $(wildcard $(SOURCE)/*.h) $(LIBHEADERS)

all:	$(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm

$(BIN):
	@mkdir -p $(BIN)

$(BIN)/$(TARGET).elf: $(CFILES) $(HFILES) Makefile $(LDSCRIPT) $(FSPATH)/fsdata.c
	@echo "Building $(BIN)/$(TARGET).elf ..."
	@mkdir -p $(BIN)
	@$(CC) -o $@ $(CFILES) $(CFLAGS) $(LDFLAGS)

$(BIN)/$(TARGET).lst: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).lst ..."
	@$(OBJDUMP) -S $^ > $(BIN)/$(TARGET).lst

$(BIN)/$(TARGET).map: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).map ..."
	@$(OBJDUMP) -t $^ > $(BIN)/$(TARGET).map

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -O binary $< $(BIN)/$(TARGET).bin

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(OBJCOPY) -O ihex $< $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).asm: $(BIN)/$(TARGET).elf
	@echo "Disassembling to $(BIN)/$(TARGET).asm ..."
	@$(OBJDUMP) -d $(BIN)/$(TARGET).elf > $(BIN)/$(TARGET).asm


$(BIN)/$(TARGET)_dump.bin:
	pyocd cmd -t $(MODEL) -f 1m -c reset halt -c savemem 0x08000000 0x6000 $(BIN)/$(TARGET)_dump.bin

elf:	$(BIN)/$(TARGET).elf

bin:	$(BIN)/$(TARGET).bin

hex:	$(BIN)/$(TARGET).hex

asm:	$(BIN)/$(TARGET).asm

dump: $(BIN)/$(TARGET)_dump.bin

$(FSPATH)/fsdata.c: $(FSFILES)
	@echo "Building filesystem ..."
	cd $(FSPATH) && ./makefsdata

flash:	$(BIN)/$(TARGET).bin
	@echo "Uploading to MCU ..."
	@pyocd load -t $(MODEL) -f 1m $(BIN)/$(TARGET).bin

connect:
	@pyocd gdb --persist -S -O semihost_console_type=console $(PYOCDFLAGS)

monitor:
	@$(PREFIX)-gdb $(BIN)/$(TARGET).elf -ex="c" &
	@pyocd gdb --persist -S -O semihost_console_type=console $(PYOCDFLAGS)

serve:
	@$(PREFIX)-gdb $(BIN)/$(TARGET).elf -ex="c" &
	@pyocd gdb --persist -S -O semihost_console_type=telnet -T $(PORT) $(PYOCDFLAGS)

serve-rtt:
	pyocd gdb rtt -O semihost_console_type=telnet -t py32f002bx5 -f 24m

tty:
	socat PTY,link=$(TTY),raw,echo=0,wait-slave TCP:localhost:$(PORT),nodelay

slip:
	sudo ./tools/slip-macos/slip -b 115200 -l 192.168.190.1 -r 192.168.190.2 $(TTY)

debug:
	@$(PREFIX)-gdb $(BIN)/$(TARGET).elf -ex="monitor reset halt"

clean:
	@echo "Cleaning all up ..."
	rm -rf $(BIN)

