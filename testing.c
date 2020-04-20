#include "spimcore.h"

void assignControl(struct_controls *controls, char Aop, char Asrc, char bra, char jum, char memR, char memW, char memTR, char regD, char regW);

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero) {
	// Sets ALUresult depending on the function
	switch(ALUControl) {
		case 0: // Add
			*ALUresult = A + B;
			break;
		case 1: // Subtract 
			*ALUresult = A - B;
			break;
		case 2: // Set less than
			if ((signed)A < (signed)B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
			break;
		case 3: // Set less than (unsigned)
			if (A < B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
		case 4: // And
			*ALUresult = A & B;
			break;
		case 5: // Or
			*ALUresult = A | B;
			break;
		case 6: // Shift B left 16 bits
			*ALUresult = B << 16;
			break;
		case 7: // Not A
			*ALUresult = ~A;
			break;
			
	}

	// Determine if result is 0
	if (!(*ALUresult)) 
		*Zero = 1;
	
	else
		*Zero = 0;
	
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction) {
    if (PC % 4 == 0 ) { // Checking if it is word aligned 
                                                                   // and if new PC goes out of bounds  PC % 4 == 0 for word alignment
        *instruction = Mem[PC >> 2]; // instruction now points to next instruction word
        return 1;
    }

    else {
        return 0;
    }
    
    
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {

	*op     = (instruction & 0xfc000000) >> 26;
	*r1     = (instruction & 0x03e00000) >> 21;
    *r2     = (instruction & 0x001f0000) >> 16;
    *r3     = (instruction & 0x0000f800) >> 11;
    // Place bits at the end of the string
    *funct  =  instruction & 0x0000003f;
    *offset =  instruction & 0x0000ffff;
    *jsec   =  instruction & 0x03ffffff;
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
        case 0x0: // R-type instruction
            assignControl(controls, 7, 0, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 0x8: // add immediate
            assignControl(controls, 0, 1, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 0x23: // load word
            assignControl(controls, 0, 1, 0, 0, 1, 1, 1, 0, 1);
            break;
        case 0x2B: // store word
            assignControl(controls, 0, 1, 0, 0, 0, 1, 0, 0, 0);
            break;
        case 0x0F: // load upper immediate
            assignControl(controls, 6, 1, 0, 0, 0, 0, 0, 0, 1);
            break;    
        case 0x4: // branch on equal
            assignControl(controls, 1, 0, 1, 0, 0, 0, 2, 2, 0);
            break;
        case 0xA: // set less than immediate
            assignControl(controls, 2, 1, 0, 0, 0, 0, 0, 0, 1 );
            break;
        case 0xB: // set less than immediate (unsigned)
            assignControl(controls, 3, 1, 0, 0, 0, 0, 0, 0, 1);
            break;
        case 0x2: // jump
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
void sign_extend(unsigned offset,unsigned *extended_value) {
	if (offset >> 15) {
		*extended_value = offset | 0xFFFF0000;
	}

	else
	{
		*extended_value = offset & 0x0000FFFF;
	}
	
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero) {
	char control;
	
	if (ALUSrc)
		data2 = extended_value;
	
	
	if (ALUOp == 7) {// Make sure its an r-type instruction
		switch (funct) {
			case 0x20: // Add
				control = 0;
				break;
			case 0x22: // Subtract
				control = 1;
				break;
			case 0x2A: // Set less than
				control = 2;
				break;
			case 0x2B: // Set less than (unsigned)
				control = 3;
				break;
			case 0x24: // And
				control = 4;
				break;
			case 0x25: // OR
				control = 5;
				break;
			case 0x6: // Shift left 16 bits
				control = 6;
				break;
			case 0x27:
				control = 7;
			default: // Halt condition
				return 1;

		}

		ALU(data1,data2,ALUOp,ALUresult,Zero);
	}
	return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem) {
	unsigned aligned = ALUresult % 4;

	if (aligned)
		return 1; // Halt condition


	if (MemWrite) {
		Mem[ALUresult >> 2] = data2;
	}

	if (MemRead) {
		*memdata = Mem[ALUresult >> 2];
	}

	return 0;
}	

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg) {
	if (RegWrite) {
		if (MemtoReg) {
			if (RegDst) {
				Reg[r3] = memdata;
			}

			else {
				Reg[r2] = memdata;
			}
		}

		else {
			if (RegDst) {
				Reg[r3] = ALUresult;
			}

			else {
				Reg[r2] = ALUresult;
			}
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC) {
	*PC += 4;

	if(Zero && Branch) {
		*PC += extended_value << 2;
	}

	if (Jump)
		*PC = (*PC & 0xF0000000) | jsec << 2;
}