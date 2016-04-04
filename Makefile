
COMPILER = gcc 

COMPFLAGS = -Wall -g 

SIMPATH = src/

FILELIST = $(SIMPATH)elf_reader/elf_reader.c $(SIMPATH)utils/heap.c $(SIMPATH)RegFile.c $(SIMPATH)Syscall.c $(SIMPATH)PROC.c 

all: 
	$(COMPILER) $(COMPFLAGS) $(FILELIST) -o eMIPS

clean:
	rm -rf RUN

