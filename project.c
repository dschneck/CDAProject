#include "spimcore.h"


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
    if ( Mem[PC] % 4 == 0 && ((PC >> 2) + 31) < (65536 >> 2) == 0) // Checking if it is word aligned 
                                                                   // and if new PC goes out of bounds  PC % 4 == 0 for word alignment
        return 1;

    else {
        instruction = &(Mem[PC >> 2]); // instruction now points to next instruction word
        return 0;
    }
    
    
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // NOT DONE YET
    unsigned * tmp = &instruction; // Getting the address 


    *op = tmp[0] >> 2; 
    *r1 = (tmp[0] << 6) + (tmp[1] >> 5);
    *r2 = tmp + 12;
    *r3 = tmp + 17;
    *funct = (tmp[3] << 2) >> 2;
    *offset = (tmp[2] << 8) + tmp[3];
    *jsec = ( tmp[2])+ tmp[3];





    /* I think my understanding of the memory representation
    was wrong
    op = tmp + 0;
    r1 = tmp + 7;
    r2 = tmp + 12;
    r3 = tmp + 17;
    funct = tmp + 5;
    offset = tmp + 17;
    jsec = tmp + 7;
    */
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
      
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //int sign = offset & (-offset); // 

    *extended_value = 0;

    if (offset & (1 << 16)) { //checks if the 16th bit is a 1
        for (int i = 16; i < 32 ; i++) {
            *extended_value = (offset & (1 << i)); // change leftmost 16 bits to 1
        }
        
        for (int i = 0; i < 16; i++) {
            *extended_value = (offset | (1 << i)); // copy rightmost 16 bits to the values in offset
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
                                           
         if(funct == 000) control = '0';        // Check what operation based on funct
    else if(funct == 001) control = '1';
    else if(funct == 010) control = '2';
    else if(funct == 011) control = '3';
    else if(funct == 100) control = '4';
    else if(funct == 101) control = '5';
    else if(funct == 110) control = '6';
    else if(funct == 111) control = '7';
    else return 1;                              // Halt, invalid instruction
    
    
    // Determine what parameters to use
    if(ALUSrc == 'R')
        val2 = data2;                   // R-Types, pass data2
        
    else if(ALUSrc == 'I')
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

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

