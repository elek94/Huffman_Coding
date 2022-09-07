#include <assert.h>
#include "bit_writer.h"
#include <stdbool.h>
#include "clog.h"
//..
BitWriter open_bit_writer(const char* path) {
	return (BitWriter) { .file = fopen(path, "w"), .current_byte = 0x00, .num_bits_left = 8 };
}

void write_bits(BitWriter* a_writer, uint8_t bits, uint8_t num_bits_to_write) {
	assert(num_bits_to_write >= 0 && num_bits_to_write <= 8);			 	 // [0, 8]
	assert(a_writer -> num_bits_left >= 1 && a_writer -> num_bits_left <= 8);// [1, 8]

	if((a_writer -> num_bits_left) > 0) {
		// If this is too many for the current byte, then split it to two chunks.
		if(num_bits_to_write > (a_writer -> num_bits_left)) {

			// Write the first chunk.
			uint8_t remain_bits = num_bits_to_write - (a_writer -> num_bits_left);
			a_writer -> current_byte |= bits >> remain_bits;

			fputc(a_writer -> current_byte, a_writer -> file);
			a_writer -> current_byte = 0x00;
			a_writer -> num_bits_left = 8;

			// Figure out how many bits go in to the SECOND chunk.
			write_bits(a_writer, bits, remain_bits);
		}
		else {
			// Write the bits to the current byte (in memory).
			uint8_t step_byte = bits << (8 - num_bits_to_write);
			step_byte = step_byte >> (8 - num_bits_to_write);
			a_writer -> current_byte |= step_byte << ((a_writer -> num_bits_left) - num_bits_to_write);
			a_writer -> num_bits_left -= num_bits_to_write;
		}

		// if the current byte is full.
		if((a_writer -> num_bits_left) == 0) {
			// Write the current byte to the file and then reset it to zero.
			fputc(a_writer -> current_byte, a_writer -> file);
			a_writer -> current_byte = 0x00;
			a_writer -> num_bits_left = 8;
		}
	}
	assert(a_writer -> num_bits_left >= 1 && a_writer -> num_bits_left <= 8);// [1, 8]
}

void flush_bit_writer(BitWriter* a_writer) {
	if(a_writer -> num_bits_left < 8) {
		fputc(a_writer -> current_byte, a_writer -> file);
	}
	a_writer -> current_byte = 0;
	a_writer -> num_bits_left = 8;
}

void close_bit_writer(BitWriter* a_writer) {
	if(a_writer -> current_byte != 0) {
		flush_bit_writer(a_writer);
	}
	fclose(a_writer -> file);
	a_writer -> file = NULL;
}

#define __BIT_WRITER_C_V1__
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab colorcolumn=96: */
