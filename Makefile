######################################
# Toolchain Setup
######################################
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

######################################
# Project Files
######################################
# The UnityMbed IDE builds with `make -j PROJECT=<name>` and then looks for
# build/<name>.{elf,bin,hex} when it flashes or starts a debug session, so
# TARGET has to follow PROJECT rather than being fixed. `?=` keeps the plain
# `make` build working outside the IDE.
PROJECT ?= My_Project
TARGET = $(PROJECT)
OUTDIR = build

# Source Files
SRCS  = $(wildcard src/*.c) \
        $(wildcard drivers/src/*.c) \
        startup/startup_n32g031_gcc.s

# Include Paths (Update these to match your folder structure)
INCLUDES = -I. -Iinc -IN32_SDK -ICMSIS/Core/Include -Idrivers/inc -IN32G031_StdPeriph_Driver/inc

######################################
# Flags
######################################
# MCU Specific Flags
MCU = -mcpu=cortex-m0 -mthumb

# Compiler Flags. CFLAGS_EXTRA is how the IDE turns on debug-grade output
# (`-O0 -g3 -ggdb -DDEBUG`) before starting a gdb session; without the hook
# those flags are silently dropped.
CFLAGS = $(MCU) -O0 -g -Wall $(INCLUDES) $(CFLAGS_EXTRA) \
         -ffunction-sections -fdata-sections

# Linker Flags
LDSCRIPT = n32g031_flash.ld
LDFLAGS = $(MCU) -T$(LDSCRIPT) \
          -Wl,--gc-sections \
          --specs=nosys.specs \
          -Wl,-Map=$(OUTDIR)/$(TARGET).map

######################################
# Build Rules
######################################

# Convert source list to object list in build directory
OBJS = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(basename $(SRCS))))

all: $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex

# Rule to create Hex from Elf
$(OUTDIR)/$(TARGET).hex: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(SIZE) $<

# Rule to create raw Bin from Elf. The IDE names the image it flashes
# build/<name>.bin — n32g03x.cfg's n32_program rewrites that to .hex, but the
# FPEC openocd.cfg used to work around the flaky Nations driver reads the raw
# .bin, so both have to exist.
$(OUTDIR)/$(TARGET).bin: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Rule to Link Elf
$(OUTDIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Rule for C files
$(OUTDIR)/%.o: %.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for Assembly files
$(OUTDIR)/%.o: %.s
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR):
	mkdir -p $(OUTDIR)

######################################
# Flash and Debug
######################################
OPENOCD_BIN = /C/openocd-v0.12/bin/openocd.exe

flash: all
	$(OPENOCD_BIN) \
	-f interface/cmsis-dap.cfg \
	-f target/n32g03x.cfg \
	-c "adapter speed 1000" \
	-c "reset_config none" \
	-c "init" \
	-c "halt" \
	-c "flash write_image erase $(OUTDIR)/$(TARGET).elf" \
	-c "verify_image $(OUTDIR)/$(TARGET).elf" \
	-c "reset run" \
	-c "exit"

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean flash
