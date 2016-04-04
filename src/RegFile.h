

#ifndef   REG_FILE_H_  
#define  REG_FILE_H_

#define NUMBER_OF_REGS 34
//hi RegFile[32]
//lo RegFile[33]

extern int32_t RegFile[NUMBER_OF_REGS];
extern void initRegFile(int32_t val);
extern void printRegFile();
extern int32_t getRegister(int number);
extern int32_t setRegister(int number, int32_t value);
extern int32_t getHiRegister();
extern int32_t setHiRegister(int32_t value);
extern int32_t getLoRegister();
extern int32_t setLoRegister(int32_t value);


#endif


