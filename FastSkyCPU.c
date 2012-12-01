/*
 * See header file for details
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

/* Includes */
#include "FastSkyCPU.h"
#include "Endian_utility.h"
#include "FastSkyCPU_opcodes.h"

/* Bitwise macro */
/* Instruction : [oooooobb] (o = opcode, b = bits mode) */
#define INSTRUCTION_OPCODE(x) (((x) & 63) >> 2)
#define INSTRUCTION_BITSMODE(x) ((x) & 3)
/* Argument : [cpsrrrrr] (c = constant, p = pointer, s = sfr / inline constant (if c = 1), r = register code*/
#define ARGUMENT_CONSTANT(x) ((x) & 128)
#define ARGUMENT_POINTEDBY(x) ((x) & 64)
#define ARGUMENT_SFRMODE(x) ((x) & 32)
#define ARGUMENT_INLINECONST(x) ((x) & 32)
#define ARGUMENT_REGISTERCODE(x) ((x) & 31)
#define ARGUMENT_INLINEVALUE(x) ((x) & 31)

static uint32_t fetch_argument(const SkyCPU_runtime_t* runtime,
		const uint8_t offset, uint8_t *argument_size, const uint8_t bits_mode) {

	/* Fetch argument */
	uint8_t argument = runtime->memory[runtime->program_counter + offset + *argument_size];

	/* Result value */
	uint32_t address = 0, value = 0;
	*argument_size += 1;

	/* Check for constant value */
	if (ARGUMENT_CONSTANT(argument)) { /* Argument is a constant */

		/* Check for access mode */
		if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by constant */

			/* Check for inline constant */
			if (ARGUMENT_INLINECONST(argument)) { /* Inline constant */

				/* Get inline address ((fixed 6 bits)) */
				address = ARGUMENT_INLINEVALUE(argument);

			} else { /* Normal constant */

				/* Compute address (fixed 16 bits) */
				address = get16bitsValue(runtime->memory,
						runtime->program_counter + offset + *argument_size);
				*argument_size += 2;
			}

		} else { /* Raw constant value */

			/* Check for inline constant */
			if (ARGUMENT_INLINECONST(argument)) { /* Inline constant */

				/* Get inline value ((fixed 6 bits)) */
				value = ARGUMENT_INLINEVALUE(argument);

			} else { /* Normal constant */

				/* Switch according bits mode */
				switch (bits_mode) {
				case SINGLE_BYTE: /* 8 bits constant */
					value = get8bitsValue(runtime->memory,
							runtime->program_counter + offset + *argument_size);
					*argument_size += 1;
					break;

				case SINGLE_WORD: /* 16 bits constant */
					value = get16bitsValue(runtime->memory,
							runtime->program_counter + offset + *argument_size);
					*argument_size += 2;
					break;

				case DOUBLE_WORD: /* 32 bits constant */
					value = get32bitsValue(runtime->memory,
							runtime->program_counter + offset + *argument_size);
					*argument_size += 4;
					break;
				}
			}
		}

	} else { /* Argument is a register */

		/* Check for Special function registers */
		if (ARGUMENT_SFRMODE(argument)) { /* Special function register */

			/* Check for access mode */
			if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by register */

				/* Switch according sfr opcode */
				switch (ARGUMENT_REGISTERCODE(argument)) {
				case REGISTER_PC: /* Program counter */
					address = runtime->program_counter - offset  - *argument_size - 1;
					break;

				case REGISTER_SP: /* Stack pointer */
					address = runtime->stack_pointer;
					break;
				}

			} else { /* Raw register value */

				/* Switch according sfr opcode */
				switch (ARGUMENT_REGISTERCODE(argument)) {
				case REGISTER_PC: /* Program counter */
					value = runtime->program_counter;
					break;

				case REGISTER_SP: /* Stack pointer */
					value = runtime->stack_pointer;
					break;
				}

				/* Special case : Single byte mode */
				if (bits_mode == SINGLE_BYTE)
					value &= 0xFF;
			}

		} else { /* General purpose registers */

			/* Check for access mode */
			if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by register (fixed 16 bits) */

				/* Compute address */
				value = get16bitsValue(runtime->registers,
						ARGUMENT_REGISTERCODE(argument));

			} else { /* Raw register value */

				/* Switch according bits mode */
				switch (bits_mode) {
				case SINGLE_BYTE: /* 8 bits register value */
					value = get8bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument));
					*argument_size += 1;
					break;

				case SINGLE_WORD: /* 16 bits register value */
					value = get16bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument));
					*argument_size += 2;
					break;

				case DOUBLE_WORD: /* 32 bits register value */
					value = get32bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument));
					*argument_size += 4;
					break;
				}
			}
		}
	}

	/* Check for access mode */
	if (ARGUMENT_POINTEDBY(argument)) {

		/* Switch according bits mode */
		switch (bits_mode) {
		case SINGLE_BYTE: /* 8 bits pointed by constant */
			value = get8bitsValue(runtime->memory, address);
			break;

		case SINGLE_WORD: /* 16 bits pointed by constant */
			value = get16bitsValue(runtime->memory, address);
			break;

		case DOUBLE_WORD: /* 32 bits pointed by constant */
			value = get32bitsValue(runtime->memory, address);
			break;
		}
	}

	/* Return result value */
	return value;
}

static void commit_register(SkyCPU_runtime_t* runtime, const uint32_t value,
		const uint8_t offset, const uint8_t bits_mode) {

	/* Fetch argument */
	uint8_t argument = runtime->memory[runtime->program_counter + offset];

	/* Target address */
	uint16_t address = 0;

	/* Check for commit skip */
	if (runtime->skip_next)
		return;

	/* Check for constant value */
	if (ARGUMENT_CONSTANT(argument)) { /* Argument is a constant */

		/* Check for access mode */
		if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by constant address (fixed 16 bits) */

			/* Check for inline constant */
			if (ARGUMENT_INLINECONST(argument)) { /* Inline constant */

				/* Compute address ((fixed 6 bits)) */
				address = ARGUMENT_INLINEVALUE(argument);

			} else { /* Normal constant */

				/* Compute address (fixed 16 bits) */
				address = get16bitsValue(runtime->memory,
						runtime->program_counter + offset);
			}

		} else { /* Raw constant value */

			/* You cannot modify a constant value ! It's fucking obvious ! */
		}

	} else { /* Argument is a register */

		/* Check for Special function registers */
		if (ARGUMENT_SFRMODE(argument)) { /* Special functionr register */

			/* Check for access mode */
			if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by register */

				/* Switch according sfr opcode */
				switch (ARGUMENT_REGISTERCODE(argument)) {
				case REGISTER_PC: /* Program counter */
					address = runtime->program_counter - 1;
					break;

				case REGISTER_SP: /* Stack pointer */
					address = runtime->stack_pointer;
					break;
				}

			} else { /* Raw register value */

				/* Switch according sfr opcode */
				switch (ARGUMENT_REGISTERCODE(argument)) {
				case REGISTER_PC: /* Program counter */
					runtime->program_counter = value;
					break;

				case REGISTER_SP: /* Stack pointer */
					runtime->stack_pointer = value;
					break;
				}
			}

		} else { /* General purpose registers */

			/* Check for access mode */
			if (ARGUMENT_POINTEDBY(argument)) { /* Pointed by value */

				/* Compute address */
				address = get16bitsValue(runtime->registers,
						ARGUMENT_REGISTERCODE(argument));

			} else { /* Raw register value */

				/* Switch according bits mode */
				switch (bits_mode) {
				case SINGLE_BYTE: /* 8 bits register value */
					set8bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument), value);
					break;

				case SINGLE_WORD: /* 16 bits register value */
					set16bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument), value);
					break;

				case DOUBLE_WORD: /* 32 bits register value */
					set32bitsValue(runtime->registers,
							ARGUMENT_REGISTERCODE(argument), value);
					break;
				}
			}
		}
	}

	/* Check for access mode & commit */
	if (ARGUMENT_POINTEDBY(argument)) {

		/* Switch according bits mode */
		switch (bits_mode) {
		case SINGLE_BYTE: /* 8 bits pointed by register */
			set8bitsValue(runtime->memory, address, value);
			break;

		case SINGLE_WORD: /* 16 bits pointed by register */
			set16bitsValue(runtime->memory, address, value);
			break;

		case DOUBLE_WORD: /* 32 bits pointed by register */
			set32bitsValue(runtime->memory, address, value);
			break;
		}
	}
}

/* CPU runtime function */
void SkyCPU_fetch_and_execute(SkyCPU_runtime_t* runtime) {

	/* Fetch instruction */
	uint8_t instruction = runtime->memory[(runtime->program_counter)++];
	uint8_t bits_mode = INSTRUCTION_BITSMODE(instruction);

	/* Runtime variables */
	uint8_t offset_A = 0, offset_B = 0;
	uint32_t A, B, R;

	/* Fetch required registers */
	if (INSTRUCTION_OPCODE(instruction) >= INSTRUCTION_JMP)
		A = fetch_argument(runtime, 0, &offset_A, bits_mode);
	if (INSTRUCTION_OPCODE(instruction) >= INSTRUCTION_ADD)
		B = fetch_argument(runtime, offset_A, &offset_B, bits_mode);

	/* Switch according instruction */
	switch (INSTRUCTION_OPCODE(instruction)) {
	case INSTRUCTION_ADD: /* A = A + B */
		R = A + B;
		break;

	case INSTRUCTION_SUB: /* A = A - B */
		R = A - B;
		break;

	case INSTRUCTION_MUL: /* A = A * B */
		R = A * B;
		break;

	case INSTRUCTION_DIV: /* A = A / B */
		R = A / B;
		break;

	case INSTRUCTION_INC: /* A = A + 1 */
		R = A + 1;
		break;

	case INSTRUCTION_DEC: /* A = A - 1 */
		R = A - 1;
		break;

	case INSTRUCTION_CLR: /* A = 0 */
		R = 0;
		break;

	case INSTRUCTION_SET: /* A = MAX_VALUE */
		R = 0xFFFFFFFF;
		break;

	case INSTRUCTION_AND: /* A = A & B */
		R = A & B;
		break;

	case INSTRUCTION_NAND: /* A = ~(A & B) */
		R = ~(A & B);
		break;

	case INSTRUCTION_OR: /* A = A | B */
		R = A | B;
		break;

	case INSTRUCTION_NOR: /* A = ~(A | B) */
		R = ~(A | B);
		break;

	case INSTRUCTION_XOR: /* A = A ^ B */
		R = A ^ B;
		break;

	case INSTRUCTION_NOT: /* A = ~A */
		R = ~A;
		break;

	case INSTRUCTION_NEG: /* A = !A */
		R = !A;
		break;

	case INSTRUCTION_SBI: /* A |= 1 << B */
		R = A | (1 << B);
		break;

	case INSTRUCTION_CLI: /* A &=  ~(1 << B) */
		R = A | ~(1 << B);
		break;

	case INSTRUCTION_LSL: /* A = A << B */
		R = A << B;
		break;

	case INSTRUCTION_LSR: /* A = A >> B */
		R = A >> B;
		break;

	case INSTRUCTION_ROL: /* A = ((A & MSB_MASK) ? LSB_MASK : 0) | (A << B) */
		switch (bits_mode) {
		case SINGLE_BYTE:
			R = ((A & (1 << 7)) ? 1 : 0) | (A << B);
			break;

		case SINGLE_WORD:
			R = ((A & (1 << 15)) ? 1 : 0) | (A << B);
			break;

		case DOUBLE_WORD:
			R = ((A & (1 << 31)) ? 1 : 0) | (A << B);
			break;
		}
		break;

	case INSTRUCTION_ROR: /* A = ((A & LSB_MASK) ? MSB_MASK : 0) | (A >> B) */
		switch (bits_mode) {
		case SINGLE_BYTE:
			R = ((A & 1) ? (1 << 7) : 0) | (A << B);
			break;

		case SINGLE_WORD:
			R = ((A & 1) ? (1 << 15) : 0) | (A << B);
			break;

		case DOUBLE_WORD:
			R = ((A & 1) ? (1 << 31) : 0) | (A << B);
			break;
		}
		break;

	case INSTRUCTION_CXH: /* tmp = A, A = B, B = tmp */
		commit_register(runtime, A, offset_A, bits_mode);
		commit_register(runtime, B, 0, bits_mode);
		break;

	case INSTRUCTION_SWAP: /* A = swap(A) */
		switch (bits_mode) {
		case SINGLE_BYTE:
			R = A;
			break;

		case SINGLE_WORD:
			((uint8_t*) &R)[1] = ((uint8_t*) &A)[0];
			((uint8_t*) &R)[0] = ((uint8_t*) &A)[1];
			break;

		case DOUBLE_WORD:
			((uint8_t*) &R)[3] = ((uint8_t*) &A)[0];
			((uint8_t*) &R)[2] = ((uint8_t*) &A)[1];
			((uint8_t*) &R)[1] = ((uint8_t*) &A)[2];
			((uint8_t*) &R)[0] = ((uint8_t*) &A)[3];
			break;
		}
		break;

	case INSTRUCTION_JN: /* JMP if !(A) */
	case INSTRUCTION_SNN: /* SKIP if (A) */
		if (A)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JNN: /* JMP if (A) */
	case INSTRUCTION_SN: /* SKIP if !(A) */
		if (!A)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JNE: /* JMP if A != B */
	case INSTRUCTION_SE: /* SKIP if A == B */
		if (A == B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JE: /* JMP if A == B */
	case INSTRUCTION_SNE: /* SKIP if A != B */
		if (A != B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JLE: /* JMP if A <= B */
	case INSTRUCTION_SG: /* SKIP if A > B */
		if (A > B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JL: /* JMP if A < B */
	case INSTRUCTION_SGE: /* SKIP if A >= B */
		if (A >= B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JGE: /* JMP if A >= B */
	case INSTRUCTION_SL: /* SKIP if A < B */
		if (A < B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JG: /* JMP if A > B */
	case INSTRUCTION_SLE: /* SKIP if A <= B */
		if (A <= B)
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JBS: /* JMP if A & (1 << B) */
	case INSTRUCTION_SBC: /* SKIP if !(A & (1 << B)) */
		if (!(A & (1 << B)))
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JBC: /* JMP if !(A & (1 << B)) */
	case INSTRUCTION_SBS: /* SKIP if A & (1 << B) */
		if (A & (1 << B))
			runtime->skip_next = 1;
		break;

	case INSTRUCTION_JMP: /* PC = A */
		runtime->program_counter = A & 0xFFFF;
		break;

	case INSTRUCTION_CALL: /* PUSH PC, PC = A */
		runtime->stack_pointer -= 2;
		set16bitsValue(runtime->memory, runtime->stack_pointer,
				runtime->program_counter);
		runtime->program_counter = A & 0xFFFF;
		break;

	case INSTRUCTION_RET: /* POP PC */
		runtime->program_counter = get16bitsValue(runtime->memory,
				runtime->stack_pointer);
		runtime->stack_pointer += 2;
		break;

	case INSTRUCTION_NOP: /* nothing */
		break;

	case INSTRUCTION_BRK: /* breakpoint(A) */
		runtime->breakpoint_callback(A);
		break;

	case INSTRUCTION_INT: /* interrupt(A) */
		runtime->interrupt_callback(A);
		break;

	case INSTRUCTION_MOV: /* A = B */
		R = B;
		break;

	case INSTRUCTION_POP: /* A = RAM[SP++] */
		switch (bits_mode) {
		case SINGLE_BYTE:
			R = get8bitsValue(runtime->memory, runtime->stack_pointer);
			runtime->stack_pointer += 1;
			break;

		case SINGLE_WORD:
			R = get16bitsValue(runtime->memory, runtime->stack_pointer);
			runtime->stack_pointer += 2;
			break;

		case DOUBLE_WORD:
			R = get32bitsValue(runtime->memory, runtime->stack_pointer);
			runtime->stack_pointer += 4;
			break;
		}
		break;

	case INSTRUCTION_PUSH: /* RAM[--SP] = A */
		switch (bits_mode) {
		case SINGLE_BYTE:
			runtime->stack_pointer -= 1;
			set8bitsValue(runtime->memory, runtime->stack_pointer, A & 0xFF);
			break;

		case SINGLE_WORD:
			runtime->stack_pointer -= 2;
			set16bitsValue(runtime->memory, runtime->stack_pointer, A & 0xFFFF);
			break;

		case DOUBLE_WORD:
			runtime->stack_pointer -= 4;
			set32bitsValue(runtime->memory, runtime->stack_pointer, A);
			break;
		}
		break;

	default: /* Unknown opcode */
		break;
	}

	/* Commit result if required */
	if (INSTRUCTION_OPCODE(instruction) >= INSTRUCTION_INC
			&& INSTRUCTION_OPCODE(instruction) <= INSTRUCTION_MOV)
		commit_register(runtime, R, 0, bits_mode);

	/* Apply instruction size offset */
	runtime->program_counter += offset_A + offset_B;
	runtime->skip_next = 0;
}
