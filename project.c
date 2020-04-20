#include "spimcore.h"

// Extra functions

void assignControl(struct_controls *controls, char Aop, char Asrc, char bra, char jum, char memW, char memR, char memTR, char regD, char regW);

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    int Z = 0;
    
    if(ALUControl == '0')   Z = A + B;              // Addition
    if(ALUControl == '1')   Z = A - B;              // Subtraction
    if(ALUControl == '2' || ALUControl == '3')      // Set on less than (options 2 or 3)
    {
        if(A < B)   Z = 1;
        else        Z = 0;
    }
    if(ALUControl == '4')   Z = A && B;             // And
    if(ALUControl == '5')   Z = A || B;             // Or
    if(ALUControl == '6')   Z = B << 16;            // Bitshift 16 times
    if(ALUControl == '7')   Z = !A;                 // Not A
    
    
    if(Z == 0)              // If result is 0, assign 1 to Zero
        *Zero = '1';
    else
        *Zero = '0';
    
    *ALUresult = Z;         // Output result to ALUresult
    
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if ( Mem[PC] % 4 == 0 && ((PC >> 2) + 31) < (65536 >> 2)) { // Checking if it is word aligned
                                                                   // and if new PC goes out of bounds  PC % 4 == 0 for word alignment
        instruction = &(Mem[PC >> 2]); // instruction now points to next instruction word
        return 0;
    }

    else {
        return 1;
    }
    
    
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
   // Shift right to place the bits at the end of the string
    *op = (instruction & 0xfc000000) >> 26; // Grabs 6 bits [31-26]

    *r1 = (instruction & 0x03e00000) >> 21; // Grabs 5 bits [25-21]

    *r2 = (instruction & 0x001f0000) >> 16; // Grabs 5 bits [20-16]

    *r3 = (instruction & 0x0000f800) >> 11; //Grabs 5 bits [15-11]

    *funct = instruction & 0x0000003f; // Grabs 6 bits [5-0]

    *offset = instruction & 0x0000ffff; // Grabs 16 bits [15-0]
    
    *jsec = instruction & 0x03ffffff; // Grabs 26 bits [25-0]
}

// Function that assignes values to Control codes
// so I don't repeat the code
void assignControl(struct_controls *controls, char Aop, char Asrc, char bra, char jum, char memR, char memW, char memTR, char regD, char regW) {
    controls->ALUOp = Aop;
    controls->ALUSrc = Asrc;
    controls->Branch = bra;
    controls->Jump = jum;
    controls->MemRead = memR;
    controls->MemWrite = memW;
    controls->MemtoReg = memTR;
    controls->RegDst = regD;
    controls->RegWrite = regW;
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    switch(op) { //
        case 0: // R-type instruction
            assignControl(controls, 0, 7, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 8: // add immediate
            assignControl(controls, 0, 1, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 35: // load word
            assignControl(controls, 0, 1, 0, 0, 1, 1, 1, 0, 1);
            break;
        case 43: // store word
            assignControl(controls, 0, 1, 0, 0, 0, 1, 0, 0, 0);
            break;
        case 15: // load upper immediate
            assignControl(controls, 6, 1, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 4: // branch on equal
            assignControl(controls, 1, 0, 1, 0, 0, 0, 2, 2, 0);
            break;
        case 10: // set less than immediate
            assignControl(controls, 2, 1, 0, 0, 0, 0, 0, 0, 1 );
            break;
        case 11: // set less than immediate (unsigned)
            assignControl(controls, 3, 1, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 2: // jump
            assignControl(controls, 0, 2, 0, 1, 2, 2, 2, 2, 2);
            break;
        default:
            return 1; // illegal instruction so return halt
    }
   
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1]; //  The value at Reg at address r1 is assigned to data1
    *data2 = Reg[r2]; // The value at Reg at address r2 is assigned to data2
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

    *extended_value = 0;

    if (offset & (1 << 16)) { //checks if the 16th bit is a 1
        for (int i = 15; i < 32 ; i++) {
            *extended_value = (offset | (1 << i)); // change leftmost 16 bits to 1
        }
        
        for (int i = 0; i < 16; i++) {
            *extended_value = (offset & (1 << i)); // copy rightmost 16 bits to the values in offset
        }
    }

    else {
        *extended_value = offset; // exact copy
    }
    
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    char control;
    unsigned val2;
    
    if(ALUOp == '7')
    {
        if(funct == 000) control = '0';        // Check what operation based on funct
        else if(funct == 001) control = '1';
        else if(funct == 010) control = '2';
        else if(funct == 011) control = '3';
        else if(funct == 100) control = '4';
        else if(funct == 101) control = '5';
        else if(funct == 110) control = '6';
        else if(funct == 111) control = '7';
        else return 1;                          // Halt, invalid instruction
    }
    else
        control = ALUOp;
                            
    
    
    // Determine what parameters to use
    if(ALUSrc == '0')
        val2 = data2;                   // R-Types, pass data2
        
    else if(ALUSrc == '1')
        val2 = extended_value;          // I-Types, pass extended_val
    
    else
        return 1;                       // Halt, invalid instruction
    
    
    ALU(data1, val2, control, ALUresult, Zero);     // Call ALU
    return 0;                                       // No halt condition
    
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // Write to memory, ensuring ALUResult is multiple of 4
    if(MemWrite && (ALUresult % 4 == 0))
        Mem[ALUresult >> 2] = data2;
    
    // Read from memory
    else if(MemRead && (ALUresult % 4 == 0))
        *memdata = Mem[ALUresult >> 2];
    
    // Halt condition
    else
        return 1;
    
    // Successful
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // Write data from memory to register
    if(RegWrite == '1' && MemtoReg == '1')
    {
        if(RegDst)
            Reg[r3] = memdata;              // Bits 15-11
        else
            Reg[r2] = memdata;              // Bits 25-21
    }
    
    // Write data from ALU to result
    else if(RegWrite == '1' && MemtoReg == '0')
    {
        if(RegDst)
            Reg[r3] = ALUresult;            // Bits 15-11
        else
            Reg[r2] = ALUresult;            // Bits 25-21
    }
        
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    // Retrieve next instructions
    *PC += 4;
    
    // Jump to specified location
    if(Jump == '1')
            *PC = (jsec << 2) | (*PC & 0xf0000000);
    
    //Add address to extended value
    if(Branch == '1' && Zero == '1')
        *PC += (extended_value << 2);
    
}
