#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>


#include "utils/heap.h"
#include "utils/utarray.h"
#include "Syscall.h"
#include "RegFile.h"
#include "elf_reader/elf_reader.h"


int  FDT_state[10];
const char *FDT_filename[10];
int FileDescriptorIndex=0;

FILE *stdoutF;
FILE *stderrF;

void initFDT(){
    char *s;
    //1 - open 0 - closed
    s = "stdin"; 
    FDT_filename[FileDescriptorIndex] = s;
    FDT_state[FileDescriptorIndex]    = 0;  
    FileDescriptorIndex++;	

    s = "stdout";  
    FDT_filename[FileDescriptorIndex] = s;
    FDT_state[FileDescriptorIndex] = 0;   
    FileDescriptorIndex++;	

    s = "stderr";  
    FDT_filename[FileDescriptorIndex] = s;
    FDT_state[FileDescriptorIndex] = 0;   
    FileDescriptorIndex++;	
    
    
    stdoutF = fopen("stdout.txt","w");
    stderrF = fopen("stderr.txt","w");

}


void closeFDT() {

fclose(stdoutF);
fclose(stderrF);

}


/*inline*/ int hexCharValue(const char ch){
  if (ch>='0' && ch<='9')return ch-'0';
  if (ch>='a' && ch<='f')return ch-'a'+10;
  return 0;
}

void loadSingleHEX(const char * newValue, int location){

    writeByte((location+0) , ((hexCharValue(newValue[1])) + (hexCharValue(newValue[0])<<4)), false);		//msb
    writeByte((location+1) , ((hexCharValue(newValue[3])) + (hexCharValue(newValue[2])<<4)), false);
    writeByte((location+2) , ((hexCharValue(newValue[5])) + (hexCharValue(newValue[4])<<4)), false);
    writeByte((location+3) , ((hexCharValue(newValue[7])) + (hexCharValue(newValue[6])<<4)), false);		//lsb


}

void sm_uname(int sp){
/*insert into stack...
 * "SescLinux"
 * "sesc"
 * "2.4.18"
 * "#1 SMP Tue Jun 4 16:05:29 CDT 2002"
 * "mips"*/
	printf("running sm_uname\n");
        loadSingleHEX("6d697073",sp +348);
        loadSingleHEX("32000000",sp +316);
        loadSingleHEX("20323030",sp +312);
        loadSingleHEX("20434454",sp +308);
        loadSingleHEX("353a3239",sp +304);
        loadSingleHEX("31363a30",sp +300);
        loadSingleHEX("6e203420",sp +296);
        loadSingleHEX("65204a75",sp +292);
        loadSingleHEX("50205475",sp +288);
        loadSingleHEX("3120534d",sp +284);
        loadSingleHEX("00000023",sp +280);
        loadSingleHEX("342e3138",sp +220);
        loadSingleHEX("0000322e",sp +216);
        loadSingleHEX("63000000",sp +156);
        loadSingleHEX("00736573",sp +152);
        loadSingleHEX("78000000",sp +96);
        loadSingleHEX("4c696e75",sp +92);
        loadSingleHEX("53657363",sp +88);
        printf("exiting sm_uname\n");

}


void fxstat64(int sp)
{
        loadSingleHEX("00000009",sp +32);
        loadSingleHEX("00000000",sp +48);
        loadSingleHEX("00000002",sp +52);
        loadSingleHEX("00002190",sp +56);
        loadSingleHEX("00000001",sp +60);
        loadSingleHEX("00001fb3",sp +64);
        loadSingleHEX("00000005",sp +68);
        loadSingleHEX("00008800",sp +72);
        loadSingleHEX("00000000",sp +88);
        loadSingleHEX("00000000",sp +92);
        loadSingleHEX("00000400",sp +120);
        loadSingleHEX("00000000",sp +128);
        loadSingleHEX("00000000",sp +132);
}


//Syscall Handler 
void SyscallExe(uint32_t SID) {
     printf("Syscall %d Execution \n",SID);

    switch(SID) {
    case 4246 :
    case 4001:{ printf(" ----- Execution Complete -----  \n"); 
                printf("Program Exiting ");
                syscall(SYS_exit, RegFile[4]);
                break;}
    case 4003:{ printf("SYSCALL Read File:\n");//read
                printf("Uninplemented \n");
               break;}
	case 4004:{			//write
            printf("SYSCALL Write File \n"); 
            printf("File Descriptor Index =  %d",RegFile[4]);
            unsigned int k=RegFile[5];						//start at specified element
            unsigned int length=RegFile[6];
            int i = k;
            if (RegFile[4]!=1 && RegFile[4]!=2) {
         
            	FILE *_file;
            	_file = fopen(FDT_filename[RegFile[4]],"a+" );
            	while (length != 0) {
            		length--; 
                        fprintf(_file,"%c",(char)readByte(i,false));
            		i++;
            	}
                fflush(_file);
            	fclose(_file);
            
            }
            else {
            	while (readByte(i,false)!=00) {
            	length--; 
                printf("%c",readByte(i,false));
            		if(RegFile[4]==1) {
            			fprintf(stdoutF,"%c", readByte(i,false));
            		} else {
            			fprintf(stderrF,"%c", readByte(i,false));
            		}
            		i++; if(length == 0)break;
            	}

            	if(RegFile[4]==1) {
            	fflush(stdoutF);
            	} else {
            	fflush(stderrF);
            	} 
            	}
            i++;
            RegFile[2] = i-k-1;
            break;}


    case 4007:{ printf("SYSCALL Write Number to  File \n"); 
                printf("File Descriptor Index =  1");
                fprintf(stdoutF,"%d", RegFile[4]);
            	fflush(stdoutF);
            break;}
    case 4005:{                                         //open file
               printf("SYSCALL File Open \n");
               const char * fIterator = NULL;
               const char * fName = NULL;
               uint8_t x;
               int StrLen = 0;
               int k = RegFile[4];
               x = readByte(k,false);
               while ( x!=0 ) {
                  k++;
                  x = readByte(k,false);
                  StrLen++;
               }

               fName = (char *) malloc(sizeof(char) * StrLen);
               fIterator = (char *) malloc(sizeof(char) * StrLen);
               fName = fIterator;

               k = RegFile[4];
               x = readByte(k,false);
               while ( x!=0 ) {
                 fIterator = memset((void *) fIterator,x,1);
                 fIterator++;
                 k++;
                 x = readByte(k,false);
               }

               printf(" Filename = %s  Index = %d \n",fName,FileDescriptorIndex);
               RegFile[2] = FileDescriptorIndex;

               FILE *_file;
               _file = fopen(fName,"w+");    
               fclose(_file);
               FDT_filename[FileDescriptorIndex] = fName;
               FDT_state[FileDescriptorIndex]    = 1;  
               FileDescriptorIndex++;	
               break;}
  case 4006:{printf("SYSCALL File Close \n");
             printf("File Descriptor Index =  %d",RegFile[4]);
             FDT_state[RegFile[4]]=0;
             RegFile[2] = 0 ;
             break;}  //close file

 
  case 4020:{printf("SYSCALL Getpid \n");
             RegFile[2] = syscall(SYS_getpid);
             break;}
  case 4024:{printf("SYSCALL Getuid \n");
                     RegFile[2] = syscall(SYS_getuid);
                     break;}

  case 4028:{printf("SYSCALL FStat \n");										
             RegFile[4] = RegFile[5];
             RegFile[5] = RegFile[6];
             struct stat buf;
             RegFile[2] = fstat(RegFile[4],&buf);
             fxstat64(RegFile[29]);
             break;}
  case 4047:{printf("SYSCALL Getgid \n");
             RegFile[2] = syscall(SYS_getgid);
             break;}
  case 4049 : {
               printf("SYSCALL Geteuid \n");
               RegFile[2] = syscall(SYS_geteuid);
               printf(" EUID = %x \n",RegFile[2]);
               break;
              }                                  
  case 4050:{printf("SYSCALL Getegid\n");
             RegFile[2] = syscall(SYS_getegid);break;}
  case 4064:{printf("Getppid at time:\n");
             RegFile[2] = syscall(SYS_getppid);break;}
  case 4065:{printf("Getpgrp at time:\n");
             RegFile[2] = syscall(SYS_getpgrp);break;} 
  case 4076:{printf("Getrlimit at time:\n");
             RegFile[2] = syscall(SYS_getrlimit);break;}
  case 4077:{printf("Getrusage at time:\n");
             RegFile[2] = syscall(SYS_getrusage);break;}
  case 4078:{printf("GetTimeofDay at time:\n");
             RegFile[2] = syscall(SYS_gettimeofday,NULL,NULL); break;}
  case 4090:{printf("SYSCALL MMap :\n");
             uint32_t size = RegFile[5]*(1+RegFile[4]);
             if(size < 32) {size = 32;}
             uint32_t ans = mm_malloc(size);
             printf("MMap: %x",ans);
             RegFile[2] = ans;
             break;}
  case 4091:{printf("SYSCALL Munmap ");
             mm_free(RegFile[4]);
             break;}
  case 4122:{
               printf("SYSCALL Uname \n");
               sm_uname(RegFile[29]);
               RegFile[2] = 0;
               break;}
  case 4555:{printf("SYSCALL Malloc  \n");
              int size = RegFile[4];
              if(size < 32){size = 32;}
              uint32_t ans = mm_malloc(size);
              printf("MMap: %x Size: %d \n",ans,size);
              RegFile[2] = ans;
              break;}

  default : printf("Syscall Unimplemented"); break;

}//switch(SID)

}//SyscallExe
                                           



