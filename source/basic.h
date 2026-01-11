#ifndef SCOURGE__basic
#define SCOURGE__basic

/* Include */
// C
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Define */
// types
typedef uint8_t SAILOR__u8;
typedef uint16_t SAILOR__u16;
typedef uint32_t SAILOR__u32;
typedef uint64_t SAILOR__u64;
typedef int8_t SAILOR__s8;
typedef int16_t SAILOR__s16;
typedef int32_t SAILOR__s32;
typedef int64_t SAILOR__s64;

// aliases
typedef SAILOR__u8 u8;
typedef SAILOR__u16 u16;
typedef SAILOR__u32 u32;
typedef SAILOR__u64 u64;
typedef SAILOR__s8 s8;
typedef SAILOR__s16 s16;
typedef SAILOR__s32 s32;
typedef SAILOR__s64 s64;

// pointers
typedef void* SAILOR__address;
#define SAILOR__define__null_address 0
#define SAILOR__define__null_flag 0

// general
typedef SAILOR__u8 SAILOR__character;
typedef SAILOR__u64 SAILOR__file_index;
typedef SAILOR__u64 SAILOR__line_number;
typedef SAILOR__u64 SAILOR__character_index;
typedef SAILOR__u64 SAILOR__tab_count;
typedef SAILOR__u64 SAILOR__length;
typedef SAILOR__u64 SAILOR__bit_count;
typedef SAILOR__u64 SAILOR__byte_count;
typedef SAILOR__u64 SAILOR__digit_count;
#define SAILOR__define__bits_in_byte 8
#define SAILOR__define__zero_length 0

// boolean type
typedef enum SAILOR__bt {
	SAILOR__bt__false = 0,
	SAILOR__bt__true = 1,
	SAILOR__bt__COUNT = 2,
} SAILOR__bt;

// file index
#define SAILOR__define__null_file_index_ID -1

/* Allocation */
// return memory to OS
void SAILOR__close__allocation(SAILOR__address start, SAILOR__address end) {
	// do useless operation to quiet compiler
	end = end;

	// return memory
	free(start);

	return;
}

// ask OS for memory
SAILOR__address SAILOR__open__allocation(SAILOR__length length) {
	// return allocation
	return (SAILOR__address)malloc(length);
}

/* Buffer */
// buffer
typedef struct SAILOR__buffer {
	SAILOR__address start;
	SAILOR__address end;
} SAILOR__buffer;

// close buffer
void SAILOR__close__buffer(SAILOR__buffer buffer) {
	// close allocation
	SAILOR__close__allocation(buffer.start, buffer.end);

	return;
}

// create buffer to have specific exact contents
SAILOR__buffer SAILOR__create__buffer(SAILOR__address start, SAILOR__address end) {
	SAILOR__buffer output;

	// setup output
	output.start = start;
	output.end = end;

	return output;
}

// create buffer in it's standard null setup
SAILOR__buffer SAILOR__create_null__buffer() {
	// return standard null buffer
	return SAILOR__create__buffer(SAILOR__define__null_address, SAILOR__define__null_address);
}

// calculate buffer length
SAILOR__length SAILOR__calculate__buffer_length(SAILOR__buffer buffer) {
    return (SAILOR__length)((u8*)buffer.end - (u8*)buffer.start) + 1;
}

// check to see if the pointers in the buffers are equal
SAILOR__bt SAILOR__calculate__buffer_addresses_equal(SAILOR__buffer a, SAILOR__buffer b) {
    return (SAILOR__bt)((a.start == b.start) && (a.end == b.end));
}

// check to see if the contents in the buffers are identical
SAILOR__bt SAILOR__calculate__buffer_contents_equal(SAILOR__buffer a, SAILOR__buffer b) {
    // check if names are same length
    if (SAILOR__calculate__buffer_length(a) != SAILOR__calculate__buffer_length(b)) {
        // not same length so not identical
        return SAILOR__bt__false;
    }

    // get pointers
    SAILOR__address a_current = a.start;
    SAILOR__address b_current = b.start;

    // check each character
    while (a_current <= a.end) {
        // check character
        if (*(SAILOR__character*)a_current != *(SAILOR__character*)b_current) {
            // character not identical, string not identical
            return SAILOR__bt__false;
        }

        // next characters
        a_current += sizeof(SAILOR__character);
        b_current += sizeof(SAILOR__character);
    }

    // no issues found, buffers are identical
    return SAILOR__bt__true;
}

SAILOR__bt SAILOR__calculate__buffer_starts_with_buffer(SAILOR__buffer searching_in, SAILOR__buffer searching_for) {
    // ensure search is larger than the buffer
    if (SAILOR__calculate__buffer_length(searching_in) < SAILOR__calculate__buffer_length(searching_for)) {
        // not bigger or equal, thus impossible to contain
        return SAILOR__bt__false;
    }

    return SAILOR__calculate__buffer_contents_equal(searching_for, SAILOR__create__buffer(searching_in.start, searching_in.start + SAILOR__calculate__buffer_length(searching_for) - 1));
}

// calculate buffer contains range
SAILOR__bt SAILOR__calculate__buffer_range_in_buffer_range_inclusive(SAILOR__buffer outside, SAILOR__buffer inside) {
    // return calculation
    return (outside.start <= inside.start) && (outside.end >= inside.end);
}

// check to see if it is an empty buffer
SAILOR__bt SAILOR__check__empty_buffer(SAILOR__buffer buffer) {
    // return calculation
    return (SAILOR__bt)(buffer.start == SAILOR__define__null_address);
}

// open buffer
SAILOR__buffer SAILOR__open__buffer(SAILOR__length length) {
	SAILOR__buffer output;

	// attempt allocation
	output.start = SAILOR__open__allocation(length);

	// set end of buffer according to allocation success
	if (output.start != SAILOR__define__null_address) {
		output.end = (SAILOR__address)((((SAILOR__u64)output.start) + length) - 1);
	} else {
		output.end = SAILOR__define__null_address;
	}

	return output;
}

// check if buffers are the same size
SAILOR__bt SAILOR__calculate__are_buffers_same_size(SAILOR__buffer a, SAILOR__buffer b) {
    // return calculation
    return (SAILOR__bt)((a.end - a.start) == (b.end - b.start));
}

// copy buffer
void SAILOR__copy__buffer(SAILOR__buffer source, SAILOR__buffer destination, SAILOR__bt* error) {
    // check for invalid buffer
    if (SAILOR__calculate__are_buffers_same_size(source, destination) == SAILOR__bt__true && source.start <= source.end && destination.start <= destination.end) {
        // copy buffer
        for (SAILOR__length byte_index = 0; byte_index < (SAILOR__length)(destination.end - destination.start) + 1; byte_index++) {
            // copy byte
            ((SAILOR__u8*)destination.start)[byte_index] = ((SAILOR__u8*)source.start)[byte_index];
        }
    } else {
        // set error
        *error = SAILOR__bt__true;
    }

    return;
}

// copy buffer backwards
void SAILOR__copy__buffer__backwards(SAILOR__buffer source, SAILOR__buffer destination, SAILOR__bt* error) {
    // check for invalid buffer
    if (SAILOR__calculate__are_buffers_same_size(source, destination) == SAILOR__bt__true && source.start <= source.end  && destination.start <= destination.end) {
        // copy buffer
        for (SAILOR__length byte_index = (SAILOR__length)(destination.end - destination.start) + 1; byte_index > 0; byte_index--) {
            // copy byte
            ((SAILOR__u8*)destination.start)[byte_index - 1] = ((SAILOR__u8*)source.start)[byte_index - 1];
        }
    } else {
        // set error
        *error = SAILOR__bt__true;
    }

    return;
}

// create or open a buffer from a string literal (can either duplicate buffer or simply reference original) (can opt out of null termination)
SAILOR__buffer SAILOR__open__buffer_from_string(u8* string, SAILOR__bt duplicate, SAILOR__bt null_terminate) {
    SAILOR__buffer output;
    SAILOR__length length;

    // setup length
    length = 0;

    // get buffer length
    while (string[length] != 0) {
        length++;
    }

    // optionally append null termination
    if (null_terminate == SAILOR__bt__true) {
        length++;
    }

    // reference or duplicate
    if (duplicate == SAILOR__bt__true) {
        // attempt allocation
        output = SAILOR__open__buffer(length);

        // check for null allocation
        if (output.start == SAILOR__define__null_address) {
            // return empty buffer
            return output;
        }

        // copy buffer byte by byte
        for (SAILOR__length byte_index = 0; byte_index < length; byte_index++) {
            // copy byte
            ((SAILOR__u8*)output.start)[byte_index] = string[byte_index];
        }
    } else {
        // setup duplicate output
        output.start = string;
        output.end = string + length - 1;
    }

    return output;
}

// read buffer
SAILOR__u64 SAILOR__read__buffer(SAILOR__address source, SAILOR__length byte_amount) {
	SAILOR__u64 output;

	// setup output
	output = 0;

	// read buffer
	for (SAILOR__u64 byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// get byte
		((SAILOR__u8*)&output)[byte_index] = ((SAILOR__u8*)source)[byte_index];
	}

	// return output
	return output;
}

// write buffer
void SAILOR__write__buffer(SAILOR__u64 source, SAILOR__length byte_amount, SAILOR__address destination) {
	// write data to buffer
	for (SAILOR__length byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// write byte
		((SAILOR__u8*)destination)[byte_index] = ((SAILOR__u8*)&source)[byte_index];
	}
	
	return;
}

// append null termination
SAILOR__buffer SAILOR__add__null_termination_to_file_path(SAILOR__buffer file_path, SAILOR__bt* error_occured) {
    SAILOR__buffer output;

    // allocate buffer
    output = SAILOR__open__buffer(SAILOR__calculate__buffer_length(file_path) + 1);

    // copy buffer
    SAILOR__copy__buffer(file_path, SAILOR__create__buffer(output.start, output.end - 1), error_occured);

    // append null termination
    *((SAILOR__character*)output.end) = 0;

    return output;
}

// create buffer from file
SAILOR__buffer SAILOR__move__file_to_buffer(SAILOR__buffer file_path) {
	SAILOR__buffer output;
	FILE* file_handle;
	SAILOR__u64 file_size;
    SAILOR__buffer null_terminated_file_path = file_path;
    SAILOR__bt error_occured = SAILOR__bt__false;

    // check for null termination
    if (*(SAILOR__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = SAILOR__add__null_termination_to_file_path(file_path, &error_occured);
    }

	// open file
	file_handle = fopen((const char*)null_terminated_file_path.start, "rb");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return empty buffer
		output = SAILOR__create_null__buffer();

        goto quit_no_file_handle;
	}

	// get file size
	fseek(file_handle, 0, SEEK_END);
	file_size = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);

	// allocate buffer
	output = SAILOR__open__buffer(file_size);

	// check if buffer allocated
	if (output.start == SAILOR__define__null_address) {
		// exit
        goto quit;
	}

	// read file into buffer
	fread(output.start, file_size, 1, file_handle);

    // return offset
    quit:

	// close file handle
	fclose(file_handle);

    // quit no file handle
    quit_no_file_handle:

    // close null file path if necessary
    if (*(SAILOR__character*)file_path.end != 0) {
        SAILOR__close__buffer(null_terminated_file_path);
    }

	// return buffer
	return output;
}

// create file from buffer
void SAILOR__move__buffer_to_file(SAILOR__bt* error, SAILOR__buffer file_path, SAILOR__buffer data) {
	FILE* file_handle;
    SAILOR__buffer null_terminated_file_path = file_path;

    // check for null termination
    if (*(SAILOR__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = SAILOR__add__null_termination_to_file_path(file_path, error);
    }

    // setup error to no error to start
    *error = SAILOR__bt__false;

	// open file
	file_handle = fopen((const char*)null_terminated_file_path.start, "w+b");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return error
        *error = SAILOR__bt__true;

		goto quit;
	}

	// write buffer to file
	fwrite(data.start, SAILOR__calculate__buffer_length(data), 1, file_handle);

	// close file handle
	fclose(file_handle);

    // exit offset
    quit:

    // close null file path if necessary
    if (*(SAILOR__character*)file_path.end != 0) {
        SAILOR__close__buffer(null_terminated_file_path);
    }

	// return
	return;
}

// delete a file
void SAILOR__delete__file(SAILOR__bt* error, SAILOR__buffer file_path) {
    SAILOR__buffer null_terminated_file_path = file_path;

    // check for null termination
    if (*(SAILOR__character*)file_path.end != 0) {
        // setup null termination
        null_terminated_file_path = SAILOR__add__null_termination_to_file_path(file_path, error);
    }

    // free file
    remove(null_terminated_file_path.start);

    // close null file path if necessary
    if (*(SAILOR__character*)file_path.end != 0) {
        SAILOR__close__buffer(null_terminated_file_path);
    }

    return;
}

// print buffer
void SAILOR__print__buffer(SAILOR__buffer buffer) {
    // print character by character
    for (SAILOR__address character = buffer.start; character <= buffer.end; character += sizeof(SAILOR__character)) {
        // print character
        putchar(*(SAILOR__character*)character);
    }

    return;
}

/* List */
// list types
typedef SAILOR__u64 SAILOR__list_filled_index;
typedef SAILOR__u64 SAILOR__list_increase;

// list object
typedef struct SAILOR__list {
    SAILOR__buffer buffer;
    SAILOR__list_filled_index filled_index;
    SAILOR__list_increase increase;
} SAILOR__list;

// create a list
SAILOR__list SAILOR__create__list(SAILOR__buffer buffer, SAILOR__list_filled_index filled_index, SAILOR__list_increase increase) {
    SAILOR__list output;

    // setup output
    output.buffer = buffer;
    output.filled_index = filled_index;
    output.increase = increase;

    return output;
}

// create a null list
SAILOR__list SAILOR__create_null__list() {
    // return empty list
    return SAILOR__create__list(SAILOR__create_null__buffer(), 0, 0);
}

// open a list
SAILOR__list SAILOR__open__list(SAILOR__list_increase increase, SAILOR__bt* error_occured) {
    SAILOR__list output;
    SAILOR__buffer allocation;

    // allocate list
    allocation = SAILOR__open__buffer(increase);

    // check list validity
    if (allocation.start == SAILOR__define__null_address) {
        // set error
        *error_occured = SAILOR__bt__true;

        // return empty
        return SAILOR__create_null__list();
    // list is valid
    } else {
        // set error to false
        *error_occured = SAILOR__bt__false;
    }

    // setup output
    output.buffer = allocation;
    output.filled_index = 0;
    output.increase = increase;

    return output;
}

// destroy a list
void SAILOR__close__list(SAILOR__list list) {
    // free buffer
    SAILOR__close__buffer(SAILOR__create__buffer(list.buffer.start, list.buffer.end));

    return;
}

// expand a list
void SAILOR__list__expand(SAILOR__list* list, SAILOR__bt* error_occured) {
    SAILOR__list_filled_index new_size;
    SAILOR__buffer new_allocation;

    // calculate new buffer size
    new_size = ((SAILOR__u64)(*list).buffer.end - (SAILOR__u64)(*list).buffer.start + 1) + (*list).increase;

    // request new memory
    new_allocation = SAILOR__open__buffer(new_size);

    // check for failure
    if (new_allocation.start == SAILOR__define__null_address) {
        // set error
        *error_occured = SAILOR__bt__true;

        // return unmodified list
        return;
    }

    // copy old data to new list
    for (SAILOR__list_filled_index i = 0; i < (*list).filled_index; i++) {
        // copy one byte
        ((SAILOR__u8*)new_allocation.start)[i] = ((SAILOR__u8*)(*list).buffer.start)[i];
    }

    // free old buffer
    SAILOR__close__buffer((*list).buffer);

    // setup new list allocation
    (*list).buffer = new_allocation;

    return;
}

// request space for the list
void SAILOR__list__request__space(SAILOR__list* list, SAILOR__byte_count byte_count, SAILOR__bt* error_occured) {
    // expand the list until there is enough space
    while (((SAILOR__u64)(*list).buffer.end - (SAILOR__u64)(*list).buffer.start + 1) < ((*list).filled_index + byte_count)) {
        // expand the list
        SAILOR__list__expand(list, error_occured);

        // check for error
        if (*error_occured == SAILOR__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// add index to address
SAILOR__address SAILOR__calculate__address_from_buffer_index(SAILOR__address start, SAILOR__list_filled_index index) {
    return start + index;
}

// calculate the tip of the list
SAILOR__address SAILOR__calculate__list_current_address(SAILOR__list* list) {
    return SAILOR__calculate__address_from_buffer_index((*list).buffer.start, (*list).filled_index);
}

// calculate the current buffer
SAILOR__buffer SAILOR__calculate__list_current_buffer(SAILOR__list* list) {
    return SAILOR__create__buffer(((*list).buffer.start), SAILOR__calculate__list_current_address(list) - 1);
}

// macro to create custom appender


// add a buffer to a list
void SAILOR__list__append__buffer(SAILOR__list* list, SAILOR__buffer buffer, SAILOR__bt* memory_error_occured) {
    // request space
    SAILOR__list__request__space(list, sizeof(SAILOR__buffer), memory_error_occured);

    // append data
    (*(SAILOR__buffer*)SAILOR__calculate__list_current_address(list)) = buffer;

    // increase fill
    (*list).filled_index += sizeof(SAILOR__buffer);

    return;
}

// add a buffer's data to a list
void SAILOR__list__append__buffer_data(SAILOR__list* list, SAILOR__buffer buffer, SAILOR__bt* memory_error_occured) {
    SAILOR__length buffer_length;
    SAILOR__address buffer_old_end;

    // calculate buffer length
    buffer_length = SAILOR__calculate__buffer_length(buffer);

    // request space
    SAILOR__list__request__space(list, buffer_length, memory_error_occured);

    // calculate old buffer end
    buffer_old_end = (*list).buffer.start + (*list).filled_index - 1;

    // append data
    SAILOR__copy__buffer(buffer, SAILOR__create__buffer(buffer_old_end + 1, buffer_old_end + 1 + buffer_length - 1), memory_error_occured);

    // increase fill
    (*list).filled_index += buffer_length;

    return;
}

// add a list to a list
void SAILOR__list__append__list(SAILOR__list* list, SAILOR__list data, SAILOR__bt* memory_error_occured) {
    // request space
    SAILOR__list__request__space(list, sizeof(SAILOR__list), memory_error_occured);

    // append data
    (*(SAILOR__list*)SAILOR__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(SAILOR__list);

    return;
}

// remove a slice of data from a list
void SAILOR__list__erase__space(SAILOR__list* list, SAILOR__list_filled_index range_start_index, SAILOR__list_filled_index range_end_index) {
    SAILOR__buffer old_right;
    SAILOR__buffer new_right;
    SAILOR__bt error;

    // get new right buffer
    old_right = SAILOR__create__buffer(SAILOR__calculate__address_from_buffer_index((*list).buffer.start, range_end_index), SAILOR__calculate__list_current_address(list));
    new_right = SAILOR__create__buffer(old_right.start - (range_end_index - range_start_index + 1), old_right.end - (range_end_index - range_start_index + 1));

    // move data from left to right filling in the gap
    SAILOR__copy__buffer(old_right, new_right, &error);

    // should not happen but handled anyways
    if (error == SAILOR__bt__true) {
        // tell user
        printf("Internal Error: Buffer could not erase data.\n");
    // buffer was clipped, change filled index
    } else {
        // change filled index
        (*list).filled_index -= range_end_index - range_start_index + 1;
    }

    return;
}

// check if two lists are filled up to the same amount
SAILOR__bt SAILOR__calculate__lists_have_same_fill_size(SAILOR__list* a, SAILOR__list* b) {
    return (SAILOR__bt)((*a).filled_index == (*b).filled_index);
}

// take a list and make a standalone buffer
SAILOR__buffer SAILOR__list__open_buffer_from_list(SAILOR__list* list, SAILOR__bt* memory_error_occured) {
    SAILOR__buffer output;

    // allocate output
    output = SAILOR__open__buffer((*list).filled_index);

    // if buffer did not open
    if (SAILOR__check__empty_buffer(output) == SAILOR__bt__true) {
        // set error
        *memory_error_occured = SAILOR__bt__true;
    // if buffer opened
    } else {
        // copy data from list to buffer
        SAILOR__copy__buffer(SAILOR__calculate__list_current_buffer(list), output, memory_error_occured);
    }

    return output;
}

// check if a list is uninitialized
SAILOR__bt SAILOR__check__empty_list(SAILOR__list list) {
    return SAILOR__check__empty_buffer(list.buffer);
}

/* Counted List */
// structure
typedef struct SAILOR__counted_list {
    SAILOR__list list;
    SAILOR__length count;
} SAILOR__counted_list;

// create structure
SAILOR__counted_list SAILOR__create__counted_list(SAILOR__list list, SAILOR__length count) {
    SAILOR__counted_list output;

    // setup output
    output.list = list;
    output.count = count;

    return output;
}

// create null structure
SAILOR__counted_list SAILOR__create_null__counted_list() {
    // return empty
    return SAILOR__create__counted_list(SAILOR__create_null__list(), SAILOR__define__zero_length);
}

// close counted list
void SAILOR__close__counted_list(SAILOR__counted_list counted_list) {
    // close internals
    SAILOR__close__list(counted_list.list);

    return;
}

// open counted list
SAILOR__counted_list SAILOR__open__counted_list(SAILOR__list_increase increase, SAILOR__bt* error_occured) {
    return SAILOR__create__counted_list(SAILOR__open__list(increase, error_occured), 0);
}

/* Current */
// define
typedef SAILOR__buffer SAILOR__current;

// check if a current buffer is still valid
SAILOR__bt SAILOR__check__current_within_range(SAILOR__current current) {
    return (current.start <= current.end);
}

// calculate a current buffer from a list // NOTE: buffer cannot be null or calculation fails!
SAILOR__current SAILOR__calculate__current_from_list_filled_index(SAILOR__list* list) {
    return SAILOR__create__buffer((*list).buffer.start, (*list).buffer.start + (*list).filled_index - 1);
}

// check for a character at a current
SAILOR__bt SAILOR__check__character_range_at_current(SAILOR__current current, SAILOR__character range_start, SAILOR__character range_end) {
    return ((*(SAILOR__character*)current.start) >= range_start) && ((*(SAILOR__character*)current.start) <= range_end);
}

// calculate the amounnt of items in one list (assumes all items are same size!)
SAILOR__list_filled_index SAILOR__calculate__list_content_count(SAILOR__list list, size_t item_size) {
    return list.filled_index / item_size;
}

/* Essentials */
SAILOR__u64 SAILOR__calculate__exponent(SAILOR__u64 base, SAILOR__u64 exponent) {
    SAILOR__u64 output = 1;

    // if zero
    if (exponent == 0) {
        return output;
    }

    // calculate number
    while (exponent > 0) {
        // multiply
        output *= base;

        // next exponent
        exponent--;
    }

    return output;
}

// print tabs
void SAILOR__print__tabs(SAILOR__tab_count tab_count) {
    // print tabs
    while (tab_count > 0) {
        // print tab
        printf("|   ");

        // next tab
        tab_count--;
    }

    return;
}

// calculate character count for binary number to decimal string
SAILOR__digit_count SAILOR__calculate__digit_count(SAILOR__u64 base, SAILOR__u64 value) {
    // check for zero value
    if (value == 0) {
        return 1;
    }

    // check for invalid denominator
    if (base < 2) {
        return 0;
    }

    // calculate count
    // setup count
    SAILOR__digit_count count = 0;

    // loop through bases
    while (value > 0) {
        // divide
        value /= base;

        // count
        count++;
    }

    return count;
}

// convert an integer into a base numbered number
SAILOR__buffer SAILOR__cast__integer_to_unsigned_text_value(SAILOR__u64 value, SAILOR__u64 base, SAILOR__buffer digits, SAILOR__bt direction /* false is lower to higher, true is higher to lower */) {
    // calculate digit count
    SAILOR__digit_count digit_count = SAILOR__calculate__digit_count(base, value);

    // allocate string
    SAILOR__buffer output = SAILOR__open__buffer(digit_count);

    // if lower to higher
    if (direction == SAILOR__bt__false) {
        // for each character
        for (SAILOR__digit_count digit_index = 0; digit_index < digit_count; digit_index++) {
            // calculate & write digit
            ((SAILOR__u8*)output.start)[digit_index] = (value % base) + (*(SAILOR__u8*)digits.start);

            // next digit
            value /= base;
        }
    }

    // if higher to lower
    if (direction == SAILOR__bt__true) {
        // for each character
        for (SAILOR__digit_count digit_index = digit_count; digit_index > 0; digit_index--) {
            // calculate & write digit
            ((SAILOR__u8*)output.start)[digit_index - 1] = (value % base) + (*(SAILOR__u8*)digits.start);

            // next digit
            value /= base;
        }
    }

    return output;
}

// cast integer to unsigned base 10 integer
SAILOR__buffer SAILOR__cast__integer_to_unsigned_base_10(SAILOR__u64 value) {
    return SAILOR__cast__integer_to_unsigned_text_value(value, 10, SAILOR__open__buffer_from_string((u8*)"0123456789", SAILOR__bt__false, SAILOR__bt__false), SAILOR__bt__true);
}

#endif
