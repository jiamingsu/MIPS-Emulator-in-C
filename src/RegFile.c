

#include <stdio.h>
#include <inttypes.h>
#include "RegFile.h"

// 32x32 Register File
int32_t RegFile[NUMBER_OF_REGS];

void initRegFile(int32_t val) {
    int i;
    for(i=0;i<34;i++) {
    RegFile[i] = val;
    }
}

void printRegFile() {
    printf("\n ----- REG DUMP ----- \n");
    int j;
    for ( j=0; j < NUMBER_OF_REGS; j++) {
        printf("REG[%2d]: 0x%08x (%d)",j,RegFile[j],RegFile[j]);
        if(j%2==0){printf("\t\t");}
        else{printf("\n");}
        }
}

int32_t getRegister(int number){
	if(number == 0)
		return 0;
	else if(number > 0 && number < 32)
		return RegFile[number];
	else
		return 0;
}

int32_t setRegister(int number, int32_t value){
	if(number == 0)
		return 0;
	else if(number > 0 && number < 32){
		RegFile[number] = value;
		return RegFile[number];
	}
	else
		return 0;
}

//hi RegFile[32]
//lo RegFile[33]
int32_t getHiRegister(){
	return RegFile[32];
}

int32_t setHiRegister(int32_t value){
	RegFile[32] = value;
	return RegFile[32];
}

int32_t getLoRegister(){
	return RegFile[33];
}

int32_t setLoRegister(int32_t value){
	RegFile[33] = value;
	return RegFile[33];
}
