/**
 * @file FastSkyCPU.h
 * @brief Virtual CPU written in plain C !
 * @mainpage
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This is my virtual CPU named SkyCPU !\n
 * This version is designed to be as fastest as possible but also lightweight !\n
 * Everything in this CPU is callback driven.\n
 * \n
 * Please report bug to <skywodd at gmail.com>
 *
 * @section licence_sec Licence
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 * \n
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 * \n
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 *
 * @section other_sec Others notes and compatibility warning
 * Please be sure of your callback fonctions, no check will be made at run time !
 */

#ifndef _FASTSKYCPU_H_
#define _FASTSKYCPU_H_

/* Dependency */
#include <stdint.h>

/* Memory definition */
#ifndef MEMORY_MASK /* All lower bits MUST be set to "1" */
#define MEMORY_MASK 0xFFFF
#endif

/**
 * Interrupts callback type definition
 *
 * @param icode Interrupt code
 */
typedef void (*SkyCPU_interrupt_callback_t)(uint32_t icode);

/**
 * Breakpoints callback type definition
 *
 * @param bcode Breakpoint code
 */
typedef void (*SkyCPU_breakpoint_callback_t)(uint32_t bcode);

/**
 *  CPU runtime structure
 */
typedef struct {
	uint8_t registers[32 + 3]; /*!< General purpose register (+ 3 dummy bytes to avoid buffer overflow) */
	uint8_t skip_next; /*!< If true the next instruction will not be committed */
	uint16_t program_counter, stack_pointer; /*!< Program counter and stack pointer */
	uint8_t memory[MEMORY_MASK + 1]; /*!< Runtime memory space */
	SkyCPU_interrupt_callback_t interrupt_callback; /*!< Callback for INT */
	SkyCPU_breakpoint_callback_t breakpoint_callback; /*!< Callback for BREAK */
} SkyCPU_runtime_t;

/**
 * Initialize registers of a SkyCPU runtime instance
 *
 * @param runtime Pointer to the SkyCPU runtime instance to initialize
 */
static __inline__ void SkyCPU_runtime_init(SkyCPU_runtime_t* runtime) {
	uint8_t i = 0;
	for (; i < 38; ++i)
		((uint8_t*) runtime)[i] = 0;
	runtime->stack_pointer = MEMORY_MASK;
}

/**
 * Setup callbacks of a SkyCPU runtime instance
 *
 * @param runtime Pointer to the SkyCPU runtime instance to setup
 * @param interrupt_callback Pointer to the interrupt function callback
 * @param breakpoint_callback Pointer to the breakpoint function callback
 * @param cpu_halted_callback Pointer to the "cpu halted" function callback
 */
static __inline__ void SkyCPU_callback_setup(SkyCPU_runtime_t* runtime,
		const SkyCPU_interrupt_callback_t interrupt_callback,
		const SkyCPU_breakpoint_callback_t breakpoint_callback) {
	runtime->interrupt_callback = interrupt_callback;
	runtime->breakpoint_callback = breakpoint_callback;
}

/**
 * Fill the SkyCPU memory with some raw data
 *
 * @param runtime Pointer to the SkyCPU runtime instance to fill
 * @param src_data Buffer with source data
 * @param src_size Buffer size (max 65536 bytes)
 * @param offset Offset of buffer's data in memory
 */
static __inline__ void SkyCPU_memory_copy(SkyCPU_runtime_t* runtime,
		const uint8_t* src_data, const uint16_t src_size, const uint16_t offset) {
	uint16_t i = 0;
	for (; i < src_size; ++i)
		runtime->memory[(i + offset) & MEMORY_MASK] = src_data[i];
}

/**
 * Fetch and execute the next instruction from memory
 *
 * @param runtime Pointer to the SkyCPU runtime instance to run
 */
void SkyCPU_fetch_and_execute(SkyCPU_runtime_t* runtime);

#endif /* _FASTSKYCPU_H_ */
