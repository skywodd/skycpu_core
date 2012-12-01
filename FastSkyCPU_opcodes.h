/*
 * See main header file for details
 *
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 *
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 *
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 */

#ifndef _FASTSKYCPU_OPCODES_H_
#define _FASTSKYCPU_OPCODES_H_

/**
 * Instructions opcodes definition
 *
 * @warning The order of this enum is VERY important ! Do not modify it !
 */
typedef enum {
	/* No register used */
	INSTRUCTION_NOP, /*!< nothing */
	INSTRUCTION_RET, /*!< POP PC */

	/* Only A register used */
	INSTRUCTION_JMP, /*!< PC = A */
	INSTRUCTION_CALL, /*!< PUSH PC, PC = A */
	INSTRUCTION_PUSH, /*!< RAM[--SP] = A */
	INSTRUCTION_BRK, /*!< breakpoint(A) */
	INSTRUCTION_INT, /*!< interrupt(A) */
	INSTRUCTION_INC, /*!< A = A + 1 */
	INSTRUCTION_DEC, /*!< A = A - 1 */
	INSTRUCTION_CLR, /*!< A = 0 */
	INSTRUCTION_SET, /*!< A = MAX_VALUE */
	INSTRUCTION_NOT, /*!< A = ~A */
	INSTRUCTION_NEG, /*!< A = !A */
	INSTRUCTION_SWAP, /*!< A = swap(A) */
	INSTRUCTION_JNN, /*!< JMP if (A) */
	INSTRUCTION_JN, /*!< JMP if !(A) */
	INSTRUCTION_SNN, /*!< SKIP if (A) */
	INSTRUCTION_SN, /*!< SKIP if !(A) */
	INSTRUCTION_POP, /*!< A = RAM[SP++] */

	/* A & B registers used */
	INSTRUCTION_ADD, /*!< A = A + B */
	INSTRUCTION_SUB, /*!< A = A - B */
	INSTRUCTION_MUL, /*!< A = A * B */
	INSTRUCTION_DIV, /*!< A = A / B */
	INSTRUCTION_AND, /*!< A = A & B */
	INSTRUCTION_NAND, /*!< A = ~(A & B) */
	INSTRUCTION_OR, /*!< A = A | B */
	INSTRUCTION_NOR, /*!< A = ~(A | B) */
	INSTRUCTION_XOR, /*!< A = A ^ B */
	INSTRUCTION_SBI, /*!< A |= 1 << B */
	INSTRUCTION_CLI, /*!< A &=  ~(1 << B) */
	INSTRUCTION_LSL, /*!< A = A << B */
	INSTRUCTION_LSR, /*!< A = A >> B */
	INSTRUCTION_ROL, /*!< A = ((A & MSB_MASK) ? LSB_MASK : 0) | (A << B) */
	INSTRUCTION_ROR, /*!< A = ((A & LSB_MASK) ? MSB_MASK : 0) | (A >> B) */
	INSTRUCTION_MOV, /*!< A = B */
	INSTRUCTION_CXH, /*!< tmp = A, A = B, B = tmp */
	INSTRUCTION_JE, /*!< JMP if A == B */
	INSTRUCTION_JNE, /*!< JMP if A != B */
	INSTRUCTION_JG, /*!< JMP if A > B */
	INSTRUCTION_JGE, /*!< JMP if A >= B */
	INSTRUCTION_JL, /*!< JMP if A < B */
	INSTRUCTION_JLE, /*!< JMP if A <= B */
	INSTRUCTION_JBC, /*!< JMP if !(A & (1 << B)) */
	INSTRUCTION_JBS, /*!< JMP if A & (1 << B) */
	INSTRUCTION_SE, /*!< SKIP if A == B */
	INSTRUCTION_SNE, /*!< SKIP if A != B */
	INSTRUCTION_SG, /*!< SKIP if A > B */
	INSTRUCTION_SGE, /*!< SKIP if A >= B */
	INSTRUCTION_SL, /*!< SKIP if A < B */
	INSTRUCTION_SLE, /*!< SKIP if A <= B */
	INSTRUCTION_SBC, /*!< SKIP if !(A & (1 << B)) */
	INSTRUCTION_SBS /*!< SKIP if A & (1 << B) */
} SkyCPU_instruction_opcode_t;

/**
 * Bits modes definition
 */
typedef enum {
	NO_TYPE, /*!< No bits mode */
	SINGLE_BYTE, /*!< 8 bits mode */
	SINGLE_WORD, /*!< 16 bits mode */
	DOUBLE_WORD /*!< 32 bits mode */
} SkyCPU_instruction_bits_mode_t;

/**
 * Registers opcodes definition
 */
typedef enum {

	/* General purpose registers */
	REGISTER_0,
	REGISTER_1,
	REGISTER_2,
	REGISTER_3,
	REGISTER_4,
	REGISTER_5,
	REGISTER_6,
	REGISTER_7,
	REGISTER_8,
	REGISTER_9,
	REGISTER_10,
	REGISTER_11,
	REGISTER_12,
	REGISTER_13,
	REGISTER_14,
	REGISTER_15,
	REGISTER_16,
	REGISTER_17,
	REGISTER_18,
	REGISTER_19,
	REGISTER_20,
	REGISTER_21,
	REGISTER_22,
	REGISTER_23,
	REGISTER_24,
	REGISTER_25,
	REGISTER_26,
	REGISTER_27,
	REGISTER_28,
	REGISTER_29,
	REGISTER_30,
	REGISTER_31,

	/* Special function registers */
	REGISTER_PC = 0, /*!< Program counter register */
	REGISTER_SP /*!< Stack pointer register */
} SkyCPU_arguments_register_t;

#endif /* _FASTSKYCPU_OPCODES_H_ */
