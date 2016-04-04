
#include <stdio.h>
#include <inttypes.h>
#include "heap.h"
#include "../elf_reader/elf_reader.h"


struct heap_stat *HEAPSTATUS;

 uint32_t HEAP_END;
 uint32_t BLOCKNUM;
 uint32_t current_break;

void initHeap(){
    HEAPSTATUS = NULL;
    HEAP_END=0;
    BLOCKNUM=1;
    current_break = 0;
}

void addHeapStatus(uint32_t ADDR, int STAT,bool DEBUG) {

    struct heap_stat *m;

    HASH_FIND_INT(HEAPSTATUS,&ADDR ,m);
    if(m==NULL) {
        m = (struct heap_stat*)malloc(sizeof(struct heap_stat));    
        m->addr = ADDR;
        m->status = STAT;
        if(DEBUG) printf("hWRITE : Address = %x STAT = %x \n",ADDR,STAT);
        HASH_ADD_INT(HEAPSTATUS,addr,m);
    } else {
        struct heap_stat *dump;
        m = (struct heap_stat*)malloc(sizeof(struct heap_stat));
        m->addr = ADDR;
        m->status = STAT;
        if(DEBUG) printf("hWRITE : Address = %x STAT = %x \n",ADDR,STAT);
        HASH_REPLACE_INT(HEAPSTATUS,addr,m,dump);
        free(dump);
    }

}

int readHeapStatus(uint32_t ADDR,bool DEBUG) {

    struct heap_stat *m;
    int temp = 0;
    HASH_FIND_INT(HEAPSTATUS,&ADDR ,m);
    if(m == NULL) { temp = 0;}
    else          { temp = m->status;}
    if(DEBUG) printf("hREAD : Address = %x STAT = %x \n",ADDR,temp);
    return temp;

}


void heapDump(){
	int heapStart =  exec.HEAPSTART;
        uint32_t i;
	printf("-----Heap Dump------");
	printf("  Heap Start: %d \n",heapStart);
	printf("  Heap Size: %d \n",HEAP_END-heapStart);
	for( i=heapStart; i<=HEAP_END; i++) {
		printf(" %x ",readWord(i,false));
		if(i%2!=0){
			printf("\n");
		}
		else{
			printf("\t");
		}
	}
}

// prep heap blocck


void prepHeapBlock(uint32_t addr, uint32_t size){
     uint32_t i;
	for(i=addr; i<addr+size; i++){
	    addHeapStatus(i,BLOCKNUM,false);		
	}
}



uint32_t mm_malloc(uint32_t size){
	if(size==0){return 0;}
	uint32_t heapStart =  exec.HEAPSTART;
        uint32_t i;
	if(HEAP_END==0){HEAP_END=heapStart;}
	BLOCKNUM++;
	int blockCounter=0;
	for(i=heapStart; i<=HEAP_END+size; i++) {
		
		if (readHeapStatus(i,false)==0) {blockCounter++;}
		else {blockCounter=0;}
	       // printf("blockCounter = %d \n ",blockCounter);
		uint32_t blockStart = i-size+1;
		if (blockCounter>=size && (blockStart%4==0)) {
                        printf("DEBUG : Found Heap Block @ %x\n",blockStart);
		 	prepHeapBlock(blockStart,size);
			if(i>HEAP_END){HEAP_END=i;}
			//memLog("Malloc returned " + num2Str(blockStart));
			return blockStart;
		}
	}
	return 0;
}



void mm_free(uint32_t addr){
	//int heapStart =  exec.HEAPSTART;
	int num = readHeapStatus(addr,false);
        uint32_t i;
	//memLog("Freeing " + num2Str(addr));
	if(addr == 0) {
		return;
	}
	if(num==0){
		//fprintf(stderr,"Freeing unallocated memory at %8x!!!\n",addr);
		printf("Freeing unallocated memory at %8x!!!\n",addr);
		exit(-1);
	}
	for(i=addr; i<=HEAP_END; i++) {			
		if (readHeapStatus(i,false)==num)
			{addHeapStatus(i,0,false);}
		else break;								
	}
}


uint32_t mm_sbrk(int32_t value) {
	if(current_break < exec.BREAKSTART) {
		current_break = exec.BREAKSTART;
	}
	int64_t temp_break = current_break;
	temp_break += value;
	if(temp_break >= exec.BREAKSTART && temp_break < exec.HEAPSTART) {	
		current_break = temp_break;
	}
	return current_break;
}







