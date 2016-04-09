#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RegFile.h"
#include "Syscall.h"
#include "utils/heap.h"
#include "elf_reader/elf_reader.h"

//Stats

uint32_t DynInstCount = 0;

enum Opcode {
    SPECIAL =   0b000000,//NOT TESTED
    REGIMM  =   0b000001,//NOT TESTED
    J       =   0b000010,//NOT TESTED
    JAL     =   0b000011,//NOT TESTED
    BEQ     =   0b000100,//
    BNE     =   0b000101,/**/
    BLEZ    =   0b000110,/**/
    BGTZ    =   0b000111,/**/

    ADDI    =   0b001000,//
    ADDIU   =   0b001001,//
    SLTI    =   0b001010,/**/
    SLTIU   =   0b001011,/**/
    ANDI    =   0b001100,/**/
    ORI     =   0b001101,//
    XORI    =   0b001110,/**/
    LUI     =   0b001111,//

    BEQL    =   0b010100,/**/
    BNEL    =   0b010101,/**/
    BLEZL   =   0b010110,/**/

    LB      =   0b100000,//
    LH      =   0b100001,
    LWL     =   0b100010,
    LW      =   0b100011,//
    LBU     =   0b100100,
    LHU     =   0b100101,
    LWR     =   0b100110,

    SB      =   0b101000,//NOT TESTED
    SH      =   0b101001,
    SWL     =   0b101010,
    SW      =   0b101011,//NOT TESTED
    SWR     =   0b101110
};

enum Special_Func {
    SLL     =   0b000000, // NOP
    SRL     =   0b000010,//
    SRA     =   0b000011,//NOT TESTED
    SLLV    =   0b000100,//NOT TESTED
    SRLV    =   0b000110,//NOT TESTED
    SRAV    =   0b000111,//NOT TESTED

    JR      =   0b001000,//
    JALR    =   0b001001,//

    SYSCALL =   0b001100,//

    MFHI    =   0b010000,//
    MTHI    =   0b010001,//
    MFLO    =   0b010010,//
    MTLO    =   0b010011,//

    MULT    =   0b011000,//
    MULTU   =   0b011001,//NOT TESTED
    DIV     =   0b011010,//
    DIVU    =   0b011011,//NOT TESTED

    ADD     =   0b100000,//
    ADDU    =   0b100001,//
    SUB     =   0b100010,//
    SUBU    =   0b100011,//NOT TESTED
    AND     =   0b100100,//NOT TESTED
    OR      =   0b100101,//NOT TESTED
    XOR     =   0b100110,//
    NOR     =   0b100111,//NOT TESTED

    SLT     =   0b101010,//NOT TESTED
    SLTU    =   0b101011//NOT TESTED
};


enum Regimm_Rt {
    BLTZ    =   0b00000,//NOT TESTED
    BGEZ    =   0b00001,//NOT TESTED
    BLTZAL  =   0b10000,//NOT TESTED
    BGEZAL  =   0b10001//
};



void write_initialization_vector(uint32_t sp, uint32_t gp, uint32_t start) {
        printf("\n ----- BOOT Sequence ----- \n");
        printf("Initializing sp=0x%08x; gp=0x%08x; start=0x%08x\n", sp, gp, start);
        RegFile[28] = gp;
        RegFile[29] = sp;
        RegFile[31] = start;
        printRegFile();

    }


int main(int argc, char * argv[]) {
  
    int MaxInst = 0;
    int status = 0;
    uint32_t i; 
    uint32_t PC,newPC;
    uint32_t branch = 0;
    uint32_t CurrentInstruction;

    uint32_t opcode;//31 30 29 28 27 26
    uint32_t rs;//base 25 24 23 22 21
    uint32_t rt;//20 19 18 17 16
    uint32_t rd;//15 14 13 12 11
    uint32_t shamt;//hint, sa 10 9 8 7 6
    uint32_t func;// 5 4 3 2 1 0
    uint32_t immediate;//offset 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
    uint32_t instr_index;//25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
    //uint32_t r0;//6

    if (argc < 2) {
      printf("Input argument missing \n");
      return -1;
    }
    sscanf (argv[2], "%d", &MaxInst);

    //Open file pointers & initialize Heap & Regsiters
    initHeap();
    initFDT();
    initRegFile(0);
    
    //Load required code portions into Emulator Memory
    status =  LoadOSMemory(argv[1]);
    if(status <0) { return status; }
    
    //set Global & Stack Pointers for the Emulator
    // & provide startAddress of Program in Memory to Processor
    write_initialization_vector(exec.GSP, exec.GP, exec.GPC_START);

    printf("\n ----- Execute Program ----- \n");
    printf("Max Instruction to run = %d \n",MaxInst);
    PC = exec.GPC_START;
    for(i=0; i<MaxInst ; i++) {
        printf("process instruction[%d]\n",i);
        DynInstCount++;
        if(branch == 1)
            PC = newPC;
        CurrentInstruction = readWord(PC,false);  
        printRegFile();
        /********************************/
        //Add your implementation here
        /********************************/
        opcode  =   0xfc000000;//31 30 29 28 27 26
        rs      =   0x03e00000;//base 25 24 23 22 21
        rt      =   0x001f0000;//20 19 18 17 16
        rd      =   0x0000f800;//15 14 13 12 11
        shamt   =   0x000007c0;//hint, sa 10 9 8 7 6
        func    =   0x0000003f;// 5 4 3 2 1 0
        immediate = 0x0000ffff;//offset 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        instr_index = 0x03ffffff;//25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        //r0 = 0x00000040;//6

        opcode &= CurrentInstruction;
        opcode >>= 26;//shift right logical

        rs &= CurrentInstruction;
        rs >>= 21;

        rt &= CurrentInstruction;
        rt >>= 16;

        rd &= CurrentInstruction;
        rd >>= 11;

        shamt &= CurrentInstruction;
        shamt >>= 6;

        func &= CurrentInstruction;
        immediate &= CurrentInstruction;
        instr_index &= CurrentInstruction;

        //r0 &= CurrentInstruction;
        //r0 >>= 6;
        if(opcode == SPECIAL){
            if(func == ADD){
                //rd ← rs + rt
                setRegister((int)rd,
                            getRegister((int)rs) + getRegister((int)rt));
            }
            else if(func == ADDU){
                //rd ← rs + rt
                setRegister((int)rd,
                            (int32_t)((uint32_t)getRegister((int)rs) + (uint32_t)getRegister((int)rt)));
            }
            else if(func == SUB){
                //rd ← rs - rt
                setRegister((int)rd,
                            getRegister((int)rs) - getRegister((int)rt));
            }
            else if(func == SUBU){
                //rd ← rs - rt
                uint32_t temp = (uint32_t)getRegister((int)rs);
                uint32_t temp_1 = (uint32_t)getRegister((int)rt);
                temp_1 = ~temp_1;
                temp_1 += 1;
                setRegister((int)rd,
                            temp + temp_1);
            }
            else if(func == AND){
                //rd ← rs AND rt
                setRegister((int)rd,
                            getRegister((int)rs) & getRegister((int)rt));
            }
            else if(func == OR){
                //rd ← rs OR rt
                setRegister((int)rd,
                            getRegister((int)rs) | getRegister((int)rt));
            }
            else if(func == JALR){
                //rd ← return_addr, PC ← rs
                //The return link is the address of the second instruction following the branch,
                //where execution continues after a procedure call.

                //I: temp← GPR[rs]
                //GPR[rd] ← PC + 8
                setRegister((int)rd,
                            (int32_t)PC + 8);
                //I+1:if Config1CA = 0
                //then PC ← temp
                //else
                //PC ← tempGPRLEN-1..1 || 0
                //ISAMode ← temp0 endif

                //Jump to the effective target address in GPR rs.
                //Execute the instruction that follows the jump,in the branch delay
                //slot, before executing the jump itself.
                newPC = getRegister((int)rs);
                branch = 0b100;

            }
            else if(func == MULT){
                //(HI, LO) ← rs × rt
                int64_t multiplicand = (int64_t)getRegister((int)rs);
                int64_t multiplier = (int64_t)getRegister((int)rt);
                int64_t product = multiplicand * multiplier;
                int32_t lo = (int32_t)(product & 0x00000000FFFFFFFF);
                int32_t hi = (int32_t)((product >>32) & 0x00000000FFFFFFFF);
                setLoRegister(lo);
                setHiRegister(hi);
            }
            else if(func == MULTU){
                //(HI, LO) ← rs × rt
                //treating both operands as unsigned val- ues, to produce a 64-bit result
                //
                //prod← (0 || GPR[rs]31..0) × (0 || GPR[rt]31..0)
                uint64_t multiplicand = (uint64_t)getRegister((int)rs);
                uint64_t multiplier = (uint64_t)getRegister((int)rt);
                uint64_t product = multiplicand * multiplier;
                uint32_t lo = (uint32_t)(product & 0x00000000FFFFFFFF);
                uint32_t hi = (uint32_t)((product>>32) & 0x00000000FFFFFFFF);
                setLoRegister(lo);
                setHiRegister(hi);
                //LO ← prod31..0
                //HI ← prod63..32
            }
            else if (func == MFLO){
                //rd ← LO
                setRegister((int)rd,
                            getLoRegister());
            }
            else if(func == MTLO){
                //LO ← rs
                setLoRegister(getRegister((int)rs));
            }
            else if(func == XOR){
                //rd ← rs XOR rt
                setRegister((int)rd,
                            getRegister((int)rs) ^ getRegister((int)rt));
            }
            else if(func == NOR){
                //rd ← rs NOR rt
                setRegister((int)rd,
                            ~(getRegister((int)rs) | getRegister((int)rt)));
            }
            else if(func == SLL){
                //rd ← rt << sa
                //NOP SLL r0, r0, 0
                int sa = shamt;
                setRegister((int)rd,
                            getRegister((int)rt) << sa);
            }
            else if(func == SLLV){
                //rd ← rt << rs
                //s ← GPR[rs]4..0 s
                int32_t s = getRegister((int)rs);
                s &= 0x0000001F;//specified by the low-order 5 bits of GPR rs. 5 bit 0 to 31
                setRegister((int)rd,
                            getRegister((int)rt) << s);
                //temp ← GPR[rt](31-s)..0 || 0
                //GPR[rd]← temp

            }
            else if(func == SRL){
                //rd ← rt >> sa (logical)
                int sa = shamt;
                uint32_t temp = (uint32_t)getRegister((int)rt);
                setRegister((int)rd,
                            temp >> sa);
            }
            else if(func == SRLV){
                //rd ← rt >> rs (logical)

                //The contents of the low-order 32-bit word of GPR rt are shifted right, inserting zeros into the emptied bits; the word
                //result is placed in GPR rd. The bit-shift amount is specified by the low-order 5 bits of GPR rs.
                //s ← GPR[rs]4..0
                int32_t s = getRegister((int)rs);
                s &= 0x0000001F;


                //temp ← 0s || GPR[rt]31..s
                uint32_t temp = (uint32_t)getRegister((int)rt);
                temp >>= s;
                //GPR[rd]← temp
                setRegister((int)rd,
                            (int32_t)temp);
            }
            else if(func == SRA){
                //rd ← rt >> sa
                int sa = shamt;
                int32_t temp = getRegister((int)rt);
                setRegister((int)rd,
                            temp >> sa);
            }
            else if(func == SRAV){
                //rd ← rt >> rs (arithmetic)
                //s ← GPR[rs]4..0s
                int32_t s = getRegister((int)rs);
                s &= 0x0000001F;

                //temp ← (GPR[rt]31) || GPR[rt]31..s
                int32_t temp = getRegister((int)rt);
                //GPR[rd]← temp
                setRegister((int)rd,
                            temp >> s);

            }
            else if(func == DIV){
                //(HI, LO) ← rs / rt
                int32_t quotient = getRegister((int)rs) / getRegister((int)rt);
                int32_t remainder = getRegister((int)rs) % getRegister((int)rt);
                setLoRegister(quotient);
                setHiRegister(remainder);
            }
            else if(func == DIVU){
                //(HI, LO) ← rs / rt
                //treating both operands as unsigned values
                uint32_t quotient = (uint32_t)getRegister((int)rs) / (uint32_t)getRegister((int)rt);
                uint32_t remainder = (uint32_t)getRegister((int)rs) % (uint32_t)getRegister((int)rt);
                setLoRegister(quotient);
                setHiRegister(remainder);
            }
            else if(func == MFHI){
                //rd ← HI
                setRegister((int)rd,
                            getHiRegister());
            }
            else if(func == MTHI){
                //HI ← rs
                setHiRegister(getRegister((int)rs));
            }

            else if(func == JR){
                //PC ← rs
                //PC = (uint32_t)getRegister((int)rs);//branch delay slot fail
                //continue;
                newPC = (uint32_t)getRegister((int)rs);
                branch = 0b100;
            }
            else if(func == SLT){
                //rd ← (rs < rt)
                if(getRegister((int)rs) < getRegister((int)rt))
                    setRegister((int)rd, 1);
                else
                    setRegister((int)rd, 0);
            }
            else if(func == SLTU){
                //rd ← (rs < rt)
                if((uint32_t)getRegister((int)rs) < (uint32_t)getRegister((int)rt))
                    setRegister((int)rd, 1);
                else
                    setRegister((int)rd, 0);
            }
            else if(func == SYSCALL){
                uint32_t SID =(uint32_t)getRegister(2);//$v0
                SyscallExe(SID);

            }
        }
        else if(opcode == REGIMM){
            if(rt == BLTZ){
                //if rs < 0 then branch
                //I: target_offset ← sign_extend(offset || 02)
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;

                //condition ← GPR[rs] < 0GPRLEN
                bool condition = getRegister((int)rs) < 0;
                //I+1: if condition then
                if(condition){
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                //PC ← PC + target_offset
                //endif
            }
            else if(rt == BGEZ){
                //if rs ≥ 0 then branch
                //I: target_offset ← sign_extend(offset || 02)
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;

                //condition ← GPR[rs] ≥ 0GPRLEN
                bool condition = getRegister((int)rs) >= 0;
                //I+1: if condition then
                if(condition){
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                //PC ← PC + target_offset
                //endif
            }
            else if(rt == BLTZAL){
                //if rs < 0 then procedure_call
                //I: target_offset ← sign_extend(offset || 02)
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                //condition ← GPR[rs] < 0GPRLEN
                bool condition = (getRegister((int)rs)) < 0;
                //GPR[31] ← PC + 8
                setRegister(31,
                            (int32_t)PC + 8);
                //I+1: if condition then
                if(condition){
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                //PC ← PC + target_offset
                //endif
            }
            else if(rt == BGEZAL){
                //if rs ≥ 0 then procedure_call
                //I: target_offset ← sign_extend(offset || 02)
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;

                //condition ← GPR[rs] ≥ 0GPRLEN
                bool condition = (getRegister((int)rs)) >= 0x00000000;

                //GPR[31] ← PC + 8
                setRegister(31,
                            (int32_t)PC + 8);

                //I+1: if condition then
                if(condition){
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                //PC ← PC + target_offset
                //endif
                //An 18-bit signed offset (the 16-bit offset field shifted left 2 bits)
                //is added to the address of the instruction following the branch (not the branch itself),
                //in the branch delay slot, to form a PC-relative effective target address.
            }
        }
        else {
            if(opcode == J){
                //Jump to the effective target address.
                //Execute the instruction that follows the jump,
                //in the branch delay slot, before executing the jump itself.
                //I:
                //I+1:PC ← PCGPRLEN-1..28 || instr_index || 02
                newPC = ((PC + 4) & 0xf0000000) | (instr_index << 2);
                branch = 0b100;
            }
            else if(opcode == JAL){
                //I: GPR[31]← PC + 8
                setRegister(31,
                            (int32_t)PC + 8);
                //I+1:PC ← PCGPRLEN-1..28 || instr_index || 02
                newPC = ((PC + 4) & 0xf0000000) | (instr_index << 2);
                branch = 0b100;
            }
            else if(opcode == ADDI){
                //rt ← rs + immediate
                int32_t sign_extend = immediate << 16;
                sign_extend >>= 16;
                setRegister((int)rt,
                            getRegister((int)rs) + sign_extend);
            }
            else if(opcode == ADDIU){
                int32_t sign_extend = immediate << 16;
                sign_extend >>= 16;
                setRegister((int)rt,
                            getRegister((int)rs) + sign_extend);
            }
            else if(opcode == SLTI){
                //rt ← (rs < immediate)
                int32_t sign_extend = immediate << 16;
                sign_extend >>= 16;
                if(getRegister((int)rs) < sign_extend)
                    setRegister((int)rt, 1);
                else
                    setRegister((int)rt, 0);
            }
            else if(opcode == SLTIU){
                //rt ← (rs < immediate)
                //uint32_t sign_extend = immediate << 16;
                //sign_extend >>= 16;
                if(((uint32_t)getRegister((int)rs)) < immediate)
                    setRegister((int)rt, 1);
                else
                    setRegister((int)rt, 0);
            }
            else if(opcode == BEQ){
                //if rs = rt then branch
                //I: target_offset ← sign_extend(offset || 02)
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;

                //condition ← (GPR[rs] = GPR[rt])
                bool condition = (getRegister((int)rs) == getRegister((int)rt));

                //I+1: if condition then
                if(condition){
                    //PC += target_offset;
                    //continue;//branch delay slot fail
                    newPC = PC + 4 +  target_offset;
                    branch = 0b100;
                }
                //PC ← PC + target_offset
                //endif
            }
            else if(opcode == BEQL){
                //if rs = rt then branch_likely
                int32_t offset = immediate <<2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;

                //condition ← (GPR[rs] = GPR[rt])
                bool condition = (getRegister((int)rs) == getRegister((int)rt));

                //I+1: if condition then
                if(condition){
                    //PC += target_offset;
                    //continue;//branch delay slot fail
                    newPC = PC + 4 +  target_offset;
                    branch = 0b100;
                }
                else
                {
                    //NullifyCurrentInstruction()
                    PC += 4;
                }
            }
            else if(opcode == BGTZ){
                //if rs > 0 then branch
                int32_t offset = immediate <<2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                if(getRegister((int)rs) > 0)
                {
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
            }
            else if(opcode == BLEZ){
                //if rs ≤ 0 then branch
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                if(getRegister((int)rs) <= 0)
                {
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
            }
            else if(opcode == BLEZL){
                //if rs ≤ 0 then branch_likely
                //if rs ≤ 0 then branch
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                if(getRegister((int)rs) <= 0)
                {
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                else
                {
                    ////NullifyCurrentInstruction()
                    PC += 4;
                }
            }
            else if(opcode == BNE){
                //if rs ≠ rt then branch
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                if(getRegister((int)rs) != getRegister((int)rt))
                {
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
            }
            else if(opcode == BNEL){
                //if rs ≠ rt then branch_likely
                int32_t offset = immediate << 2;
                int32_t target_offset = offset << 14;
                target_offset >>= 14;
                if(getRegister((int)rs) != getRegister((int)rt))
                {
                    newPC = PC + 4 + target_offset;
                    branch = 0b100;
                }
                else
                {
                    PC += 4;
                }
            }
            else if(opcode == LUI){
                //rt ← immediate || 016
                setRegister((int)rt,
                            (int32_t)(immediate << 16));
            }

            else if(opcode == ANDI){
                //GPR[rt] ← GPR[rs] and zero_extend(immediate)
                //immediate = ox0000ffff & CurrentInstruction, already zero_extended.
                setRegister((int)rt,
                            (int32_t)(((uint32_t)getRegister((int)rs)) & immediate));
            }
            else if(opcode == XORI){
                //GPR[rt] ← GPR[rs] XOR zero_extend(immediate)
                //immediate = ox0000ffff & CurrentInstruction, already zero_extended.
                setRegister((int)rt,
                            (int32_t)(((uint32_t)getRegister((int)rs)) ^ immediate));
            }
            else if(opcode == ORI){
                //rt ← rs or immediate
                setRegister((int)rt,
                            (int32_t)(((uint32_t)getRegister((int)rs)) | immediate));
            }
            else if (opcode == LW){
                //rt ← memory[base+offset]
                //vAddr ← sign_extend(offset) + GPR[base] if vAddr1..0 ≠ 02 then
                int32_t offset = immediate << 16;
                offset >>= 16;//sign_extend(offset)
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                //if(vAddr & 0x00000003 != 0 )
                //SignalException(AddressError)
                //endif
                setRegister((int)rt,
                            (int32_t)readWord(vAddr,false));
                //(pAddr, CCA)← AddressTranslation (vAddr, DATA, LOAD) memword← LoadMemory (CCA, WORD, pAddr, vAddr, DATA) GPR[rt]← memword
            }
            else if(opcode == LB){
                //rt ← memory[base+offset]
                //vAddr ← sign_extend(offset) + GPR[base]
                int32_t offset = immediate << 16;//0x8000-> 0x80000000
                offset >>= 16;//0xFFFF8000
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint8_t byte = readByte(vAddr,false);
                int32_t sign_extend_byte = byte;
                sign_extend_byte <<= 24;
                sign_extend_byte >>= 24;
                setRegister((int)rt,
                            sign_extend_byte);
                //(pAddr, CCA)← AddressTranslation (vAddr, DATA, LOAD)
                //pAddr ← pAddrPSIZE-1..2 || (pAddr1..0 xor ReverseEndian2)
                //memword← LoadMemory (CCA, BYTE, pAddr, vAddr, DATA)
                //byte ← vAddr1..0 xor BigEndianCPU2
                //GPR[rt]← sign_extend(memword7+8*byte..8*byte)
            }
            else if(opcode == LBU){
                //to load a byte from memory as an unsigned value
                //rt ← memory[base+offset]
                //vAddr ← sign_extend(offset) + GPR[base]
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint8_t byte = readByte(vAddr,false);
                uint32_t zero_extended_byte = byte;//0x000000ff & byte
                setRegister((int)rt,
                            zero_extended_byte);
            }
            else if(opcode == LH){
                //To load a halfword from memory as a signed value
                //rt ← memory[base+offset]
                //vAddr ← sign_extend(offset) + GPR[base]

                uint32_t HalfWord;
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                HalfWord = readByte(vAddr,false);
                HalfWord = HalfWord << 8;
                HalfWord = HalfWord | (readByte(vAddr + 1, false));//
                int32_t sign_extended_half_word = HalfWord << 16;
                sign_extended_half_word >>= 16;
                setRegister((int)rt,
                            sign_extended_half_word);
            }
            else if(opcode == LHU){
                //To load a halfword from memory as an unsigned value
                //rt ← memory[base+offset]
                //vAddr ← sign_extend(offset) + GPR[base]

                uint32_t HalfWord;
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                HalfWord = readByte(vAddr,false);
                HalfWord = HalfWord << 8;
                HalfWord = HalfWord | (readByte(vAddr + 1, false));//
                setRegister((int)rt,
                            HalfWord);

            }
            else if(opcode == LWL){
                int32_t offset = immediate << 16;
                offset >>=16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t current_Word_Addr = vAddr & 0xFFFFFFFC;
                uint32_t next_Word_Addr = current_Word_Addr + 4;
                uint32_t temp=0;
                uint32_t current_Byte_Addr = vAddr;
                for (; current_Byte_Addr < next_Word_Addr; current_Byte_Addr++){
                    temp <<= 8;//order of this two
                    temp |= readByte(current_Byte_Addr,false);//order of this two
                }
                int delta = vAddr - current_Word_Addr;
                temp <<= (delta*8);
                //rt ← rt MERGE memory[base+offset]
                switch(delta){
                case 0:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0x00000000));
                    break;
                case 1:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0x000000FF));
                    break;
                case 2:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0x0000FFFF));
                    break;
                case 3:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0x00FFFFFF));
                    break;
                }
            }
//            else if(opcode == LWL){
//                uint32_t MSK_LWL = 0xffffffff;
//                int32_t offset = immediate << 16;
//                offset >>= 16;
//                int32_t base = getRegister((int) rs);
//                uint32_t vAddr = (uint32_t)(base + offset);
//                int alignment_diff = vAddr % 4;// the value can only be 0, 1, 2, 3.
//                //NOTE: EVEN IF vAddr is negative, remember the % rule is different in the number system, and computer system
//                /*setRegister((int)rt,
//                            (int32_t)(readWord(vAddr, false) << alignment_diff*8) | ((uint32_t)getRegister((int)rt) & (MSK_LWL >> ((4-alignment_diff)*8))));*/
//                //above is the integrated format, now step by step
//                int32_t new_rt_value;
//                uint32_t temp_data = readWord(vAddr - alignment_diff);//first get the whole word out
//
//                //shift to the unalignment position
//                temp_data = temp_data << (alignment_diff * 8);
//                uint32_t ori_rt_value = getRegister((int) rt);
//                uint32_t temp_rt_value = (ori_rt_value & (MSK_LWL >>(4-alignment_diff)*8));
//                new_rt_value = (int32_t) (temp_data | temp_rt_value);
//                setRegister((int)rt,
//                            new_rt_value);
//
//            }

            else if(opcode == LWR){
                int32_t offset = immediate << 16;
                offset >>=16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t current_Word_Addr = vAddr & 0xFFFFFFFC;
                //uint32_t next_Word_Addr = current_Word_Addr + 4;
                uint32_t temp=0;
                uint32_t current_Byte_Addr = current_Word_Addr;
                for (; current_Byte_Addr <= vAddr; current_Byte_Addr++){
                    temp <<= 8;//order of this two
                    temp |= readByte(current_Byte_Addr,false);//order of this two
                }
                int delta = vAddr - current_Word_Addr;
                switch(delta){
                case 0:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0xFFFFFF00));
                    break;
                case 1:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0xFFFF0000));
                    break;
                case 2:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0xFF000000));
                    break;
                case 3:
                    setRegister((int)rt,
                                temp | (getRegister((int)rt) & 0x00000000));
                    break;
                }
            }

            else if(opcode == SB){
                // to store a byte to memory
                // memory[base+offset] ← rt
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t lsb8bit_rt = (uint32_t)getRegister((int)rt) & 0x000000ff;
                uint8_t byte = (uint8_t) lsb8bit_rt;
                writeByte(vAddr,byte,false);

            }

            else if(opcode == SH){
                // To store a halfword to memory
                // memory[base+offset] ← rt
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t rt_value = getRegister((int)rt);
                uint8_t byte0 = rt_value;
                uint8_t byte1 = rt_value >> 8;
                writeByte(vAddr+1, byte0,false);
                writeByte(vAddr+0, byte1,false);

            }

            else if(opcode == SW){
                // memory[base+offset] ← rt
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t Word = (uint32_t)getRegister((int)rt);
                writeWord(vAddr,Word,false);
            }

            else if(opcode == SWL){
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t ori_rt_data = (uint32_t)getRegister((int)rt);
                uint32_t current_Word_Addr = vAddr & 0xFFFFFFFC;
                uint32_t current_Word = readWord(current_Word_Addr,false);
                //uint32_t next_Word_Addr = current_Word_Addr + 4;
                uint32_t data = 0x00000000;

                int delta = vAddr - current_Word_Addr;
                switch(delta)
                {
                case 0:
                    data = ori_rt_data      | (current_Word & 0x00000000); break;

                case 1:
                    data = ori_rt_data >> 8 | (current_Word & 0xff000000);break;

                case 2:
                    data = ori_rt_data >> 16 |(current_Word & 0xffff0000);break;

                case 3:
                    data = ori_rt_data >> 24 | (current_Word & 0xffffff00);break;
                }
                writeWord(current_Word_Addr, data,false);
            }

            else if(opcode == SWR){
                int32_t offset = immediate << 16;
                offset >>= 16;
                int32_t base = getRegister((int)rs);
                uint32_t vAddr = (uint32_t)(base + offset);
                uint32_t ori_rt_data = (uint32_t)getRegister((int)rt);
                uint32_t current_Word_Addr = vAddr & 0xFFFFFFFC;
                uint32_t current_Word = readWord(current_Word_Addr,false);
                //uint32_t next_Word_Addr = current_Word_Addr + 4;
                uint32_t data = 0x00000000;

                int delta = vAddr - current_Word_Addr;
                switch(delta)
                {
                case 0:
                    data = ori_rt_data << 24 | (current_Word & 0x00ffffff); break;

                case 1:
                    data = ori_rt_data << 16 | (current_Word & 0x0000ffff);break;

                case 2:
                    data = ori_rt_data << 8  | (current_Word & 0x000000ff);break;

                case 3:
                    data = ori_rt_data       | (current_Word & 0x00000000);break;
                }
                writeWord(current_Word_Addr, data,false);

            }
        }

        PC += 4;
        branch >>= 1;
        //branch == 0b10 means take branch
        //branch == 0b1 means finish branch delay slot
    } //end fori
    
    
    //Close file pointers & free allocated Memory
    closeFDT();
    CleanUp();
    return 1;
}
