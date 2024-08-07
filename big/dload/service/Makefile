OBJDIR = .
SRCDIR = .
INCLDIR = .
OUTDIR = .

## General Flags
PROGRAM = service
CC = gcc
AS = gcc
LD = gcc
CFLAGS = -Wall -Wextra -g -nostdlib -ffreestanding -nostartfiles -fPIC
ASFLAGS = 
LDFLAGS = -T ls.ld -lpthread -nostdlib -ffreestanding -nostartfiles -fPIC

## Objects that must be built in order to link
## XXX The order is important, asmstart MUST be the first one !!
OBJECTS = $(OBJDIR)/main.o          \


## Build both compiler and program
all: binary

binary: elf
	objcopy -O binary -j .text -j .rodata $(OBJDIR)/$(PROGRAM).elf $(OBJDIR)/$(PROGRAM).bin

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$*.o $< 

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.S
	$(AS) $(ASFLAGS) -c -o $(OBJDIR)/$*.o $<

elf: $(OBJECTS)
	$(LD) -o $(OBJDIR)/$(PROGRAM).elf $(OBJECTS) $(LDFLAGS)

clean:
	-rm -rf $(OBJECTS) $(OBJDIR)/$(PROGRAM).elf $(OBJDIR)/$(PROGRAM).bin

