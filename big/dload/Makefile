OBJDIR = .
SRCDIR = .
INCLDIR = .
OUTDIR = .

## General Flags
PROGRAM = dload
CC = gcc
AS = gcc
LD = gcc
CFLAGS = -Wall -Wextra -g # -fPIC
ASFLAGS = 
LDFLAGS = -lpthread  #-fPIC

## Objects that must be built in order to link
## XXX The order is important, asmstart MUST be the first one !!
OBJECTS = $(OBJDIR)/dload.o          \


SUBDIRS = service
.PHONY : $(SUBDIRS)


## Build both compiler and program
all: $(SUBDIRS) binary

binary: elf

$(SUBDIRS):
	$(MAKE) -C $@

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$*.o $< 

## Compile source files
$(OBJDIR)/%.o : $(SRCDIR)/%.S
	$(AS) $(ASFLAGS) -c -o $(OBJDIR)/$*.o $<

elf: $(OBJECTS)
	$(LD) -o $(OBJDIR)/$(PROGRAM).bin $(OBJECTS) $(LDFLAGS)

clean:
	-rm -rf $(OBJECTS) $(OBJDIR)/$(PROGRAM).bin
	for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done

