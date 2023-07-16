OBJDIR = .
SRCDIR = .
INCLDIR = .
OUTDIR = .

## General Flags
PROGRAM = aigame
CC = gcc
AS = gcc
LD = gcc
CFLAGS = -Wall -Wextra -Werror
ASFLAGS = 
LDFLAGS =  

## Objects that must be built in order to link
## XXX The order is important, asmstart MUST be the first one !!
OBJECTS = $(OBJDIR)/game.o          \
          $(OBJDIR)/ai.o            \


## Build both compiler and program
all: binary

binary: elf

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$*.o $< 

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.S
	$(AS) $(ASFLAGS) -c -o $(OBJDIR)/$*.o $<

elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(OBJDIR)/$(PROGRAM).bin $(OBJECTS)

clean:
	-rm -rf $(OBJECTS) $(OBJDIR)/$(PROGRAM).bin

