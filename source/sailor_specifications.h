#ifndef SCOURGE__interpreter_specifications
#define SCOURGE__interpreter_specifications

/* Include */
#include "basic.h"
#include <time.h>

/* Specifications */
// instructions
typedef SAILOR__address SAILOR__cell;
typedef SAILOR__u8 SAILOR__instruction_ID;
typedef SAILOR__u8 SAILOR__flag_ID;
typedef SAILOR__u8 SAILOR__operation_ID;
typedef SAILOR__u16 SAILOR__cell_ID;
typedef SAILOR__u64 SAILOR__instruction_count;
#define SAILOR__define__run_forever (SAILOR__u64)-1
#define SAILOR__define__input_string_max_length 256 // is part of fgets instruction and nothing else
#define SAILOR__define__max_allocation_size 0x100000000

// cell related types
typedef SAILOR__u64 SAILOR__cell_count;
typedef SAILOR__cell_count SAILOR__cell_index;
typedef SAILOR__u64 SAILOR__cell_integer_value;
#define SAILOR__unused_cell_ID 0

// invalids
#define SAILOR__define__null_offset_ID -1
#define SAILOR__define__null_call_ID -1
#define SAILOR__define__invalid_abstraction_call_ID -1

// scraplet types
typedef enum SAILOR__st {
    // scraplet types
    SAILOR__st__instruction_ID,
    SAILOR__st__flag_ID,
    SAILOR__st__operation_ID,
    SAILOR__st__cell_ID,
    SAILOR__st__cell,

    // count
    SAILOR__st__COUNT,
} SAILOR__st;

// context cell organization defines
// cell type
typedef enum SAILOR__rt {
    // start of defined values
    SAILOR__rt__START = 0,

    // basic cells
    SAILOR__rt__program_start_address = SAILOR__rt__START,
    SAILOR__rt__program_end_address,
    SAILOR__rt__program_current_address,
    SAILOR__rt__error_code,
    SAILOR__rt__flags_0,
    SAILOR__rt__flags_1,
    SAILOR__rt__flags_2,
    SAILOR__rt__flags_3,
    SAILOR__rt__address_catch_toggle,

    // end of defined cells
    SAILOR__rt__END,

    // count
    SAILOR__rt__RESERVED_COUNT = SAILOR__rt__END - SAILOR__rt__START,

    // statistics
    SAILOR__rt__FIRST_ID = SAILOR__rt__START,
    SAILOR__rt__LAST_ID = 65535, // NUMBER DOES NOT CHANGE!
    SAILOR__rt__TOTAL_COUNT = SAILOR__rt__LAST_ID + 1,
} SAILOR__rt;

// context
typedef struct SAILOR__context {
    SAILOR__cell cells[SAILOR__rt__TOTAL_COUNT];
} SAILOR__context;

// instruction type and instruction ID defines (type number is also ID)
typedef enum SAILOR__it {
    // start of defined instructions
    SAILOR__it__START = 0,

    // defined instructions
    SAILOR__it__stop = SAILOR__it__START, // returns context to caller
    SAILOR__it__write_cell, // overwrites entire cell with hard coded value
    SAILOR__it__operate, // inter-cell operations
    SAILOR__it__request_memory, // allocate request
    SAILOR__it__return_memory, // deallocate request
    SAILOR__it__address_to_cell, // read memory into cell
    SAILOR__it__cell_to_address, // write cell to memory
    SAILOR__it__file_to_buffer, // get file into buffer
    SAILOR__it__buffer_to_file, // write buffer to disk
    SAILOR__it__delete_file, // deletes a file
    SAILOR__it__buffer_to_buffer__low_to_high, // copy data from one buffer to another of the same size in the low to high direction
    SAILOR__it__buffer_to_buffer__high_to_low, // copy data from one buffer to another of the same size in the high to low direction
    SAILOR__it__compile, // compile one or more source files to an sailor program
    SAILOR__it__run, // run context (both until finished and run one instruction)
    SAILOR__it__get_time, // get time in seconds and nano seconds
    SAILOR__it__debug__putchar, // print one character to stdout
    SAILOR__it__debug__fgets, // read one line from stdin
    SAILOR__it__debug__mark_data_section, // mark a section of data (NOP)
    SAILOR__it__debug__mark_code_section, // mark a section of code (NOP)
    SAILOR__it__debug__get_current_context, // gets the current context as a buffer
    SAILOR__it__debug__search_for_allocation, // takes a buffer and searches for an allocation it fits inside of, if not found, null buffer

    // end of defined instruction types
    SAILOR__it__END,

    // count
    SAILOR__it__COUNT = SAILOR__it__END - SAILOR__it__START,
} SAILOR__it;

// instruction length types
typedef enum SAILOR__ilt {
    SAILOR__ilt__stop = sizeof(SAILOR__instruction_ID),
    SAILOR__ilt__write_cell = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__cell) + sizeof(SAILOR__cell_ID),
    SAILOR__ilt__operate = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__flag_ID) + sizeof(SAILOR__operation_ID) + (sizeof(SAILOR__cell_ID) * 4),
    SAILOR__ilt__request_memory = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 3),
    SAILOR__ilt__return_memory = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 2),
    SAILOR__ilt__address_to_cell = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__flag_ID) + (sizeof(SAILOR__cell_ID) * 3),
    SAILOR__ilt__cell_to_address = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__flag_ID) + (sizeof(SAILOR__cell_ID) * 3),
    SAILOR__ilt__file_to_buffer = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 4),
    SAILOR__ilt__buffer_to_file = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 4),
    SAILOR__ilt__delete_file = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 2),
    SAILOR__ilt__buffer_to_buffer__low_to_high = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 4),
    SAILOR__ilt__buffer_to_buffer__high_to_low = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 4),
    SAILOR__ilt__compile = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 15),
    SAILOR__ilt__run = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 3),
    SAILOR__ilt__get_time = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 2),
    SAILOR__ilt__debug__putchar = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__cell_ID),
    SAILOR__ilt__debug__fgets = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 2),
    SAILOR__ilt__debug__mark_data_section = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__cell),
    SAILOR__ilt__debug__mark_code_section = sizeof(SAILOR__instruction_ID) + sizeof(SAILOR__cell),
    SAILOR__ilt__debug__get_current_context = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 2),
    SAILOR__ilt__debug__search_for_allocation = sizeof(SAILOR__instruction_ID) + (sizeof(SAILOR__cell_ID) * 5),
} SAILOR__ilt;

// error codes
typedef enum SAILOR__et {
    // error codes
    SAILOR__et__no_error = 0,
    SAILOR__et__invalid_instruction_ID,
    SAILOR__et__divide_by_zero,
    SAILOR__et__modulous_by_zero,
    SAILOR__et__file_not_found,
    SAILOR__et__file_not_created,
    SAILOR__et__invalid_address__address_to_cell,
    SAILOR__et__invalid_address__cell_to_address,
    SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer,
    SAILOR__et__invalid_allocation__allocation_does_not_exist,
    SAILOR__et__invalid_allocation__requested_memory_too_large,
    SAILOR__et__invalid_address_range,
    SAILOR__et__buffer_to_buffer__buffers_are_different_sizes,
    SAILOR__et__compile__compilation_error,
    SAILOR__et__compile__invalid_input_configuration,
    SAILOR__et__program_ran_out_of_instructions,
    SAILOR__et__program_ran_out_of_instruction_parameters,

    // count
    SAILOR__et__COUNT,
} SAILOR__et;

// operation types
typedef enum SAILOR__ot {
    // start
    SAILOR__ot__START = 0,

    // copy
    SAILOR__ot__cell_to_cell = SAILOR__ot__START, // copies one cell to another without transformation

    // binary operations
    SAILOR__ot__bits_or,
    SAILOR__ot__bits_invert,
    SAILOR__ot__bits_and,
    SAILOR__ot__bits_xor,
    SAILOR__ot__bits_shift_higher,
    SAILOR__ot__bits_shift_lower,
    SAILOR__ot__bits_overwrite,

    // arithmetic operations
    SAILOR__ot__integer_add,
    SAILOR__ot__integer_subtract,
    SAILOR__ot__integer_multiply,
    SAILOR__ot__integer_division,
    SAILOR__ot__integer_modulous,

    // comparison operations
    SAILOR__ot__integer_within_range, // equivalent to (range_start <= integer_n && integer_n <= range_end) -> boolean

    // flag operations
    SAILOR__ot__flag_or,
    SAILOR__ot__flag_invert,
    SAILOR__ot__flag_and,
    SAILOR__ot__flag_xor,
    SAILOR__ot__flag_get,
    SAILOR__ot__flag_set,

    // end
    SAILOR__ot__END = SAILOR__ot__flag_set,

    // count
    SAILOR__ot__COUNT = SAILOR__ot__END - SAILOR__ot__START + 1,
} SAILOR__ot;

// should the next instruction process
// next instruction type
typedef enum SAILOR__nit {
    // next instruction types
    SAILOR__nit__return_context,
    SAILOR__nit__next_instruction,

    // count
    SAILOR__nit__COUNT,
} SAILOR__nit;

#endif
