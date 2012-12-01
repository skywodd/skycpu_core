/**
 * @file Endian_utility.h
 * @brief Endianess utilities routines
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This header file define some usefull functions for handling endianess values.\n
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
 */

#ifndef _ENDIAN_UTILITY_H_
#define _ENDIAN_UTILITY_H_

/* Dependency */
#include <stdint.h>

/**
 * Get a byte (8 bits) from memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @return Byte value (8 bits)
 */
static __inline__ uint8_t get8bitsValue(const uint8_t* buffer,
		const uint16_t address) {
	return buffer[address];
}

/**
 * Get a word (16 bits) from memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @return Word value (16 bits)
 */
static __inline__ uint16_t get16bitsValue(const uint8_t* buffer,
		const uint16_t address) {
	return (buffer[address] << 8) | buffer[address + 1];
}

/**
 * Get a double word (32 bits) from memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @return Double word value (32 bits)
 */
static __inline__ uint16_t get32bitsValue(const uint8_t* buffer,
		const uint16_t address) {
	return (buffer[address] << 24) | (buffer[address] << 16)
			| (buffer[address] << 8) | buffer[address + 1];
}

/**
 * Set a byte (8 bits) in memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @param value Byte value (8 bits)
 */
static __inline__ void set8bitsValue(uint8_t* buffer, const uint16_t address,
		const uint8_t value) {
	buffer[address] = value;
}

/**
 * Set a word (16 bits) in memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @param value Word value (16 bits)
 */
static __inline__ void set16bitsValue(uint8_t* buffer, const uint16_t address,
		const uint16_t value) {
	buffer[address] = (value >> 8) & 0xFF;
	buffer[address + 1] = value & 0xFF;
}

/**
 * Set a double word (32 bits) in memory
 *
 * @param buffer Source buffer of data
 * @param address Base address of data
 * @param value Double word value (32 bits)
 */
static __inline__ void set32bitsValue(uint8_t* buffer, const uint16_t address,
		const uint32_t value) {
	buffer[address] = (value >> 24) & 0xFF;
	buffer[address + 1] = (value >> 16) & 0xFF;
	buffer[address + 2] = (value >> 8) & 0xFF;
	buffer[address + 3] = value & 0xFF;
}

#endif /* _ENDIAN_UTILITY_H_ */
