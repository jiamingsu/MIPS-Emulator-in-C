/*
 * elf_reader.c
 *
 *  Created on: Jul 25, 2014
 *      Author: irichter
 */

#include "elf.h"
#include "common.h"
#include "mips.h"
#include "elf_reader.h"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

#ifndef __APPLE__
#include <byteswap.h>
#else
#define bswap_16(a) __builtin_bswap16(a)
#define bswap_32(a) __builtin_bswap32(a)
#endif


//global variable
struct syscall_addresses syscalls;
struct memElement *MAIN_MEMORY;
struct execinfo exec;

void writefPointer(char const *fName, uint32_t *fAddr,struct Exe_Format  *exFormat   ,bool DEBUG) {
    struct fpointer *m;

    HASH_FIND_STR(exFormat->function_pointers,fName ,m);
    if(m==NULL) {
        m = (struct fpointer*)malloc(sizeof(struct fpointer));
        //printf(" Size of memElement  = %d \n", sizeof(struct memElement));  //shows 64 should be 32 + 8 -> 40
        m->fname = fName;
        m->faddr = fAddr;
        HASH_ADD_KEYPTR(hh,exFormat->function_pointers,m->fname,strlen(m->fname) ,m);
        if(DEBUG) printf("fWRITE : Function = %s Address = %x (%p)\n",fName,*fAddr,fAddr);
    }
    else {
/*
        struct fpointer *dump;
        m = (struct fpointer*)malloc(sizeof(struct fpointer));
        //printf(" Size of memElement  = %d \n", sizeof(struct memElement));  //shows 64 should be 32 + 8 -> 40
        m->fname = fName;
        m->faddr = fAddr;
        if(DEBUG) printf("fWRITE : Function = %s Address = %x \n",fName,fAddr);
        HASH_REPLACE_KEYPTR(hh,MAIN,faddr ,m,dump);
        free(dump);
*/
    printf(" NOT NEEDED ");
    }
} 

struct fpointer * findfPointer(char const * fName, struct Exe_Format *exFormat,bool DEBUG) {
     struct fpointer *m;
     HASH_FIND_STR(exFormat->function_pointers,fName ,m);
     if(DEBUG) {
     if(m!=NULL) printf("fREAD found          = %s \n",fName );  }
     return m;
}

uint32_t * readfPointer(char const * fName, struct Exe_Format *exFormat,bool DEBUG) {

     struct fpointer *m;
     HASH_FIND_STR(exFormat->function_pointers,fName ,m);
     if(m==NULL) { printf("fREAD Could not find = %s ",fName ); return 0; }
     if(DEBUG) printf("fREAD : Function = %s Address = %p \n",m->fname,m->faddr);
     return m->faddr;
 }


void writeByte(uint32_t ADDR, uint8_t DATA,bool DEBUG) {

    struct memElement *m;

    HASH_FIND_INT(MAIN_MEMORY,&ADDR ,m);
    if(m==NULL) {
        m = (struct memElement*)malloc(sizeof(struct memElement));
        //printf(" Size of memElement  = %d \n", sizeof(struct memElement));  //shows 64 should be 32 + 8 -> 40
        m->addr = ADDR;
        m->data = DATA;
        if(DEBUG) printf("WRITE : Address = %x Data = %x \n",ADDR,DATA);
        HASH_ADD_INT(MAIN_MEMORY,addr ,m);
    }
    else {
        struct memElement *dump;
        m = (struct memElement*)malloc(sizeof(struct memElement));
        //printf(" Size of memElement  = %d \n", sizeof(struct memElement));  //shows 64 should be 32 + 8 -> 40
        m->addr = ADDR;
        m->data = DATA;
        if(DEBUG) printf("WRITE : Address = %x Data = %x \n",ADDR,DATA);
        HASH_REPLACE_INT(MAIN_MEMORY,addr ,m,dump);
        free(dump);
    }
} 

uint8_t readByte(uint32_t ADDR,bool DEBUG) {

    struct memElement *m;
    uint8_t temp = 0;
    HASH_FIND_INT(MAIN_MEMORY,&ADDR ,m);
    if(m == NULL) { temp = 0;}
    else          { temp = m->data;}
    if(DEBUG) printf("READ : Address = %x Data = %x \n",ADDR,temp);
    return temp;

} 

//add function to retrive all keys and de-allocate

void writeWord(uint32_t ADDR, uint32_t DATA,bool DEBUG1) {
    uint8_t temp;
    bool DEBUG = DEBUG1;

    if(DEBUG1) printf(" WRITE WORD: Addr = %x Data = %x \n",ADDR,DATA);
    temp = DATA;
    writeByte(ADDR + 3,temp,DEBUG);
    temp = DATA >> 8;
    writeByte(ADDR + 2,temp,DEBUG);
    temp = DATA >> 16;
    writeByte(ADDR + 1,temp,DEBUG);
    temp = DATA >> 24;
    writeByte(ADDR + 0,temp,DEBUG);

}

uint32_t readWord(uint32_t ADDR,bool DEBUG) {
    uint32_t temp;
    bool DEBUG1 = false;
    temp = readByte(ADDR + 0,DEBUG1);
    temp = temp << 8;
    temp = temp | readByte(ADDR + 1,DEBUG1);
    temp = temp << 8;
    temp = temp | readByte(ADDR + 2,DEBUG1);
    temp = temp << 8;
    temp = temp | readByte(ADDR + 3,DEBUG1);
    if(DEBUG) printf("READWD : Addr = 0x%08x Data = 0x%08x \n",ADDR,temp);
    return temp;

}

void init_syscalls() {
        syscalls.CFREE_ADDRESS = 0xFFFFFFF0;
        syscalls.EXIT_ADDRESS = 0xFFFFFFF0;
        syscalls.FXSTAT64_ADDRESS = 0xFFFFFFF0;
        syscalls.GETEGID_ADDRESS = 0xFFFFFFF0;
        syscalls.GETEUID_ADDRESS = 0xFFFFFFF0;
        syscalls.GETGID_ADDRESS = 0xFFFFFFF0;
        syscalls.GETPID_ADDRESS = 0xFFFFFFF0;
        syscalls.GETUID_ADDRESS = 0xFFFFFFF0;
        syscalls.LIBC_MALLOC_ADDRESS = 0xFFFFFFF0;
        syscalls.LIBC_OPEN_ADDRESS = 0xFFFFFFF0;
        syscalls.LIBC_READ_ADDRESS = 0xFFFFFFF0;
        syscalls.LIBC_WRITE_ADDRESS = 0xFFFFFFF0;
        syscalls.MMAP_ADDRESS = 0xFFFFFFF0;
        syscalls.MUNMAP_ADDRESS = 0xFFFFFFF0;
        syscalls.UNAME_ADDRESS = 0xFFFFFFF0;
        syscalls.CXX_EX_AND_ADD_ADDRESS = 0xFFFFFFE0;
        syscalls.CXX_ATOMIC_ADD_ADDRESS = 0xFFFFFFE0;
}




void fill_syscall(uint32_t address, uint16_t call) {
        printf("Writing syscall %hd at address %x\n", call, address);
        bool DEBUG1 = false;
        writeWord(address + 0x0, 0x24020000 | call,DEBUG1);    //li $2, call
        writeWord(address + 0x4, 0xc,DEBUG1);                                  //syscall
        writeWord(address + 0x8, 0x03e00008,DEBUG1);                   //jr $31
        writeWord(address + 0xc, 0x0,DEBUG1);                                  //nop
}

void fill_ex_and_add(uint32_t address) {
        printf("Writing Exchange and Add at address %x\n", address);
        bool DEBUG1 = false;
        writeWord(address + 0x00, 0x8c820000,DEBUG1);  //lw    v0,0(a0)
        writeWord(address + 0x04, 0x00000000,DEBUG1);  //nop
        writeWord(address + 0x08, 0x00a21821,DEBUG1);  //addu  v1,a1,v0
        writeWord(address + 0x0c, 0xac830000,DEBUG1);  //sw    v1,0(a0)
        writeWord(address + 0x10, 0x03e00008,DEBUG1);  //jr    ra
       writeWord(address + 0x14, 0x24030001,DEBUG1);  //li    v1,1
}

void fill_atomic_add(uint32_t address) {
        printf("Writing Atomic Add at address %x\n", address);
        bool DEBUG1 = false;
        writeWord(address + 0x00, 0x8c820000,DEBUG1);  //lw    v0,0(a0)
        writeWord(address + 0x04, 0x00000000,DEBUG1);  //nop
        writeWord(address + 0x08, 0x00a21021,DEBUG1);  //addu  v1,a1,v0
        writeWord(address + 0x0c, 0xac820000,DEBUG1);  //sw    v1,0(a0)
        writeWord(address + 0x10, 0x03e00008,DEBUG1);  //jr    ra
        writeWord(address + 0x14, 0x24020001,DEBUG1);  //li    v0,1
}

void fill_syscall_redirects() {
 bool EMULATE_LLSC = true;
        printf("\n ----- Redirecting Syscalls ----- \n");
        fill_syscall(syscalls.CFREE_ADDRESS, 4091);
        fill_syscall(syscalls.EXIT_ADDRESS, 4001);
        fill_syscall(syscalls.FXSTAT64_ADDRESS, 4028);
        fill_syscall(syscalls.LIBC_MALLOC_ADDRESS, 4555);
        fill_syscall(syscalls.LIBC_OPEN_ADDRESS, 4005);
        fill_syscall(syscalls.LIBC_READ_ADDRESS, 4003);
        fill_syscall(syscalls.LIBC_WRITE_ADDRESS, 4004);
        fill_syscall(syscalls.MMAP_ADDRESS, 4090);
        fill_syscall(syscalls.MUNMAP_ADDRESS, 4091);
        fill_syscall(syscalls.UNAME_ADDRESS, 4122);
        if(EMULATE_LLSC) {
                fill_ex_and_add(syscalls.CXX_EX_AND_ADD_ADDRESS);
                fill_atomic_add(syscalls.CXX_ATOMIC_ADD_ADDRESS);
        }
}


int parse_elf(const char * elf_data, size_t elf_length,struct Exe_Format * exeFormat) {

        /* ELF File Header */
    Elf32_External_Ehdr *ehdr = (Elf32_External_Ehdr*) elf_data;

    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1
            || ehdr->e_ident[EI_MAG2] != ELFMAG2
            || ehdr->e_ident[EI_MAG3] != ELFMAG3) {
      printf("Execution Header Identification Failed \n");
          return -2;
        }

    /* Fail if not 32bit */
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
          printf(" Binary not compiled for 32 bit \n");
        return -3;
    }

    /* Fail if not BigEndian */
    if (ehdr->e_ident[EI_DATA] != ELFDATA2MSB) {
          printf("Binary Architecture not BigEndian \n");
        return -4;
    }

    /* Fail if not ELF version 1 */
    if (ehdr->e_ident[EI_VERSION] != 1) {
          printf("Binary is not an ELF File \n");
        return -5;
    }

    /* Fail if not UNIX System V ABI*/
    if (ehdr->e_ident[EI_OSABI] != ELFOSABI_NONE) {
          printf("Binary not compiled for UNIX system \n");
        return -6;
    }

    /* Fail if not supported architecture (MIPS) */
    if (bswap_16 (ehdr->e_machine) != 8) {
       printf("Binary not compiled for MIPS \n");
            return -7;
        }
//  /* Fail if no valid program headers */
    if (bswap_16 (ehdr->e_phnum) < 1) {
          printf("No program headers found \n");
        return -8;
        }
    /* Fail if reported ELF header size does not match actual
     * ELF header size */
    if (bswap_16 (ehdr->e_ehsize) != sizeof(Elf32_External_Ehdr)) {
      printf("ELF execution header size ismatch \n ");
            return -9;
        }

    /* Fail if reported program header size does not match actual
     * program header size */
    if (bswap_16 (ehdr->e_phentsize) != sizeof(Elf32_External_Phdr)) {
      printf("ELF program header size ismatch \n ");
        return -10;
        }
    exeFormat->maxUsedAddr = 0;
    exeFormat->numSegments = 0;

    exeFormat->entryAddr = bswap_32(ehdr->e_entry);
    uint16_t numSegments = bswap_16(ehdr->e_phnum);

    //printf("Virtual entry point for binary  = %x \n",exeFormat->entryAddr);
    //printf("Number of readable Segments = %d \n",numSegments);
    //printf("Program header entry size = %d (bytes) \n\n",bswap_16(ehdr->e_phentsize));
    
    //Evaluate Program headers to extract segment information.
    Elf32_External_Phdr *phdr = (Elf32_External_Phdr *) (elf_data + bswap_32(ehdr->e_phoff));

    int i;
    for (i = 0; i < numSegments; i++) {
        uint32_t offset = bswap_32(phdr->p_offset);
        uint32_t seg_type = bswap_32(phdr->p_type);
                //printf("Program header pointer = %x \n",phdr);
        switch (seg_type) {
        case PT_MIPS_REGINFO:
            exeFormat->globalPointer = bswap_32(((Elf32_External_RegInfo*) (elf_data + offset))->ri_gp_value);
                  //      printf(" globalPointer = %x \n",exeFormat->globalPointer);
            //No break (we do allocate it)
        case PT_NOTE:
        case PT_LOAD: {
                        exeFormat->segmentList[exeFormat->numSegments].type         = seg_type;
                        exeFormat->segmentList[exeFormat->numSegments].offsetInFile = offset;
                        exeFormat->segmentList[exeFormat->numSegments].startAddress = bswap_32(phdr->p_vaddr);
                        exeFormat->segmentList[exeFormat->numSegments].lengthInFile = bswap_32(phdr->p_filesz);
                        exeFormat->segmentList[exeFormat->numSegments].sizeInMemory = bswap_32(phdr->p_memsz);
                        exeFormat->segmentList[exeFormat->numSegments].protFlags    = bswap_16(phdr->p_flags);   
                        //printf(" Segment type = %x \n",seg_type);
                        //printf(" Segment offset = %x \n",offset);
                        //printf(" Segment virtual address = %x \n",bswap_32(phdr->p_vaddr));
                        //printf(" Segment Length (bytes) in file   image = %d \n",bswap_32(phdr->p_filesz));
                        //printf(" Segment Length (bytes) in memory image = %d \n",bswap_32(phdr->p_memsz));
                        //printf(" Segment flags = %x \n",bswap_32(phdr->p_flags));

            uint32_t seg_end = exeFormat->segmentList[exeFormat->numSegments].startAddress + exeFormat->segmentList[exeFormat->numSegments].sizeInMemory;
                        exeFormat->numSegments++;
            if(seg_end < exeFormat->maxUsedAddr) {
                exeFormat->maxUsedAddr = seg_end;
            }
        }
            break;
        default:
            printf("Segment not required \n");
            //Don't bother loading it -- we don't need it.
            //Only known section that would fit this is PAX_FLAGS
            break;
        }
        phdr++;
    }

    //Obtain largest allocated memory location:
    uint16_t shnum = bswap_16(ehdr->e_shnum);
        /* Section header */
    Elf32_External_Shdr *base_shdr = (Elf32_External_Shdr*) (elf_data + bswap_32(ehdr->e_shoff));
    for(i = 0; i < shnum; i++) {
        uint32_t sh_flags = bswap_32(base_shdr[i].sh_flags);
        if(sh_flags & SHF_ALLOC) {
            uint32_t seg_end = bswap_32(base_shdr[i].sh_addr) + bswap_32(base_shdr[i].sh_entsize);
            if(seg_end < exeFormat->maxUsedAddr) {
                exeFormat->maxUsedAddr = seg_end;
            }
        }
    }


    exeFormat->function_pointers = NULL;

char const * temp1;

temp1 = "__libc_open";   writefPointer(temp1, &syscalls.LIBC_OPEN_ADDRESS,exeFormat,false  );
temp1 = "__libc_read";   writefPointer(temp1, &syscalls.LIBC_READ_ADDRESS,exeFormat,false  );
temp1 = "_exit";         writefPointer(temp1, &syscalls.EXIT_ADDRESS,exeFormat,false  );
temp1 = "__munmap";      writefPointer(temp1, &syscalls.MUNMAP_ADDRESS,exeFormat,false  );
temp1 = "__geteuid";     writefPointer(temp1, &syscalls.GETEUID_ADDRESS,exeFormat,false  );
temp1 = "__getuid";      writefPointer(temp1, &syscalls.GETUID_ADDRESS,exeFormat,false  );
temp1 = "__uname";       writefPointer(temp1, &syscalls.UNAME_ADDRESS,exeFormat,false  );
temp1 = "__getpid";      writefPointer(temp1, &syscalls.GETPID_ADDRESS,exeFormat,false  );
temp1 = "__getgid";      writefPointer(temp1, &syscalls.GETGID_ADDRESS,exeFormat,false );
temp1 = "__getegid";     writefPointer(temp1, &syscalls.GETEGID_ADDRESS,exeFormat,false  );
temp1 = "__libc_malloc"; writefPointer(temp1, &syscalls.LIBC_MALLOC_ADDRESS,exeFormat,false  );
temp1 = "__cfree";       writefPointer(temp1, &syscalls.CFREE_ADDRESS,exeFormat,false  );
temp1 = "__fxstat64";    writefPointer(temp1, &syscalls.FXSTAT64_ADDRESS,exeFormat,false  );
temp1 = "__mmap";        writefPointer(temp1, &syscalls.MMAP_ADDRESS,exeFormat,false  );
temp1 = "__libc_write";  writefPointer(temp1, &syscalls.LIBC_WRITE_ADDRESS,exeFormat,false  );
temp1 = "_ZN9__gnu_cxx18__exchange_and_addEPVii"; writefPointer(temp1, &syscalls.CXX_EX_AND_ADD_ADDRESS,exeFormat,false);
temp1 = "_ZN9__gnu_cxx12__atomic_addEPVii";       writefPointer(temp1, &syscalls.CXX_ATOMIC_ADD_ADDRESS,exeFormat,false );


    //Try to get string tables to re route syscalls
    uint16_t shstrndx = bswap_16(ehdr->e_shstrndx);
    if (shstrndx && shnum) {
        Elf32_External_Shdr *shstrhdr = base_shdr + shstrndx;
        char * shstrtbl = (char*) (elf_data + bswap_32(shstrhdr->sh_offset));
        Elf32_External_Shdr *symtabhdr = NULL;
        Elf32_External_Shdr *strtabhdr = NULL;
        for (i = 0; i < shnum; i++) {
            uint32_t sh_name = bswap_32(base_shdr[i].sh_name);
            if (sh_name) {
                char * shname = shstrtbl + sh_name;
                switch (bswap_32(base_shdr[i].sh_type)) {
                case SHT_SYMTAB:
                    if(!strcmp(shname, ".symtab")) {
                        symtabhdr = base_shdr + i;
                    }
                    break;
                case SHT_STRTAB:
                    if(!strcmp(shname, ".strtab")) {
                        strtabhdr = base_shdr + i;
                    }
                    break;
                }
            }
        }

        if(symtabhdr && strtabhdr) {
            Elf32_External_Sym *sym_base = (Elf32_External_Sym*)(elf_data + bswap_32(symtabhdr->sh_offset));
            uint32_t sym_count = bswap_32(symtabhdr->sh_size) / bswap_32(symtabhdr->sh_entsize);
            const char *str_base = (elf_data + bswap_32(strtabhdr->sh_offset));
            char const *name; 
            uint32_t faddr; 
            for(i = 0; i < sym_count; i++) {
                if (ELF_ST_TYPE(sym_base[i].st_info) == STT_FUNC) {
                    name = (char *)(str_base + bswap_32(sym_base[i].st_name));
                    faddr =  bswap_32(sym_base[i].st_value);

                    struct fpointer *status =  findfPointer(name, exeFormat,false) ;
                    if( status != NULL ) {
                     //printf(" MUST WRITE = %x ",*status->faddr);
                     //printf(" with = %x \n", faddr);
                     *status->faddr = faddr; 


                     }
                }
            }
        }
    }



    return 0;

}


int LoadOSMemory(const char * file_name) {
    int elf_fd = open(file_name, 0);
    if (elf_fd == -1) {
        printf("Unable to open Binary");
        return -1;
    }
        //Obtain symoblic link details
    struct stat file_stat;
    if (lstat(file_name, &file_stat)) {
        printf("Unable to read Binary");
        return -2;
    }
        //Allocate memory for elf_file data
    char * elf_data = (char*) mmap(NULL, file_stat.st_size, PROT_READ,MAP_PRIVATE, elf_fd, 0);
    if (elf_data == MAP_FAILED) {
        printf("Unable to allocated required memory");
        return -3;
    }
    Exe_Format exeFormat;
    MAIN_MEMORY=NULL;
        init_syscalls();
      int rv = parse_elf(elf_data, file_stat.st_size, &exeFormat);
         if(rv) {
       munmap(elf_data, file_stat.st_size);
                close(elf_fd);
                printf("\nERROR READING ELF!!!! (%d)\n", rv);
                return rv;
        }

        puts("\n-----ELF SUMMARY------\n");
        printf("Number of required segments %d\n",exeFormat.numSegments);

        int maxAddr = 0;
        int i;
        for(i =0; i<exeFormat.numSegments; i++){
                //read section into memory
                // j = offset from start
        printf("--- Segment %d \n",i);
        printf("    Type %x\n", exeFormat.segmentList[i].type);
        printf("    Virtual Start Address 0x%08x\n",exeFormat.segmentList[i].startAddress);
        printf("    Length in file %d (bytes)\n\n",exeFormat.segmentList[i].lengthInFile);
                int j;
                for(j = 0; j<exeFormat.segmentList[i].lengthInFile; j++){
                       writeByte( j+exeFormat.segmentList[i].startAddress, elf_data[j+exeFormat.segmentList[i].offsetInFile],false);
                }
                if((exeFormat.segmentList[i].lengthInFile+exeFormat.segmentList[i].startAddress)>maxAddr){
                        maxAddr = exeFormat.segmentList[i].lengthInFile+exeFormat.segmentList[i].startAddress;
                }
        //printf("maxAddr = %x \n",maxAddr);
        exeFormat.maxUsedAddr = maxAddr - 1;
        }  
        //store exec offsets -----------------------
        exec.GPC_START = exeFormat.entryAddr;

        //set heap beyond the scope of our addressing, and align to a page.
        exec.BREAKSTART = 0x80000000;//(exeFormat.maxUsedAddr + ((exeFormat.maxUsedAddr & 0xFFF)?0x1000:0)) & ~0xFFF;
        exec.HEAPSTART = 0xC0000000;//0xEE036000;//exec.BREAKSTART + MAX_BREAK_SIZE;

        //not sure yet how to get these from ELF
        exec.GSP = 0xf7021fc0;//for noio
        exec.GRA = 0x1006a244;//for noio, but we don't really need it
        exec.GP = exeFormat.globalPointer;
 
        fill_syscall_redirects();
        munmap(elf_data, file_stat.st_size);
        close(elf_fd);

        return 1;

}

void CleanUp() {
   struct memElement *s, *tmp;
   HASH_ITER(hh,MAIN_MEMORY, s, tmp) { free(s); }
   printf("Clean Up Complete \n");
}

