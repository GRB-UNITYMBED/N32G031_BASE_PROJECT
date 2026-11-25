######################################
# Toolchain Setup
######################################
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

######################################
# Project Files
######################################
TARGET = My_Project
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

# Compiler Flags
# -g3 keeps macro definitions in the debug info, so the debugger can read
# GPIOB->POD or RCC->APB2PCLKEN by name; with plain -g gdb answers
# 'No symbol "GPIOB" in current context'. CFLAGS_EXTRA is how UnityMbed's
# Debug passes its own flags (-O0 -g3 -ggdb -DDEBUG); it comes last so it wins.
CFLAGS = $(MCU) -O0 -g3 -Wall $(INCLUDES) \
         -ffunction-sections -fdata-sections $(CFLAGS_EXTRA)

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

all: $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).bin

# Rule to create Hex from Elf
$(OUTDIR)/$(TARGET).hex: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(SIZE) $<

# Rule to create Bin from Elf (raw image for SWD flashing via openocd.cfg)
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
