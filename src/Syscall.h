
#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <inttypes.h>
        //vector<string> FDT_filename;
        //        //vector<int> FDT_state;//1 = open, 0 = closed
 extern int FileDescriptorIndex;
 extern void initFDT();                      
 extern void closeFDT();
extern void SyscallExe(uint32_t SID); 

#endif



