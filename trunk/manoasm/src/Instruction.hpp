// File:	Instruction.hpp
// Author:	Mark Roth
// Description:
//		A structure that defines an instruction for Mano's machine
// Revision History:
//		0.0:	Initial Revision
//

#pragma once

struct SInstruction {
	char name[4];				// Name of the instruction
	char resolve_references;	// 1 if we should resolve references, else 0
	char i_bit_valid;			// 1 if I-bit is valid, else 0
	unsigned char opcode;		// Opcode for this instruction
	unsigned short operand;		// Default operand (if 0, operand 
								// is specified by asm file)
};
