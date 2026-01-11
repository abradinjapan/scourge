#ifndef SCOURGE__sailor
#define SCOURGE__sailor

/* Include */
#include "scourge/compiler/compiler.h"

/* Allocations */
// allocations
typedef struct SAILOR__allocations {
    SAILOR__list buffers; // SAILOR__buffer
} SAILOR__allocations;

// find an allocation
SAILOR__list_filled_index SAILOR__find__allocation(SAILOR__allocations* allocations, SAILOR__buffer allocation, SAILOR__bt* found) {
    SAILOR__list_filled_index output = 0;
    SAILOR__buffer current;
    
    // setup current
    current = SAILOR__calculate__list_current_buffer(&((*allocations).buffers));

    // check for valid allocation
    while (current.start <= current.end) {
        // check one allocation
        if (SAILOR__calculate__buffer_addresses_equal(*(SAILOR__buffer*)current.start, allocation)) {
            // allocation is valid
            *found = SAILOR__bt__true;

            return output;
        }

        // next allocation
        current.start += sizeof(SAILOR__buffer);
        output += sizeof(SAILOR__buffer);
    }

    // not valid
    *found = SAILOR__bt__false;

    return output;
}

// add a buffer
void SAILOR__remember__allocation(SAILOR__allocations* allocations, SAILOR__buffer allocation, SAILOR__bt* list_error_occured) {
    // append allocation
    SAILOR__list__append__buffer(&((*allocations).buffers), allocation, list_error_occured);

    return;
}

// remove a buffer
void SAILOR__forget__allocation(SAILOR__allocations* allocations, SAILOR__buffer allocation, SAILOR__bt* buffer_did_not_exist) {
    SAILOR__bt found = SAILOR__bt__false;

    // find allocation index
    SAILOR__list_filled_index start_index = SAILOR__find__allocation(allocations, allocation, &found);

    // if found
    if (found == SAILOR__bt__true) {
        // erase from list
        SAILOR__list__erase__space(&((*allocations).buffers), start_index, start_index + sizeof(SAILOR__buffer) - 1);

        // set error
        *buffer_did_not_exist = SAILOR__bt__false;
    // not found, so not removed
    } else {
        // set error
        *buffer_did_not_exist = SAILOR__bt__true;
    }

    return;
}

// search for and return a valid allocation buffer from another buffer
SAILOR__buffer SAILOR__search__valid_allocation_from_sub_buffer(SAILOR__allocations* allocations, SAILOR__buffer range, SAILOR__bt* found) {
    SAILOR__buffer current;
    
    // setup current
    current = SAILOR__calculate__list_current_buffer(&(*allocations).buffers);

    // check for valid allocation range
    while (current.start <= current.end) {
        // check one allocation
        if (SAILOR__calculate__buffer_range_in_buffer_range_inclusive(*(SAILOR__buffer*)current.start, range)) {
            // allocation is valid
            *found = SAILOR__bt__true;
            return *(SAILOR__buffer*)current.start;
        }
        
        // next allocation
        current.start += sizeof(SAILOR__buffer);
    }

    // allocation not valid
    *found = SAILOR__bt__false;
    return SAILOR__create_null__buffer();
}

// check to see if an address is valid
SAILOR__bt SAILOR__check__valid_address_range_in_allocations(SAILOR__bt catch_addresses, SAILOR__allocations* allocations, SAILOR__buffer range) {
    SAILOR__buffer current;

    // check if not catching addresses
    if (catch_addresses == SAILOR__bt__false) {
        return SAILOR__bt__true;
    }
    
    // setup current
    current = SAILOR__calculate__list_current_buffer(&(*allocations).buffers);

    // check for valid allocation range
    while (current.start <= current.end) {
        // check one allocation
        if (SAILOR__calculate__buffer_range_in_buffer_range_inclusive(*(SAILOR__buffer*)current.start, range)) {
            // allocation is valid
            return SAILOR__bt__true;
        }
        
        // next allocation
        current.start += sizeof(SAILOR__buffer);
    }

    return SAILOR__bt__false;
}

// open allocations
SAILOR__allocations SAILOR__open__allocations(SAILOR__bt* memory_error_occured) {
    SAILOR__allocations output;

    // setup output
    output.buffers = SAILOR__open__list(sizeof(SAILOR__buffer) * 256, memory_error_occured);

    return output;
}

// close allocations (does NOT clear actual allocations)
void SAILOR__close__allocations(SAILOR__allocations* allocations) {
    // clean up
    SAILOR__close__list((*allocations).buffers);

    return;
}

// print allocations
void SAILOR__print__allocations(SAILOR__allocations allocations) {
    // print header
    printf("Allocations:\n");

    // setup current
    SAILOR__current current_allocation = SAILOR__calculate__current_from_list_filled_index(&allocations.buffers);

    // for each allocation
    while (SAILOR__check__current_within_range(current_allocation)) {
        // get allocation
        SAILOR__buffer allocation = *(SAILOR__buffer*)current_allocation.start;

        // print allocation
        printf("\t[ %lu %lu ] (%li)\n", (SAILOR__cell_integer_value)allocation.start, (SAILOR__cell_integer_value)allocation.end, (SAILOR__cell_integer_value)allocation.end - (SAILOR__cell_integer_value)allocation.start + 1);

        // next allocation
        current_allocation.start += sizeof(SAILOR__buffer);
    }

    return;
}

/* Helper Functions */
// convert instruction type to instruction length type
SAILOR__ilt SAILOR__convert__it_to_ilt(SAILOR__it instruction) {
    SAILOR__ilt lengths[] = {
        SAILOR__ilt__stop,
        SAILOR__ilt__write_cell,
        SAILOR__ilt__operate,
        SAILOR__ilt__request_memory,
        SAILOR__ilt__return_memory,
        SAILOR__ilt__address_to_cell,
        SAILOR__ilt__cell_to_address,
        SAILOR__ilt__file_to_buffer,
        SAILOR__ilt__buffer_to_file,
        SAILOR__ilt__delete_file,
        SAILOR__ilt__buffer_to_buffer__low_to_high,
        SAILOR__ilt__buffer_to_buffer__high_to_low,
        SAILOR__ilt__compile,
        SAILOR__ilt__run,
        SAILOR__ilt__get_time,
        SAILOR__ilt__debug__putchar,
        SAILOR__ilt__debug__fgets,
        SAILOR__ilt__debug__mark_data_section,
        SAILOR__ilt__debug__mark_code_section,
        SAILOR__ilt__debug__get_current_context,
        SAILOR__ilt__debug__search_for_allocation,
    };

    return lengths[instruction];
}

// get a pointer from a context to a cell inside that context
SAILOR__cell* SAILOR__get__cell_address_from_context(SAILOR__context* context, SAILOR__cell_ID cell_ID) {
    // return data
    return &((*context).cells[cell_ID]);
}

// get cell value from context
SAILOR__cell SAILOR__get__cell_from_context(SAILOR__context* context, SAILOR__cell_ID cell_ID) {
    // return data
    return *SAILOR__get__cell_address_from_context(context, cell_ID);
}

// set cell value by pointer
void SAILOR__set__cell_by_address(SAILOR__cell* destination_cell, SAILOR__cell value) {
    // set value
    *destination_cell = value;

    return;
}

// set value in error code cell
void SAILOR__set__error_code_cell(SAILOR__context* context, SAILOR__et error_code) {
    // set error
    SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, SAILOR__rt__error_code), (SAILOR__cell)error_code);

    return;
}

// get flag (value)
SAILOR__bt SAILOR__get__flag_from_context(SAILOR__context* context, SAILOR__flag_ID flag_ID) {
    SAILOR__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // return bit
    return (SAILOR__bt)((((SAILOR__u8*)&((*context).cells[SAILOR__rt__flags_0]))[flag_ID / 8] & masks[flag_ID % 8]) > 0);
}

// set flag (value)
void SAILOR__set__flag_in_context(SAILOR__context* context, SAILOR__flag_ID flag_ID, SAILOR__bt value) {
    SAILOR__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // clear bit
    ((((SAILOR__u8*)&((*context).cells[SAILOR__rt__flags_0]))[flag_ID / 8] &= (~masks[flag_ID % 8])));

    // set bit
    ((SAILOR__u8*)&((*context).cells[SAILOR__rt__flags_0]))[flag_ID / 8] |= (value << (flag_ID % 8));

    return;
}

// read item from buffer and advance read address
SAILOR__u64 SAILOR__read_next__buffer_item(SAILOR__cell* address, SAILOR__byte_count byte_count) {
    SAILOR__u64 output;

    // read data from buffer
    output = SAILOR__read__buffer(*address, byte_count);

    // advance pointer
    *address = (SAILOR__address)(*((u8**)address) + byte_count);

    // return data
    return output;
}

// get cell value and advance
SAILOR__cell SAILOR__read_next__cell(SAILOR__cell* address) {
    // read data
    return (SAILOR__cell)SAILOR__read_next__buffer_item(address, sizeof(SAILOR__cell));
}

// get instruction ID and advance
SAILOR__instruction_ID SAILOR__read_next__instruction_ID(SAILOR__cell* address) {
    // read data
    return (SAILOR__instruction_ID)SAILOR__read_next__buffer_item(address, sizeof(SAILOR__instruction_ID));
}

// get flag ID and advance
SAILOR__flag_ID SAILOR__read_next__flag_ID(SAILOR__cell* address) {
    // read data
    return (SAILOR__flag_ID)SAILOR__read_next__buffer_item(address, sizeof(SAILOR__flag_ID));
}

// get operation ID and advance
SAILOR__operation_ID SAILOR__read_next__operation_ID(SAILOR__cell* address) {
    // read data
    return (SAILOR__operation_ID)SAILOR__read_next__buffer_item(address, sizeof(SAILOR__operation_ID));
}

// get cell ID and advance
SAILOR__cell_ID SAILOR__read_next__cell_ID(SAILOR__cell* address) {
    // read data
    return (SAILOR__cell_ID)SAILOR__read_next__buffer_item(address, sizeof(SAILOR__cell_ID));
}

/* Setup Alloy Code */
// create a skeleton context
SAILOR__context SAILOR__setup__context(SAILOR__buffer program) {
    SAILOR__context output;

    // setup program and execution cells
    output.cells[SAILOR__rt__program_start_address] = (SAILOR__cell)program.start;
    output.cells[SAILOR__rt__program_end_address] = (SAILOR__cell)program.end;
    output.cells[SAILOR__rt__program_current_address] = output.cells[SAILOR__rt__program_start_address];

    return output;
}

/* Run Alloy Code */
// predefined run context header
void SAILOR__run__context(SAILOR__allocations* allocations, SAILOR__context* context, SAILOR__instruction_count instruction_count);

// check compiler instruction inputs
SAILOR__bt COMPILER__check__valid_compilation_instruction_inputs(SAILOR__allocations* allocations, SAILOR__bt catch_addresses, SAILOR__buffer input_buffer) {
    // don't validate if catch addresses is disabled
    if (catch_addresses == SAILOR__bt__false) {
        // ignore validation
        return SAILOR__bt__true;
    }

    // check for valid buffer of buffers
    // check for allocation
    if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, input_buffer) == SAILOR__bt__false) {
        // invalid
        return SAILOR__bt__false;
    }

    // check for valid buffer of buffers
    // check size
    if ((SAILOR__calculate__buffer_length(input_buffer) % sizeof(SAILOR__buffer)) != 0) {
        // invalid
        return SAILOR__bt__false;
    }

    // check each user code buffer for address validity
    // setup current
    SAILOR__current current = input_buffer;

    // for each sub buffer
    while (SAILOR__check__current_within_range(current) == SAILOR__bt__true) {
        // get allocation
        SAILOR__buffer temp_buffer = *(SAILOR__buffer*)current.start;

        // validate buffer
        SAILOR__bt valid = SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, temp_buffer);
        if (valid == SAILOR__bt__false) {
            // invalid buffer
            return SAILOR__bt__false;
        }

        // next allocation
        current.start += sizeof(SAILOR__buffer);
    }

    // no issues found, validated
    return SAILOR__bt__true;
}

// process operation (assumes flag was checked)
SAILOR__nit SAILOR__run__operation(SAILOR__context* context, SAILOR__ot operation_type, SAILOR__cell_ID input_0, SAILOR__cell_ID input_1, SAILOR__cell_ID input_2, SAILOR__cell_ID output_0) {
    SAILOR__cell_integer_value temp_input_0;
    SAILOR__cell_integer_value temp_input_1;
    SAILOR__cell_integer_value temp_input_2;
    SAILOR__cell_integer_value temp_result;

    // do operation based on type
    switch (operation_type) {
    // cell to cell
    case SAILOR__ot__cell_to_cell:
        // set value
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), SAILOR__get__cell_from_context(context, input_0));

        break;
    // binary or
    case SAILOR__ot__bits_or:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 | temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary invert
    case SAILOR__ot__bits_invert:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);

        // perform operation
        temp_result = ~temp_input_0;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary and
    case SAILOR__ot__bits_and:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 & temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary xor
    case SAILOR__ot__bits_xor:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 ^ temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary bit shift higher
    case SAILOR__ot__bits_shift_higher:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 << temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary bit shift lower
    case SAILOR__ot__bits_shift_lower:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 >> temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // binary overwrite bits
    case SAILOR__ot__bits_overwrite:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // mask (positive bits are the ones being overwritten!)
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1); // old bits
        temp_input_2 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_2); // new bits

        // perform operation
        temp_result = (~temp_input_0) & temp_input_1;
        temp_result = temp_result | (temp_input_2 & temp_input_0);

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer addition
    case SAILOR__ot__integer_add:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 + temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer subtraction
    case SAILOR__ot__integer_subtract:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 - temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer multiplication
    case SAILOR__ot__integer_multiply:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 * temp_input_1;

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer division
    case SAILOR__ot__integer_division:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // check if division is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 / temp_input_1;
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__divide_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer modulous
    case SAILOR__ot__integer_modulous:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0);
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1);

        // check if modulous is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 % temp_input_1;
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__modulous_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // integer range check
    case SAILOR__ot__integer_within_range:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // range start
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1); // value to be checked
        temp_input_2 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_2); // range end

        // check range
        temp_result = (SAILOR__cell_integer_value)((temp_input_0 <= temp_input_1) && (temp_input_1 <= temp_input_2));

        // set result cell
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)temp_result);

        break;
    // flag or
    case SAILOR__ot__flag_or:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1); // second flag
        temp_result = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, output_0); // output flag

        // 'or' flags into new flag
        SAILOR__set__flag_in_context(context, temp_result, (SAILOR__bt)(SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_0) | SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_1)));

        break;
    // flag invert
    case SAILOR__ot__flag_invert:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // first flag
        temp_result = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, output_0); // output flag

        // invert flag
        SAILOR__set__flag_in_context(context, temp_result, (SAILOR__bt)!(SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_0)));

        break;
    // flag and
    case SAILOR__ot__flag_and:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1); // second flag
        temp_result = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, output_0); // output flag

        // 'and' flags into new flag
        SAILOR__set__flag_in_context(context, temp_result, (SAILOR__bt)(SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_0) & SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_1)));

        break;
    // flag xor
    case SAILOR__ot__flag_xor:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_1); // second flag
        temp_result = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, output_0); // output flag

        // 'xor' flags into new flag
        SAILOR__set__flag_in_context(context, temp_result, (SAILOR__bt)(SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_0) != SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_1)));

        break;
    // flag get
    case SAILOR__ot__flag_get:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // flag address

        // get flag
        SAILOR__set__cell_by_address(SAILOR__get__cell_address_from_context(context, output_0), (SAILOR__cell)(SAILOR__u64)(SAILOR__get__flag_from_context(context, (SAILOR__flag_ID)temp_input_0) > 0));

        break;
    // flag set
    case SAILOR__ot__flag_set:
        // get data
        temp_input_0 = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, input_0); // flag value
        temp_result = (SAILOR__cell_integer_value)SAILOR__get__cell_from_context(context, output_0); // destination flag ID

        // set flag
        SAILOR__set__flag_in_context(context, (SAILOR__flag_ID)temp_result, (SAILOR__bt)temp_input_0);

        break;
    // operation ID was invalid
    default:
        // return failed instruction
        return SAILOR__nit__return_context;
    }

    return SAILOR__nit__next_instruction;
}

// process instruction
SAILOR__nit SAILOR__run__instruction(SAILOR__allocations* allocations, SAILOR__context* context) {
    // execution current read address
    SAILOR__cell* execution_read_address;

    // address catching toggle
    SAILOR__bt catch_addresses;

    // instruction ID
    SAILOR__it instruction_ID;

    // write cell temps
    SAILOR__cell write_cell__cell;
    SAILOR__cell_ID write_cell__cell_ID;

    // operate temps
    SAILOR__flag_ID operate__flag_ID;
    SAILOR__operation_ID operate__operation_ID;
    SAILOR__cell_ID operate__input_cell_ID_0;
    SAILOR__cell_ID operate__input_cell_ID_1;
    SAILOR__cell_ID operate__input_cell_ID_2;
    SAILOR__cell_ID operate__output_cell_ID_0;

    // request memory temps
    SAILOR__cell_ID request_memory__allocation_size;
    SAILOR__cell_ID request_memory__allocation_start;
    SAILOR__cell_ID request_memory__allocation_end;
    SAILOR__buffer request_memory__allocation;
    SAILOR__bt request_memory__buffer_appending_error = SAILOR__bt__false;

    // return memory temps
    SAILOR__cell_ID return_memory__allocation_start;
    SAILOR__cell_ID return_memory__allocation_end;
    SAILOR__buffer return_memory__allocation;
    SAILOR__bt return_memory__allocation_does_not_exist = SAILOR__bt__false;

    // address to cell temps
    SAILOR__flag_ID address_to_cell__flag_ID;
    SAILOR__length address_to_cell__source_cell_ID;
    SAILOR__length address_to_cell__byte_count_cell_ID;
    SAILOR__length address_to_cell__destination_cell_ID;

    // cell to address temps
    SAILOR__flag_ID cell_to_address__flag_ID;
    SAILOR__length cell_to_address__source_cell_ID;
    SAILOR__length cell_to_address__byte_count_cell_ID;
    SAILOR__length cell_to_address__destination_cell_ID;

    // file to buffer temps
    SAILOR__cell_ID file_to_buffer__file_name_start;
    SAILOR__cell_ID file_to_buffer__file_name_end;
    SAILOR__cell_ID file_to_buffer__file_data_start;
    SAILOR__cell_ID file_to_buffer__file_data_end;
    SAILOR__buffer file_to_buffer__file_name;
    SAILOR__buffer file_to_buffer__file_data;
    SAILOR__bt file_to_buffer__buffer_appending_error = SAILOR__bt__false;

    // buffer to file temps
    SAILOR__cell_ID buffer_to_file__file_data_start;
    SAILOR__cell_ID buffer_to_file__file_data_end;
    SAILOR__cell_ID buffer_to_file__file_name_start;
    SAILOR__cell_ID buffer_to_file__file_name_end;
    SAILOR__buffer buffer_to_file__file_data;
    SAILOR__buffer buffer_to_file__file_name;
    SAILOR__bt buffer_to_file__error = SAILOR__bt__false;

    // delete file temps
    SAILOR__cell_ID delete_file__file_path_start;
    SAILOR__cell_ID delete_file__file_path_end;
    SAILOR__buffer delete_file__file_path;
    SAILOR__bt delete_file__error = SAILOR__bt__false;

    // buffer to buffer temps
    SAILOR__cell_ID buffer_to_buffer__source_start;
    SAILOR__cell_ID buffer_to_buffer__source_end;
    SAILOR__cell_ID buffer_to_buffer__destination_start;
    SAILOR__cell_ID buffer_to_buffer__destination_end;
    SAILOR__buffer buffer_to_buffer__source;
    SAILOR__buffer buffer_to_buffer__destination;
    SAILOR__bt buffer_to_buffer__error = SAILOR__bt__false;

    // compile temps
    SAILOR__cell_ID compile__user_code_buffers_buffer_start;
    SAILOR__cell_ID compile__user_code_buffers_buffer_end;
    SAILOR__cell_ID compile__include_standard;
    SAILOR__cell_ID compile__debug_enabled;
    SAILOR__cell_ID compile__generate_kickstarter;
    SAILOR__cell_ID compile__output_start;
    SAILOR__cell_ID compile__output_end;
    SAILOR__cell_ID compile__debug_start;
    SAILOR__cell_ID compile__debug_end;
    SAILOR__cell_ID compile__error__occured;
    SAILOR__cell_ID compile__error__message_start;
    SAILOR__cell_ID compile__error__message_end;
    SAILOR__cell_ID compile__error__character_location__file_index;
    SAILOR__cell_ID compile__error__character_location__line_number;
    SAILOR__cell_ID compile__error__character_location__character_index;
    SAILOR__buffer compile__output_program;
    SAILOR__buffer compile__debug_information;
    SAILOR__buffer compile__output_error_message;
    COMPILER__error compile__error;
    SAILOR__bt compile__buffer_appending_error = SAILOR__bt__false;

    // run temps
    SAILOR__cell_ID run__context_buffer_start;
    SAILOR__cell_ID run__context_buffer_end;
    SAILOR__cell_ID run__instruction_count;

    // get time temps
    SAILOR__cell_ID get_time__seconds;
    SAILOR__cell_ID get_time__nanoseconds;
    struct timespec get_time__data_destination;

    // debug putchar temps
    SAILOR__cell_ID debug__putchar__printing_cell_ID;

    // debug fgets temps
    SAILOR__cell_ID debug__fgets__buffer_address_start;
    SAILOR__cell_ID debug__fgets__buffer_address_end;
    SAILOR__u8 debug__fgets__temporary_string[SAILOR__define__input_string_max_length];
    SAILOR__buffer debug__fgets__buffer;
    SAILOR__length debug__fgets__buffer_length;
    SAILOR__bt debug__fgets__buffer_appending_error = SAILOR__bt__false;

    // debug mark data section temps
    SAILOR__cell debug__mark_data_section__section_length;

    // debug mark code section temps
    SAILOR__cell debug__mark_code_section__section_length;

    // debug get current context
    SAILOR__cell_ID get_current_context__buffer_start;
    SAILOR__cell_ID get_current_context__buffer_end;

    // debug search for allocation
    SAILOR__cell_ID search_for_allocation__source_buffer_start;
    SAILOR__cell_ID search_for_allocation__source_buffer_end;
    SAILOR__buffer search_for_allocation__source_buffer;
    SAILOR__cell_ID search_for_allocation__was_found;
    SAILOR__bt search_for_allocation__was_found_boolean;
    SAILOR__cell_ID search_for_allocation__found_buffer_start;
    SAILOR__cell_ID search_for_allocation__found_buffer_end;
    SAILOR__buffer search_for_allocation__found_buffer;

    // setup address catcher toggle
    catch_addresses = (SAILOR__bt)(SAILOR__u64)SAILOR__get__cell_from_context(context, SAILOR__rt__address_catch_toggle);

    // setup execution read address
    execution_read_address = SAILOR__get__cell_address_from_context(context, SAILOR__rt__program_current_address);

    // check for no more instructions
    if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, SAILOR__create__buffer((*context).cells[SAILOR__rt__program_current_address], (*context).cells[SAILOR__rt__program_current_address] + sizeof(SAILOR__it) - 1)) == SAILOR__bt__false) {
        // set error code
        SAILOR__set__error_code_cell(context, SAILOR__et__program_ran_out_of_instructions);

        // allocation does not exist, quit
        return SAILOR__nit__return_context;
    }

    // get instruction ID from program
    instruction_ID = (SAILOR__it)SAILOR__read_next__instruction_ID(execution_read_address);

    // check for no more instruction parameters
    if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, SAILOR__create__buffer((*context).cells[SAILOR__rt__program_current_address], (*context).cells[SAILOR__rt__program_current_address] + SAILOR__convert__it_to_ilt(instruction_ID) - sizeof(SAILOR__it) - 1)) == SAILOR__bt__false) {
        // set error code
        SAILOR__set__error_code_cell(context, SAILOR__et__program_ran_out_of_instruction_parameters);

        // allocation does not exist, quit
        return SAILOR__nit__return_context;
    }

    // DEBUG
    // printf("[%lu]: instruction_ID: %lu\n", (SAILOR__u64)(*execution_read_address) - 1, (SAILOR__u64)instruction_ID);

    // check for valid instruction ID
    if (instruction_ID >= SAILOR__it__COUNT) {
        // set error
        SAILOR__set__error_code_cell(context, SAILOR__et__invalid_instruction_ID);

        // return exit context
        return SAILOR__nit__return_context;
    }

    // process instruction accordingly
    switch (instruction_ID) {
    // if context should stop
    case SAILOR__it__stop:
        // return exit context
        return SAILOR__nit__return_context;
    // overwrite cell value
    case SAILOR__it__write_cell:
        // get parameters
        write_cell__cell = SAILOR__read_next__cell(execution_read_address);
        write_cell__cell_ID = SAILOR__read_next__cell_ID(execution_read_address);

        // do action
        (*context).cells[write_cell__cell_ID] = write_cell__cell;

        break;
    // operate between cells
    case SAILOR__it__operate:
        // get parameters
        operate__flag_ID = SAILOR__read_next__flag_ID(execution_read_address);
        operate__operation_ID = SAILOR__read_next__operation_ID(execution_read_address);
        operate__input_cell_ID_0 = SAILOR__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_1 = SAILOR__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_2 = SAILOR__read_next__cell_ID(execution_read_address);
        operate__output_cell_ID_0 = SAILOR__read_next__cell_ID(execution_read_address);

        // if flag enabled
        if (SAILOR__get__flag_from_context(context, operate__flag_ID) == SAILOR__bt__true) {
            // perform operation
            return SAILOR__run__operation(context, (SAILOR__ot)operate__operation_ID, operate__input_cell_ID_0, operate__input_cell_ID_1, operate__input_cell_ID_2, operate__output_cell_ID_0);
        }

        break;
    // ask os for new buffer
    case SAILOR__it__request_memory:
        // get parameters
        request_memory__allocation_size = SAILOR__read_next__cell_ID(execution_read_address);
        request_memory__allocation_start = SAILOR__read_next__cell_ID(execution_read_address);
        request_memory__allocation_end = SAILOR__read_next__cell_ID(execution_read_address);

        // do action
        if ((SAILOR__length)(*context).cells[request_memory__allocation_size] <= SAILOR__define__max_allocation_size) {
            // allocate
            request_memory__allocation = SAILOR__open__buffer((SAILOR__length)(*context).cells[request_memory__allocation_size]);
        } else {
            // set buffer
            request_memory__allocation = SAILOR__create_null__buffer();
            
            // set internal error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_allocation__requested_memory_too_large);
        }

        // add allocation if successful
        if (SAILOR__check__empty_buffer(request_memory__allocation) == SAILOR__bt__false) {
            // remember allocation
            SAILOR__remember__allocation(allocations, request_memory__allocation, &request_memory__buffer_appending_error);

            // if buffer could not be added
            if (request_memory__buffer_appending_error == SAILOR__bt__true) {
                // set internal error
                SAILOR__set__error_code_cell(context, SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                SAILOR__close__buffer(request_memory__allocation);

                // setup empty buffer
                request_memory__allocation = SAILOR__create_null__buffer();
            }
        }

        // set cell data
        (*context).cells[request_memory__allocation_start] = request_memory__allocation.start;
        (*context).cells[request_memory__allocation_end] = request_memory__allocation.end;

        break;
    // return buffer to OS
    case SAILOR__it__return_memory:
        // get parameters
        return_memory__allocation_start = SAILOR__read_next__cell_ID(execution_read_address);
        return_memory__allocation_end = SAILOR__read_next__cell_ID(execution_read_address);

        // get parameters
        return_memory__allocation.start = (*context).cells[return_memory__allocation_start];
        return_memory__allocation.end = (*context).cells[return_memory__allocation_end];

        // remove buffer from valid allocation list
        SAILOR__forget__allocation(allocations, return_memory__allocation, &return_memory__allocation_does_not_exist);

        // if allocation existed
        if (return_memory__allocation_does_not_exist == SAILOR__bt__false) {
            // deallocate
            SAILOR__close__buffer(return_memory__allocation);
        // allocation did not exist
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // take data from an address and put it into a cell
    case SAILOR__it__address_to_cell:
        // get parameters
        address_to_cell__flag_ID = SAILOR__read_next__flag_ID(execution_read_address);
        address_to_cell__source_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);
        address_to_cell__byte_count_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);
        address_to_cell__destination_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, SAILOR__create__buffer((SAILOR__address)(*context).cells[address_to_cell__source_cell_ID], (SAILOR__address)(*context).cells[address_to_cell__source_cell_ID] + (((SAILOR__length)(*context).cells[address_to_cell__byte_count_cell_ID])) - 1))) {
            // if flag enabled
            if (SAILOR__get__flag_from_context(context, address_to_cell__flag_ID) == SAILOR__bt__true) {
                // read data into cell
                (*context).cells[address_to_cell__destination_cell_ID] = (SAILOR__cell)SAILOR__read__buffer((SAILOR__address)(*context).cells[address_to_cell__source_cell_ID], ((SAILOR__length)(*context).cells[address_to_cell__byte_count_cell_ID]));
            }
        // invalid address range
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_address_range);

            // setup blank value
            (*context).cells[address_to_cell__destination_cell_ID] = SAILOR__define__null_address;
        }

        break;
    // take data from a cell and put it at an address
    case SAILOR__it__cell_to_address:
        // get parameters
        cell_to_address__flag_ID = SAILOR__read_next__flag_ID(execution_read_address);
        cell_to_address__source_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);
        cell_to_address__byte_count_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);
        cell_to_address__destination_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, SAILOR__create__buffer((*context).cells[cell_to_address__destination_cell_ID], (*context).cells[cell_to_address__destination_cell_ID] + (((SAILOR__length)(*context).cells[cell_to_address__byte_count_cell_ID])) - 1))) {
            // if flag enabled
            if (SAILOR__get__flag_from_context(context, cell_to_address__flag_ID) == SAILOR__bt__true) {
                // write data to an address
                SAILOR__write__buffer((SAILOR__u64)(*context).cells[cell_to_address__source_cell_ID], ((SAILOR__length)(*context).cells[cell_to_address__byte_count_cell_ID]), (*context).cells[cell_to_address__destination_cell_ID]);
            }
        // invalid address range
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_address_range);
        }

        break;
    // take data from a file and create a buffer with it
    case SAILOR__it__file_to_buffer:
        // get parameters
        file_to_buffer__file_name_start = SAILOR__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_name_end = SAILOR__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_start = SAILOR__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup temps
        file_to_buffer__file_name.start = (*context).cells[file_to_buffer__file_name_start];
        file_to_buffer__file_name.end = (*context).cells[file_to_buffer__file_name_end];

        // get data from file
        file_to_buffer__file_data = SAILOR__move__file_to_buffer(file_to_buffer__file_name);

        // check for errors
        if (file_to_buffer__file_data.start == SAILOR__define__null_address) {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__file_not_found);
        // append buffer as valid allocation
        } else {
            // remember allocation
            SAILOR__remember__allocation(allocations, file_to_buffer__file_data, &file_to_buffer__buffer_appending_error);

            // if buffer could not be added
            if (file_to_buffer__buffer_appending_error == SAILOR__bt__true) {
                // set internal error
                SAILOR__set__error_code_cell(context, SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                SAILOR__close__buffer(file_to_buffer__file_data);

                // setup empty buffer
                file_to_buffer__file_data = SAILOR__create_null__buffer();
            }
        }

        // write data to cells
        (*context).cells[file_to_buffer__file_data_start] = file_to_buffer__file_data.start;
        (*context).cells[file_to_buffer__file_data_end] = file_to_buffer__file_data.end;

        break;
    // take a buffer and overwrite a file with it
    case SAILOR__it__buffer_to_file:
        // get parameters
        buffer_to_file__file_data_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_data_end = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_file__file_data.start = (*context).cells[buffer_to_file__file_data_start];
        buffer_to_file__file_data.end = (*context).cells[buffer_to_file__file_data_end];
        buffer_to_file__file_name.start = (*context).cells[buffer_to_file__file_name_start];
        buffer_to_file__file_name.end = (*context).cells[buffer_to_file__file_name_end];

        // if source allocations exists
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_file__file_data) && SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_file__file_name)) {
            // create file
            SAILOR__move__buffer_to_file(&buffer_to_file__error, buffer_to_file__file_name, buffer_to_file__file_data);

            // check for errors
            if (buffer_to_file__error == SAILOR__bt__true) {
                // set error
                SAILOR__set__error_code_cell(context, SAILOR__et__file_not_created);
            }
        // if any allocations do not exist
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // take a file path and delete it
    case SAILOR__it__delete_file:
        // get parameters
        delete_file__file_path_start = SAILOR__read_next__cell_ID(execution_read_address);
        delete_file__file_path_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup temps
        delete_file__file_path.start = (*context).cells[delete_file__file_path_start];
        delete_file__file_path.end = (*context).cells[delete_file__file_path_end];

        // remove file
        SAILOR__delete__file(&delete_file__error, delete_file__file_path);

        // check for error
        if (delete_file__error == SAILOR__bt__true) {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__file_not_found);
        }

        break;
    // copy one buffer of data from one area to another of equal size copying bytes low to high
    case SAILOR__it__buffer_to_buffer__low_to_high:
        // get parameters
        buffer_to_buffer__source_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__source_end = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_buffer__source.start = (*context).cells[buffer_to_buffer__source_start];
        buffer_to_buffer__source.end = (*context).cells[buffer_to_buffer__source_end];
        buffer_to_buffer__destination.start = (*context).cells[buffer_to_buffer__destination_start];
        buffer_to_buffer__destination.end = (*context).cells[buffer_to_buffer__destination_end];

        // if both allocations exist
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__source) && SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__destination)) {
            // perform copy
            SAILOR__copy__buffer(buffer_to_buffer__source, buffer_to_buffer__destination, &buffer_to_buffer__error);

            // check for error
            if (buffer_to_buffer__error == SAILOR__bt__true) {
                SAILOR__set__error_code_cell(context, SAILOR__et__buffer_to_buffer__buffers_are_different_sizes);
            }
        // if one allocation does not exist
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // copy one buffer of data from one area to another of equal size copying bytes low to high
    case SAILOR__it__buffer_to_buffer__high_to_low:
        // get parameters
        buffer_to_buffer__source_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__source_end = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_start = SAILOR__read_next__cell_ID(execution_read_address);
        buffer_to_buffer__destination_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_buffer__source.start = (*context).cells[buffer_to_buffer__source_start];
        buffer_to_buffer__source.end = (*context).cells[buffer_to_buffer__source_end];
        buffer_to_buffer__destination.start = (*context).cells[buffer_to_buffer__destination_start];
        buffer_to_buffer__destination.end = (*context).cells[buffer_to_buffer__destination_end];

        // if both allocations exist
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__source) && SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, buffer_to_buffer__destination)) {
            // perform copy
            SAILOR__copy__buffer__backwards(buffer_to_buffer__source, buffer_to_buffer__destination, &buffer_to_buffer__error);

            // check for error
            if (buffer_to_buffer__error == SAILOR__bt__true) {
                SAILOR__set__error_code_cell(context, SAILOR__et__buffer_to_buffer__buffers_are_different_sizes);
            }
        // if one allocation does not exist
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // compile program
    case SAILOR__it__compile:
        // get parameters
        compile__user_code_buffers_buffer_start = SAILOR__read_next__cell_ID(execution_read_address);
        compile__user_code_buffers_buffer_end = SAILOR__read_next__cell_ID(execution_read_address);
        compile__include_standard = SAILOR__read_next__cell_ID(execution_read_address);
        compile__debug_enabled = SAILOR__read_next__cell_ID(execution_read_address);
        compile__generate_kickstarter = SAILOR__read_next__cell_ID(execution_read_address);
        compile__output_start = SAILOR__read_next__cell_ID(execution_read_address);
        compile__output_end = SAILOR__read_next__cell_ID(execution_read_address);
        compile__debug_start = SAILOR__read_next__cell_ID(execution_read_address);
        compile__debug_end = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__occured = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__message_start = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__message_end = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__character_location__file_index = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__character_location__line_number = SAILOR__read_next__cell_ID(execution_read_address);
        compile__error__character_location__character_index = SAILOR__read_next__cell_ID(execution_read_address);

        // null init error
        compile__error = COMPILER__create_null__error();

        // validate input buffers
        // validate
        SAILOR__bt compile__valid_inputs = COMPILER__check__valid_compilation_instruction_inputs(
            allocations,
            catch_addresses,
            SAILOR__create__buffer(
                (*context).cells[compile__user_code_buffers_buffer_start],
                (*context).cells[compile__user_code_buffers_buffer_end]
            )
        );

        // if valid
        if (compile__valid_inputs == SAILOR__bt__true) {
            // run compiler
            COMPILER__compile__files(
                SAILOR__create__buffer(
                    (*context).cells[compile__user_code_buffers_buffer_start],
                    (*context).cells[compile__user_code_buffers_buffer_end]
                ),
                (SAILOR__bt)(SAILOR__cell_integer_value)(*context).cells[compile__include_standard],
                (SAILOR__bt)(SAILOR__cell_integer_value)(*context).cells[compile__generate_kickstarter],
                (SAILOR__bt)(SAILOR__cell_integer_value)(*context).cells[compile__debug_enabled],
                (SAILOR__bt)(SAILOR__cell_integer_value)(*context).cells[compile__debug_enabled],
                &compile__output_program,
                &compile__debug_information,
                &compile__error
            );
        // invalid
        } else {
            // set internal error
            SAILOR__set__error_code_cell(context, SAILOR__et__compile__invalid_input_configuration);

            // zero out everything
            (*context).cells[compile__output_start] = SAILOR__define__null_address;
            (*context).cells[compile__output_end] = SAILOR__define__null_address;
            (*context).cells[compile__error__occured] = (SAILOR__cell)(SAILOR__cell_integer_value)SAILOR__bt__true;
            (*context).cells[compile__error__message_start] = SAILOR__define__null_address;
            (*context).cells[compile__error__message_end] = SAILOR__define__null_address;
            (*context).cells[compile__error__character_location__file_index] = (SAILOR__cell)SAILOR__define__null_file_index_ID;
            (*context).cells[compile__error__character_location__line_number] = (SAILOR__cell)SAILOR__define__null_address;
            (*context).cells[compile__error__character_location__character_index] = (SAILOR__cell)SAILOR__define__null_address;

            break;
        }

        // get temps
        compile__output_error_message = compile__error.message;

        // check if program was created
        if (COMPILER__check__error_occured(&compile__error) == SAILOR__bt__false) {
            // add new buffer to allocations
            SAILOR__remember__allocation(allocations, compile__output_program, &compile__buffer_appending_error);

            // if buffer could not be added
            if (compile__buffer_appending_error == SAILOR__bt__true) {
                // set internal error
                SAILOR__set__error_code_cell(context, SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                SAILOR__close__buffer(compile__output_program);

                // setup empty buffer
                compile__output_program = SAILOR__create_null__buffer();
            }
        // program was not created
        } else {
            // setup empty buffer
            compile__output_program = SAILOR__create_null__buffer();

            // set error code
            SAILOR__set__error_code_cell(context, SAILOR__et__compile__compilation_error);

            // add error message to allocations
            SAILOR__remember__allocation(allocations, compile__output_error_message, &compile__buffer_appending_error);

            // if buffer could not be added
            if (compile__buffer_appending_error == SAILOR__bt__true) {
                // set internal error
                SAILOR__set__error_code_cell(context, SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                SAILOR__close__buffer(compile__output_error_message);

                // setup empty buffer
                compile__output_error_message = SAILOR__create_null__buffer();
            }
        }

        // setup outputs
        (*context).cells[compile__output_start] = compile__output_program.start;
        (*context).cells[compile__output_end] = compile__output_program.end;
        (*context).cells[compile__error__occured] = (SAILOR__cell)(SAILOR__cell_integer_value)compile__error.occured;
        (*context).cells[compile__error__message_start] = compile__output_error_message.start;
        (*context).cells[compile__error__message_end] = compile__output_error_message.end;
        (*context).cells[compile__error__character_location__file_index] = (SAILOR__cell)compile__error.location.file_index;
        (*context).cells[compile__error__character_location__line_number] = (SAILOR__cell)compile__error.location.line_number;
        (*context).cells[compile__error__character_location__character_index] = (SAILOR__cell)compile__error.location.character_index;

        break;
    // run a context like a program
    case SAILOR__it__run:
        // get parameters
        run__context_buffer_start = SAILOR__read_next__cell_ID(execution_read_address);
        run__context_buffer_end = SAILOR__read_next__cell_ID(execution_read_address);
        run__instruction_count = SAILOR__read_next__cell_ID(execution_read_address);

        // useless operation to quiet compiler
        run__context_buffer_end = run__context_buffer_end;

        // if context buffer is valid
        if (SAILOR__check__valid_address_range_in_allocations(catch_addresses, allocations, SAILOR__create__buffer((SAILOR__context*)(*context).cells[run__context_buffer_start], (SAILOR__context*)(*context).cells[run__context_buffer_end]))) {
            // run context
            SAILOR__run__context(allocations, (SAILOR__context*)(*context).cells[run__context_buffer_start], (u64)(*context).cells[run__instruction_count]);
        // error
        } else {
            // set error
            SAILOR__set__error_code_cell(context, SAILOR__et__invalid_address_range);
        }

        break;
    // get current time
    case SAILOR__it__get_time:
        // get parameters
        get_time__seconds = SAILOR__read_next__cell_ID(execution_read_address);
        get_time__nanoseconds = SAILOR__read_next__cell_ID(execution_read_address);

        // get time of day
        clock_gettime(CLOCK_REALTIME, &get_time__data_destination);

        // set variables
        (*context).cells[get_time__seconds] = (SAILOR__cell)get_time__data_destination.tv_sec;
        (*context).cells[get_time__nanoseconds] = (SAILOR__cell)get_time__data_destination.tv_nsec;

        break;
    // print one char to stdout
    case SAILOR__it__debug__putchar:
        // get parameters
        debug__putchar__printing_cell_ID = SAILOR__read_next__cell_ID(execution_read_address);

        // print
        putchar((u8)(u64)((*context).cells[debug__putchar__printing_cell_ID]));

        // flush stream for full update
        fflush(stdout);

        break;
    // read one string from stdin
    case SAILOR__it__debug__fgets:
        // get parameters
        debug__fgets__buffer_address_start = SAILOR__read_next__cell_ID(execution_read_address);
        debug__fgets__buffer_address_end = SAILOR__read_next__cell_ID(execution_read_address);

        // zero out temporaries
        for (SAILOR__length i = 0; i < SAILOR__define__input_string_max_length; i++) {
            // zero out character
            debug__fgets__temporary_string[i] = 0;
        }
        debug__fgets__buffer_length = 0;

        // read string
        fgets((char*)debug__fgets__temporary_string, SAILOR__define__input_string_max_length, stdin);

        // read buffer for string size
        while (debug__fgets__buffer_length < SAILOR__define__input_string_max_length && debug__fgets__temporary_string[debug__fgets__buffer_length] != 0) {
            // increment length
            debug__fgets__buffer_length++;
        }

        // create buffer based on string length
        debug__fgets__buffer = SAILOR__open__buffer(debug__fgets__buffer_length);

        // if buffer is not empty
        if (SAILOR__check__empty_buffer(debug__fgets__buffer) == SAILOR__bt__false) {
            // copy data into buffer
            for (SAILOR__u64 i = 0; i < debug__fgets__buffer_length; i++) {
                // write character
                SAILOR__write__buffer((u8)debug__fgets__temporary_string[i], sizeof(SAILOR__u8), (SAILOR__u8*)debug__fgets__buffer.start + (i * sizeof(SAILOR__u8)));
            }

            // remember allocation
            SAILOR__remember__allocation(allocations, debug__fgets__buffer, &debug__fgets__buffer_appending_error);

            // if buffer could not be added
            if (debug__fgets__buffer_appending_error == SAILOR__bt__true) {
                // set internal error
                SAILOR__set__error_code_cell(context, SAILOR__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                SAILOR__close__buffer(debug__fgets__buffer);

                // setup empty buffer
                debug__fgets__buffer = SAILOR__create_null__buffer();
            }
        }

        // setup cells
        (*context).cells[debug__fgets__buffer_address_start] = debug__fgets__buffer.start;
        (*context).cells[debug__fgets__buffer_address_end] = debug__fgets__buffer.end;

        break;
    // mark section of data
    case SAILOR__it__debug__mark_data_section:
        // get parameters
        debug__mark_data_section__section_length = SAILOR__read_next__cell(execution_read_address);

        // skip over data section
        (*context).cells[SAILOR__rt__program_current_address] = (SAILOR__address)((u64)(*context).cells[SAILOR__rt__program_current_address] + (u64)debug__mark_data_section__section_length);

        break;
    // mark section of code
    case SAILOR__it__debug__mark_code_section:
        // instruction does nothing but mark code space, so get length and do nothing
        debug__mark_code_section__section_length = SAILOR__read_next__cell(execution_read_address);

        // useless operation to quiet compiler
        debug__mark_code_section__section_length = debug__mark_code_section__section_length;

        break;
    // get current context as a buffer
    case SAILOR__it__debug__get_current_context:
        // get parameters
        get_current_context__buffer_start = SAILOR__read_next__cell_ID(execution_read_address);
        get_current_context__buffer_end = SAILOR__read_next__cell_ID(execution_read_address);
        
        // setup output
        (*context).cells[get_current_context__buffer_start] = (void*)SAILOR__get__cell_address_from_context(context, 0);
        (*context).cells[get_current_context__buffer_end] = ((void*)SAILOR__get__cell_address_from_context(context, 0)) + sizeof(SAILOR__context) - 1;

        break;
    // search for an allocation given a buffer
    case SAILOR__it__debug__search_for_allocation:
        // get parameters
        search_for_allocation__source_buffer_start = SAILOR__read_next__cell_ID(execution_read_address);
        search_for_allocation__source_buffer_end = SAILOR__read_next__cell_ID(execution_read_address);
        search_for_allocation__was_found = SAILOR__read_next__cell_ID(execution_read_address);
        search_for_allocation__found_buffer_start = SAILOR__read_next__cell_ID(execution_read_address);
        search_for_allocation__found_buffer_end = SAILOR__read_next__cell_ID(execution_read_address);

        // setup inputs
        search_for_allocation__source_buffer.start = (*context).cells[search_for_allocation__source_buffer_start];
        search_for_allocation__source_buffer.end = (*context).cells[search_for_allocation__source_buffer_end];

        // search for allocation
        search_for_allocation__found_buffer = SAILOR__search__valid_allocation_from_sub_buffer(allocations, search_for_allocation__source_buffer, &search_for_allocation__was_found_boolean);

        // pass outputs
        (*context).cells[search_for_allocation__was_found] = (SAILOR__cell)(SAILOR__u64)search_for_allocation__was_found_boolean;
        (*context).cells[search_for_allocation__found_buffer_start] = search_for_allocation__found_buffer.start;
        (*context).cells[search_for_allocation__found_buffer_end] = search_for_allocation__found_buffer.end;

        break;
    // in case instruction ID was invalid
    default:
        // set error
        SAILOR__set__error_code_cell(context, SAILOR__et__invalid_instruction_ID);

        // return exit context
        return SAILOR__nit__return_context;
    }

    // return next instruction by default
    return SAILOR__nit__next_instruction;
}

// run context
void SAILOR__run__context(SAILOR__allocations* allocations, SAILOR__context* context, SAILOR__instruction_count instruction_count) {
    SAILOR__nit next_instruction_action;

    // if an infinite amount of instructions can execute
    if (instruction_count == SAILOR__define__run_forever) {
        // run instructions
        while (1) {
            // run instruction
            next_instruction_action = SAILOR__run__instruction(allocations, context);

            // if quit
            if (next_instruction_action == SAILOR__nit__return_context) {
                return;
            }

            // assuming SAILOR__nit__next_instruction
            continue;
        }
    // if a finite amount of instructions can execute
    } else {
        for (SAILOR__instruction_count i = 0; i < instruction_count; i++) {
            // run instruction
            next_instruction_action = SAILOR__run__instruction(allocations, context);

            // check for early quitting
            if (next_instruction_action == SAILOR__nit__return_context) {
                return;
            }
        }
    }

    // execution ended without error
    return;
}

#endif
