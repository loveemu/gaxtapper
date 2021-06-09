TOOLCHAIN := $(DEVKITARM)

export PATH := $(TOOLCHAIN)/bin:$(PATH)
PREFIX := arm-none-eabi-
OBJCOPY := $(PREFIX)objcopy
export AS := $(PREFIX)as
export LD := $(PREFIX)ld

ELF = $(ROM:.gba=.elf)

ASFLAGS := -mcpu=arm7tdmi

ROM := gax2driver.gba

MAKEFLAGS += --no-print-directory

# Clear the default suffixes
.SUFFIXES:
# Don't delete intermediate files
.SECONDARY:
# Delete files that weren't built properly
.DELETE_ON_ERROR:

# Secondary expansion is required for dependency variables in object rules.
.SECONDEXPANSION:

.PHONY: all rom clean

SRCS := $(wildcard *.s)
DEPS := $(wildcard *.inc)
OBJS := $(patsubst %.s,%.o,$(SRCS))

all: rom

rom: $(ROM)

clean:
	rm -f $(ROM)
	rm -f $(ELF)
	rm -f $(OBJS)

%.s: ;

%.o: %.s $(DEPS)
	$(AS) $(ASFLAGS) -o $@ $<

LD_SCRIPT := ld_script.txt

$(ELF): $(LD_SCRIPT) $(OBJS)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) -o $@ $(OBJS)

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
