#ifndef SCOURGE__sailor_coder
#define SCOURGE__sailor_coder

/* Include */
// C
#include "sailor_specifications.h"

/* Instruction Workspace */
// pass type
typedef enum SAILOR__pt {
    SAILOR__pt__get_offsets,
    SAILOR__pt__write_program,
    SAILOR__pt__COUNT,
} SAILOR__pt;

// offsets
typedef SAILOR__u64 SAILOR__offset;

// invalid offset placeholder
#define SAILOR__invalid_offset -1

// instruction creation container
typedef struct SAILOR__workspace {
    SAILOR__pt pass;
    SAILOR__u64 current_program_offset;
    SAILOR__address write_to;
    SAILOR__buffer* program_buffer;
} SAILOR__workspace;

// setup workspace
SAILOR__workspace SAILOR__setup__workspace(SAILOR__buffer* program_buffer_destination) {
    SAILOR__workspace output;

    // setup output
    output.pass = SAILOR__pt__get_offsets;
    output.current_program_offset = 0;
    output.write_to = (*program_buffer_destination).start;
    output.program_buffer = program_buffer_destination;

    return output;
}

// create an offset
SAILOR__offset SAILOR__get__offset(SAILOR__workspace* workspace) {
    // return current offset
    return (*workspace).current_program_offset;
}

// setup pass
void SAILOR__setup__pass(SAILOR__workspace* workspace, SAILOR__pt pass) {
    // setup pass in workspace
    (*workspace).pass = pass;

    // do stuff
    switch ((*workspace).pass) {
    case SAILOR__pt__get_offsets:
        (*workspace).current_program_offset = 0;
        
        break;
    case SAILOR__pt__write_program:
        // allocate program buffer
        (*(*workspace).program_buffer) = SAILOR__open__buffer((*workspace).current_program_offset);

        // setup pass
        (*workspace).current_program_offset = 0;
        (*workspace).write_to = (*(*workspace).program_buffer).start;

        break;
    default:
        break;
    }

    return;
}

/* Appending */
// add an offset to a list
void SAILOR__append__offset(SAILOR__list* list, SAILOR__offset offset, SAILOR__bt* memory_error_occured) {
    // request space
    SAILOR__list__request__space(list, sizeof(SAILOR__offset), memory_error_occured);

    // append data
    (*(SAILOR__offset*)SAILOR__calculate__list_current_address(list)) = offset;

    // increase fill
    (*list).filled_index += sizeof(SAILOR__offset);

    return;
}

/* Write Instruction Scraplets */
// write instruction ID
void SAILOR__write_next__instruction_ID(SAILOR__workspace* workspace, SAILOR__instruction_ID instruction_ID) {
    // write value
    if ((*workspace).pass == SAILOR__pt__write_program) {
        *((SAILOR__instruction_ID*)(*workspace).write_to) = instruction_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(SAILOR__instruction_ID);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + sizeof(SAILOR__instruction_ID));

    return;
}

// write flag ID
void SAILOR__write_next__flag_ID(SAILOR__workspace* workspace, SAILOR__flag_ID flag_ID) {
    // write value
    if ((*workspace).pass == SAILOR__pt__write_program) {
        *((SAILOR__flag_ID*)(*workspace).write_to) = flag_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(SAILOR__flag_ID);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + sizeof(SAILOR__flag_ID));

    return;
}

// write operation ID
void SAILOR__write_next__operation_ID(SAILOR__workspace* workspace, SAILOR__operation_ID operation_ID) {
    // write value
    if ((*workspace).pass == SAILOR__pt__write_program) {
        *((SAILOR__operation_ID*)(*workspace).write_to) = operation_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(SAILOR__operation_ID);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + sizeof(SAILOR__operation_ID));

    return;
}

// write cell ID
void SAILOR__write_next__cell_ID(SAILOR__workspace* workspace, SAILOR__cell_ID cell_ID) {
    // write value
    if ((*workspace).pass == SAILOR__pt__write_program) {
        *((SAILOR__cell_ID*)(*workspace).write_to) = cell_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(SAILOR__cell_ID);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + sizeof(SAILOR__cell_ID));

    return;
}

// write cell value
void SAILOR__write_next__cell(SAILOR__workspace* workspace, SAILOR__cell cell_value) {
    // write value
    if ((*workspace).pass == SAILOR__pt__write_program) {
        *((SAILOR__cell*)(*workspace).write_to) = cell_value;
    }

    // advance
    (*workspace).current_program_offset += sizeof(SAILOR__cell);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + sizeof(SAILOR__cell));

    return;
}

// write buffer
void SAILOR__write_next__buffer_contents(SAILOR__workspace* workspace, SAILOR__buffer buffer) {
    SAILOR__length buffer_length;

    // set buffer length
    buffer_length = SAILOR__calculate__buffer_length(buffer);

    // write buffer
    if ((*workspace).pass == SAILOR__pt__write_program) {
        // write buffer length
        SAILOR__write__buffer(buffer_length, sizeof(SAILOR__length), (*workspace).write_to);

        // copy buffer
        for (SAILOR__length byte_index = 0; byte_index < buffer_length; byte_index++) {
            // copy character
            ((SAILOR__u8*)(*workspace).write_to)[byte_index + sizeof(SAILOR__length)] = ((SAILOR__u8*)buffer.start)[byte_index];
        }
    }

    // advance
    (*workspace).current_program_offset += buffer_length + sizeof(SAILOR__length);
    (*workspace).write_to = (SAILOR__address)((u64)(*workspace).write_to + buffer_length + sizeof(SAILOR__length));

    return;
}

/* Write Instructions */
// write buffer data
void SAILOR__code__buffer_contents(SAILOR__workspace* workspace, SAILOR__buffer buffer) {
    // write data
    SAILOR__write_next__buffer_contents(workspace, buffer);

    return;
}

// write stop instruction
void SAILOR__code__stop(SAILOR__workspace* workspace) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__stop);

    return;
}

// write write cell instruction
void SAILOR__code__write_cell(SAILOR__workspace* workspace, SAILOR__cell value, SAILOR__cell_ID value_destination) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__write_cell);
    SAILOR__write_next__cell(workspace, value);
    SAILOR__write_next__cell_ID(workspace, value_destination);

    return;
}

// write operate instruction
void SAILOR__code__operate(SAILOR__workspace* workspace, SAILOR__flag_ID flag_ID, SAILOR__operation_ID operation_ID, SAILOR__cell_ID input_0, SAILOR__cell_ID input_1, SAILOR__cell_ID input_2, SAILOR__cell_ID output_0) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__operate);
    SAILOR__write_next__flag_ID(workspace, flag_ID);
    SAILOR__write_next__operation_ID(workspace, operation_ID);
    SAILOR__write_next__cell_ID(workspace, input_0);
    SAILOR__write_next__cell_ID(workspace, input_1);
    SAILOR__write_next__cell_ID(workspace, input_2);
    SAILOR__write_next__cell_ID(workspace, output_0);

    return;
}

// write request memory instruction
void SAILOR__code__request_memory(SAILOR__workspace* workspace, SAILOR__cell_ID allocation_size, SAILOR__cell_ID allocation_start, SAILOR__cell_ID allocation_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__request_memory);
    SAILOR__write_next__cell_ID(workspace, allocation_size);
    SAILOR__write_next__cell_ID(workspace, allocation_start);
    SAILOR__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write return memory instruction
void SAILOR__code__return_memory(SAILOR__workspace* workspace, SAILOR__cell_ID allocation_start, SAILOR__cell_ID allocation_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__return_memory);
    SAILOR__write_next__cell_ID(workspace, allocation_start);
    SAILOR__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write address to cell instruction
void SAILOR__code__address_to_cell(SAILOR__workspace* workspace, SAILOR__flag_ID flag_ID, SAILOR__cell_ID source_address, SAILOR__cell_ID byte_count, SAILOR__cell_ID destination_cell) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__address_to_cell);
    SAILOR__write_next__flag_ID(workspace, flag_ID);
    SAILOR__write_next__cell_ID(workspace, source_address);
    SAILOR__write_next__cell_ID(workspace, byte_count);
    SAILOR__write_next__cell_ID(workspace, destination_cell);

    return;
}

// write cell to address instruction
void SAILOR__code__cell_to_address(SAILOR__workspace* workspace, SAILOR__flag_ID flag_ID, SAILOR__cell_ID source_cell, SAILOR__cell_ID byte_count, SAILOR__cell_ID destination_address) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__cell_to_address);
    SAILOR__write_next__flag_ID(workspace, flag_ID);
    SAILOR__write_next__cell_ID(workspace, source_cell);
    SAILOR__write_next__cell_ID(workspace, byte_count);
    SAILOR__write_next__cell_ID(workspace, destination_address);

    return;
}

// write file to buffer instruction
void SAILOR__code__file_to_buffer(SAILOR__workspace* workspace, SAILOR__cell_ID file_name_start, SAILOR__cell_ID file_name_end, SAILOR__cell_ID file_data_start, SAILOR__cell_ID file_data_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__file_to_buffer);
    SAILOR__write_next__cell_ID(workspace, file_name_start);
    SAILOR__write_next__cell_ID(workspace, file_name_end);
    SAILOR__write_next__cell_ID(workspace, file_data_start);
    SAILOR__write_next__cell_ID(workspace, file_data_end);

    return;
}

// write buffer to file instruction
void SAILOR__code__buffer_to_file(SAILOR__workspace* workspace, SAILOR__cell_ID file_data_start, SAILOR__cell_ID file_data_end, SAILOR__cell_ID file_name_start, SAILOR__cell_ID file_name_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__buffer_to_file);
    SAILOR__write_next__cell_ID(workspace, file_data_start);
    SAILOR__write_next__cell_ID(workspace, file_data_end);
    SAILOR__write_next__cell_ID(workspace, file_name_start);
    SAILOR__write_next__cell_ID(workspace, file_name_end);

    return;
}

// write delete file instruction
void SAILOR__code__delete_file(SAILOR__workspace* workspace, SAILOR__cell_ID file_path_start, SAILOR__cell_ID file_path_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__delete_file);
    SAILOR__write_next__cell_ID(workspace, file_path_start);
    SAILOR__write_next__cell_ID(workspace, file_path_end);

    return;
}

// write buffer to buffer instruction
void SAILOR__code__buffer_to_buffer__low_to_high(SAILOR__workspace* workspace, SAILOR__cell_ID source_start, SAILOR__cell_ID source_end, SAILOR__cell_ID destination_start, SAILOR__cell_ID destination_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__buffer_to_buffer__low_to_high);
    SAILOR__write_next__cell_ID(workspace, source_start);
    SAILOR__write_next__cell_ID(workspace, source_end);
    SAILOR__write_next__cell_ID(workspace, destination_start);
    SAILOR__write_next__cell_ID(workspace, destination_end);

    return;
}

// write buffer to buffer instruction
void SAILOR__code__buffer_to_buffer__high_to_low(SAILOR__workspace* workspace, SAILOR__cell_ID source_start, SAILOR__cell_ID source_end, SAILOR__cell_ID destination_start, SAILOR__cell_ID destination_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__buffer_to_buffer__high_to_low);
    SAILOR__write_next__cell_ID(workspace, source_start);
    SAILOR__write_next__cell_ID(workspace, source_end);
    SAILOR__write_next__cell_ID(workspace, destination_start);
    SAILOR__write_next__cell_ID(workspace, destination_end);

    return;
}

// write compile instruction
void SAILOR__code__compile(SAILOR__workspace* workspace, SAILOR__cell_ID user_codes_buffers_buffer_start, SAILOR__cell_ID user_codes_buffers_buffer_end, SAILOR__cell_ID include_standard, SAILOR__cell_ID debug_enabled, SAILOR__cell_ID generate_kickstarter, SAILOR__cell_ID output_program_start, SAILOR__cell_ID output_program_end, SAILOR__cell_ID output_debug_start, SAILOR__cell_ID output_debug_end, SAILOR__cell_ID error__error_occured, SAILOR__cell_ID error__message_start, SAILOR__cell_ID error__message_end, SAILOR__cell_ID error__character_location__file_index, SAILOR__cell_ID error__character_location__line_number, SAILOR__cell_ID error__character_location__character_index) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__compile);
    SAILOR__write_next__cell_ID(workspace, user_codes_buffers_buffer_start);
    SAILOR__write_next__cell_ID(workspace, user_codes_buffers_buffer_end);
    SAILOR__write_next__cell_ID(workspace, include_standard);
    SAILOR__write_next__cell_ID(workspace, debug_enabled);
    SAILOR__write_next__cell_ID(workspace, generate_kickstarter);
    SAILOR__write_next__cell_ID(workspace, output_program_start);
    SAILOR__write_next__cell_ID(workspace, output_program_end);
    SAILOR__write_next__cell_ID(workspace, output_debug_start);
    SAILOR__write_next__cell_ID(workspace, output_debug_end);
    SAILOR__write_next__cell_ID(workspace, error__error_occured);
    SAILOR__write_next__cell_ID(workspace, error__message_start);
    SAILOR__write_next__cell_ID(workspace, error__message_end);
    SAILOR__write_next__cell_ID(workspace, error__character_location__file_index);
    SAILOR__write_next__cell_ID(workspace, error__character_location__line_number);
    SAILOR__write_next__cell_ID(workspace, error__character_location__character_index);

    return;
}

// write run instruction
void SAILOR__code__run(SAILOR__workspace* workspace, SAILOR__cell_ID context_buffer_start, SAILOR__cell_ID context_buffer_end, SAILOR__cell_ID instruction_count) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__run);
    SAILOR__write_next__cell_ID(workspace, context_buffer_start);
    SAILOR__write_next__cell_ID(workspace, context_buffer_end);
    SAILOR__write_next__cell_ID(workspace, instruction_count);

    return;
}

// get time instruction
void SAILOR__code__get_time(SAILOR__workspace* workspace, SAILOR__cell_ID time_in_seconds, SAILOR__cell_ID time_in_nanoseconds) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__get_time);
    SAILOR__write_next__cell_ID(workspace, time_in_seconds);
    SAILOR__write_next__cell_ID(workspace, time_in_nanoseconds);

    return;
}

// write debug putchar instruction
void SAILOR__code__debug__putchar(SAILOR__workspace* workspace, SAILOR__cell_ID printing_cell_ID) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__putchar);
    SAILOR__write_next__cell_ID(workspace, printing_cell_ID);

    return;
}

// write debug fgets instruction
void SAILOR__code__debug__fgets(SAILOR__workspace* workspace, SAILOR__cell_ID buffer_start_ID, SAILOR__cell_ID buffer_end_ID) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__fgets);
    SAILOR__write_next__cell_ID(workspace, buffer_start_ID);
    SAILOR__write_next__cell_ID(workspace, buffer_end_ID);

    return;
}

// write debug mark data section instruction
void SAILOR__code__debug__mark_data_section(SAILOR__workspace* workspace, SAILOR__cell buffer_length) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__mark_data_section);
    SAILOR__write_next__cell(workspace, buffer_length);

    return;
}

// write debug mark code section instruction
void SAILOR__code__debug__mark_code_section(SAILOR__workspace* workspace, SAILOR__cell code_buffer_length) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__mark_code_section);
    SAILOR__write_next__cell(workspace, code_buffer_length);

    return;
}

// write debug get current context instruction
void SAILOR__code__debug__get_current_context(SAILOR__workspace* workspace, SAILOR__cell_ID buffer_start, SAILOR__cell_ID buffer_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__get_current_context);
    SAILOR__write_next__cell_ID(workspace, buffer_start);
    SAILOR__write_next__cell_ID(workspace, buffer_end);

    return;
}

// write debug search for allocation instruction
void SAILOR__code__debug__search_for_allocation(SAILOR__workspace* workspace, SAILOR__cell_ID source_buffer_start, SAILOR__cell_ID source_buffer_end, SAILOR__cell_ID was_found, SAILOR__cell_ID found_buffer_start, SAILOR__cell_ID found_buffer_end) {
    // write instruction
    SAILOR__write_next__instruction_ID(workspace, SAILOR__it__debug__search_for_allocation);
    SAILOR__write_next__cell_ID(workspace, source_buffer_start);
    SAILOR__write_next__cell_ID(workspace, source_buffer_end);
    SAILOR__write_next__cell_ID(workspace, was_found);
    SAILOR__write_next__cell_ID(workspace, found_buffer_start);
    SAILOR__write_next__cell_ID(workspace, found_buffer_end);

    return;
}

/* Stack ABI Defines */
// types
typedef u64 SAILOR__preserve;
typedef SAILOR__preserve SAILOR__preserve__start;
typedef SAILOR__preserve SAILOR__preserve__end;

// stack cell types
typedef enum SAILOR__srt {
    // start of cells
    SAILOR__srt__START = SAILOR__rt__END,

    // constant cells
    SAILOR__srt__constant__0 = SAILOR__srt__START,
    SAILOR__srt__constant__1,
    SAILOR__srt__constant__2,
    SAILOR__srt__constant__3,
    SAILOR__srt__constant__4,
    SAILOR__srt__constant__5,
    SAILOR__srt__constant__6,
    SAILOR__srt__constant__7,
    SAILOR__srt__constant__8,
    SAILOR__srt__constant__16,
    SAILOR__srt__constant__32,
    SAILOR__srt__constant__64,
    SAILOR__srt__constant__cell_byte_size,
    SAILOR__srt__constant__context_byte_size,
    SAILOR__srt__constant__return_address_offset_creation_size,

    // context io cells
    SAILOR__srt__input_buffer_start,
    SAILOR__srt__input_buffer_end,
    SAILOR__srt__output_result_buffer_start,
    SAILOR__srt__output_result_buffer_end,
    SAILOR__srt__output_error_buffer_start,
    SAILOR__srt__output_error_buffer_end,

    // stack cells
    SAILOR__srt__stack__start_address,
    SAILOR__srt__stack__end_address,
    SAILOR__srt__stack__current_address,

    // control flow cells
    SAILOR__srt__return_address,
    SAILOR__srt__always_run__flag_ID,
    SAILOR__srt__never_run__flag_ID,

    // sailor data cells
    SAILOR__srt__sailor_scraplet__instruction_ID__byte_size,
    SAILOR__srt__sailor_scraplet__flag_ID__byte_size,
    SAILOR__srt__sailor_scraplet__operation_ID__byte_size,
    SAILOR__srt__sailor_scraplet__cell_ID__byte_size,
    SAILOR__srt__sailor_scraplet__cell__byte_size,
    SAILOR__srt__sailor_scraplet__operation_ID_value__START,
    SAILOR__srt__sailor_scraplet__operation_ID_value__END = SAILOR__srt__sailor_scraplet__operation_ID_value__START + SAILOR__ot__COUNT,

    // temporary cells
    SAILOR__srt__temp__write = SAILOR__srt__sailor_scraplet__operation_ID_value__END,
    SAILOR__srt__temp__offset,
    SAILOR__srt__temp__address,
    SAILOR__srt__temp__flag,
    SAILOR__srt__temp__flag_ID_0,
    SAILOR__srt__temp__flag_ID_1,
    SAILOR__srt__temp__flag_ID_2,
    SAILOR__srt__temp__byte_count,
    SAILOR__srt__temp__length,
    SAILOR__srt__temp__buffer_0__start,
    SAILOR__srt__temp__buffer_0__end,
    SAILOR__srt__temp__buffer_1__start,
    SAILOR__srt__temp__buffer_1__end,
    SAILOR__srt__temp__cell_ID,

    // end of cells
    SAILOR__srt__END,

    // aliases
    SAILOR__srt__constant__true = SAILOR__srt__constant__1,
    SAILOR__srt__constant__false = SAILOR__srt__constant__0,
    SAILOR__srt__constant__always_run__value = SAILOR__srt__constant__true,
    SAILOR__srt__constant__never_run__value = SAILOR__srt__constant__false,
    SAILOR__srt__constant__bits_in_byte = SAILOR__srt__constant__8,
    SAILOR__srt__constant__ascii_character_byte_size = SAILOR__srt__constant__1,

    // locations
    SAILOR__srt__start__workspace = 8192,
    SAILOR__srt__start__function_io = SAILOR__rt__TOTAL_COUNT - 8192,

    // count
    SAILOR__srt__COUNT = SAILOR__srt__END - SAILOR__srt__START,
    SAILOR__srt__workspace__COUNT = SAILOR__srt__start__function_io - SAILOR__srt__start__workspace,
} SAILOR__srt;

// stack instruction types
typedef enum SAILOR__sit {
    // start
    SAILOR__sit__START = SAILOR__it__END,

    // instructions
    SAILOR__sit__cell_to_cell = SAILOR__sit__START,
    SAILOR__sit__push_cell,
    SAILOR__sit__pop_cell,
    SAILOR__sit__calculate_dynamically__offset_address,
    SAILOR__sit__calculate_statically__offset_address,
    SAILOR__sit__jump__explicit,
    SAILOR__sit__jump__static,
    SAILOR__sit__create_return_address__directly_after_jump,
    SAILOR__sit__call__explicit,
    SAILOR__sit__call__static,
    SAILOR__sit__start,
    SAILOR__sit__preserve_workspace,
    SAILOR__sit__restore_workspace,
    SAILOR__sit__operate__flag,
    SAILOR__sit__operate__jump__explicit,
    SAILOR__sit__operate__jump__dynamic,
    SAILOR__sit__operate__jump__static,
    SAILOR__sit__setup__context,

    // end
    SAILOR__sit__END,

    // counts
    SAILOR__sit__COUNT = SAILOR__sit__END - SAILOR__sit__START,
    SAILOR__sit__TOTAL_COUNT = SAILOR__sit__END - SAILOR__it__START,
} SAILOR__sit;

// stack instruction length types
typedef enum SAILOR__silt {
    SAILOR__silt__cell_to_cell = SAILOR__ilt__operate,
    SAILOR__silt__push_cell = SAILOR__ilt__cell_to_address + SAILOR__ilt__operate,
    SAILOR__silt__pop_cell = SAILOR__ilt__operate + SAILOR__ilt__address_to_cell,
    SAILOR__silt__calculate_dynamically__offset_address = SAILOR__ilt__operate,
    SAILOR__silt__calculate_statically__offset_address = SAILOR__ilt__write_cell + SAILOR__silt__calculate_dynamically__offset_address,
    SAILOR__silt__jump__explicit = SAILOR__silt__cell_to_cell,
    SAILOR__silt__jump__static = SAILOR__silt__calculate_statically__offset_address + SAILOR__silt__jump__explicit,
} SAILOR__silt;

// stack flag types
typedef enum SAILOR__sft {
    // start of flags
    SAILOR__sft__START = 0,

    // flags
    SAILOR__sft__always_run = SAILOR__sft__START,
    SAILOR__sft__never_run,
    SAILOR__sft__temp_0,
    ANIVL__sft__temp_1,
    ANIVL__sft__temp_2,

    // end of stack flags
    SAILOR__sft__END,
} SAILOR__sft;

// stack size
typedef u64 SAILOR__stack_size;

/* Context IO */
// pass input
void SAILOR__set__input(SAILOR__context* context, SAILOR__buffer input) {
    // write data to cells
    (*context).cells[SAILOR__srt__input_buffer_start] = (SAILOR__cell)input.start;
    (*context).cells[SAILOR__srt__input_buffer_end] = (SAILOR__cell)input.end;

    return;
}

/* Stack ABI Code */
// move one cell to the next
void SAILOR__code__cell_to_cell(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID source, SAILOR__cell_ID destination) {
    // write code
    SAILOR__code__operate(workspace, flag, SAILOR__ot__cell_to_cell, source, SAILOR__unused_cell_ID, SAILOR__unused_cell_ID, destination);

    return;
}

// push a cell onto the stack
void SAILOR__code__push_cell(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID source_cell) {
    // write data to stack
    SAILOR__code__cell_to_address(workspace, flag, source_cell, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__stack__current_address);

    // increase stack pointer
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__srt__stack__current_address, SAILOR__srt__constant__cell_byte_size, SAILOR__unused_cell_ID, SAILOR__srt__stack__current_address);

    return;
}

// pop a cell from the stack
void SAILOR__code__pop_cell(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID destination_cell) {
    // decrease stack pointer
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_subtract, SAILOR__srt__stack__current_address, SAILOR__srt__constant__cell_byte_size, SAILOR__unused_cell_ID, SAILOR__srt__stack__current_address);

    // read data from stack
    SAILOR__code__address_to_cell(workspace, flag, SAILOR__srt__stack__current_address, SAILOR__srt__constant__cell_byte_size, destination_cell);

    return;
}

// calculate a buffer's length
void SAILOR__code__calculate__buffer_length(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID buffer_start, SAILOR__cell_ID buffer_end, SAILOR__cell_ID buffer_length) {
    // subtract end and start
	SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_subtract, buffer_end, buffer_start, SAILOR__unused_cell_ID, buffer_length);

    // convert difference to length
	SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, buffer_length, SAILOR__srt__constant__1, SAILOR__unused_cell_ID, buffer_length);

    return;
}

// calculate a buffers end address from the start and length
void SAILOR__code__calculate__buffer_end_address(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID buffer_start, SAILOR__cell_ID buffer_length, SAILOR__cell_ID buffer_end) {
    // add length to value
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, buffer_start, buffer_length, SAILOR__unused_cell_ID, buffer_end);

    // adjust length to buffer end pointer
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_subtract, buffer_end, SAILOR__srt__constant__1, SAILOR__unused_cell_ID, buffer_end);

    return;
}

// calculate an address from the program start and an offset cell
void SAILOR__code__calculate_dynamically__offset_address(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID offset_cell, SAILOR__cell_ID destination) {
    // calculate address
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__rt__program_start_address, offset_cell, SAILOR__unused_cell_ID, destination);

    return;
}

// calculate an address from the program start and an offset
void SAILOR__code__calculate_statically__offset_address(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__offset offset, SAILOR__cell_ID destination) {
    // write temp
    SAILOR__code__write_cell(workspace, (SAILOR__cell)offset, SAILOR__srt__temp__offset);

    // calculate address
    SAILOR__code__calculate_dynamically__offset_address(workspace, flag, SAILOR__srt__temp__offset, destination);

    return;
}

// jump to a specific address
void SAILOR__code__jump__explicit(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID address) {
    // jump
    SAILOR__code__cell_to_cell(workspace, flag, address, SAILOR__rt__program_current_address);

    return;
}

// jump to an offset calculated address
void SAILOR__code__jump__static(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__offset offset) {
    // calculate offset
    SAILOR__code__calculate_statically__offset_address(workspace, flag, offset, SAILOR__srt__temp__address);

    // jump
    SAILOR__code__jump__explicit(workspace, flag, SAILOR__srt__temp__address);

    return;
}

// create return address
void SAILOR__code__create_return_address__directly_after_jump(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID destination) {
    // create offset
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__rt__program_current_address, SAILOR__srt__constant__return_address_offset_creation_size, SAILOR__unused_cell_ID, destination);

    return;
}

// call function explicitly
void SAILOR__code__inner_call__explicit(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID address) {
    // preserve return address
    SAILOR__code__push_cell(workspace, flag, SAILOR__srt__return_address);

    // setup new return address
    SAILOR__code__create_return_address__directly_after_jump(workspace, flag, SAILOR__srt__return_address);

    // jump
    SAILOR__code__jump__explicit(workspace, flag, address);

    // restore return address
    SAILOR__code__pop_cell(workspace, flag, SAILOR__srt__return_address);

    return;
}

// call function statically
void SAILOR__code__inner_call__static(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__offset offset) {
    // calculate address
    SAILOR__code__calculate_statically__offset_address(workspace, flag, offset, SAILOR__srt__temp__address);

    // call function with offset set
    SAILOR__code__inner_call__explicit(workspace, flag, SAILOR__srt__temp__address);

    return;
}

// kickstart program (assumes program variables are set!)
void SAILOR__code__start(SAILOR__workspace* workspace, SAILOR__stack_size stack_size, SAILOR__offset jump_to) {
    // setup code marker
    SAILOR__code__debug__mark_code_section(workspace, 0);

    // setup error cell
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__et__no_error, SAILOR__rt__error_code);

    // setup address catcher (default is enabled)
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__bt__true, SAILOR__rt__address_catch_toggle);

    // setup flag cells
    SAILOR__code__write_cell(workspace, (SAILOR__cell)1, SAILOR__rt__flags_0);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)0, SAILOR__rt__flags_1);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)0, SAILOR__rt__flags_2);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)0, SAILOR__rt__flags_3);

    // setup constants
    SAILOR__code__write_cell(workspace, (SAILOR__cell)0, SAILOR__srt__constant__0);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)1, SAILOR__srt__constant__1);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)2, SAILOR__srt__constant__2);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)3, SAILOR__srt__constant__3);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)4, SAILOR__srt__constant__4);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)5, SAILOR__srt__constant__5);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)6, SAILOR__srt__constant__6);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)7, SAILOR__srt__constant__7);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)8, SAILOR__srt__constant__8);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)16, SAILOR__srt__constant__16);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)32, SAILOR__srt__constant__32);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)64, SAILOR__srt__constant__64);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__cell), SAILOR__srt__constant__cell_byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__context), SAILOR__srt__constant__context_byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__silt__jump__explicit, SAILOR__srt__constant__return_address_offset_creation_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__sft__always_run, SAILOR__srt__always_run__flag_ID);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__sft__never_run, SAILOR__srt__never_run__flag_ID);

    // setup sailor scraplet constants
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__instruction_ID), SAILOR__srt__sailor_scraplet__instruction_ID__byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__flag_ID), SAILOR__srt__sailor_scraplet__flag_ID__byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__operation_ID), SAILOR__srt__sailor_scraplet__operation_ID__byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__cell_ID), SAILOR__srt__sailor_scraplet__cell_ID__byte_size);
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__cell), SAILOR__srt__sailor_scraplet__cell__byte_size);

    // generate operation ID scraplet constants
    for (SAILOR__cell_index operation_ID_list_start = SAILOR__srt__sailor_scraplet__operation_ID_value__START; operation_ID_list_start <= SAILOR__srt__sailor_scraplet__operation_ID_value__END; operation_ID_list_start++) {
        // write cell
        SAILOR__code__write_cell(workspace, (SAILOR__cell)(SAILOR__ot__START + operation_ID_list_start), operation_ID_list_start);
    }

    // setup outputs
    SAILOR__code__cell_to_cell(workspace, SAILOR__sft__always_run, SAILOR__srt__constant__0, SAILOR__srt__output_result_buffer_start);
    SAILOR__code__cell_to_cell(workspace, SAILOR__sft__always_run, SAILOR__srt__constant__0, SAILOR__srt__output_result_buffer_end);
    SAILOR__code__cell_to_cell(workspace, SAILOR__sft__always_run, SAILOR__srt__constant__0, SAILOR__srt__output_error_buffer_start);
    SAILOR__code__cell_to_cell(workspace, SAILOR__sft__always_run, SAILOR__srt__constant__0, SAILOR__srt__output_error_buffer_end);

    // setup stack
    SAILOR__code__write_cell(workspace, (SAILOR__cell)stack_size, SAILOR__srt__temp__write);
    SAILOR__code__request_memory(workspace, SAILOR__srt__temp__write, SAILOR__srt__stack__start_address, SAILOR__srt__stack__end_address);
    SAILOR__code__cell_to_cell(workspace, (SAILOR__flag_ID)SAILOR__sft__always_run, SAILOR__srt__stack__start_address, SAILOR__srt__stack__current_address);

    // jump to main
    SAILOR__code__inner_call__static(workspace, SAILOR__sft__always_run, jump_to);

    // deallocate stack
    SAILOR__code__return_memory(workspace, SAILOR__srt__stack__start_address, SAILOR__srt__stack__end_address);

    // quit program
    SAILOR__code__stop(workspace);

    return;
}

// preserve workspace
void SAILOR__code__preserve_workspace(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__preserve__start preserve_start, SAILOR__preserve__end preserve_end) {
    // preserve flags
    SAILOR__code__push_cell(workspace, flag, SAILOR__rt__flags_0);
    SAILOR__code__push_cell(workspace, flag, SAILOR__rt__flags_1);
    SAILOR__code__push_cell(workspace, flag, SAILOR__rt__flags_2);
    SAILOR__code__push_cell(workspace, flag, SAILOR__rt__flags_3);

    // preserve error code
    SAILOR__code__push_cell(workspace, flag, SAILOR__rt__error_code);

    // preserve workspace cells
    for (SAILOR__preserve i = preserve_start; i <= preserve_end; i++) {
        // preserve cell
        SAILOR__code__push_cell(workspace, flag, i);
    }

    return;
}

// restore workspace
void SAILOR__code__restore_workspace(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__preserve__start preserve_start, SAILOR__preserve__end preserve_end) {
    // restore workspace cells
    for (SAILOR__preserve i = preserve_end; i >= preserve_start; i--) {
        // restore cell
        SAILOR__code__pop_cell(workspace, flag, i);
    }

    // restore error code
    SAILOR__code__pop_cell(workspace, flag, SAILOR__rt__error_code);

    // restore flags
    SAILOR__code__pop_cell(workspace, flag, SAILOR__rt__flags_3);
    SAILOR__code__pop_cell(workspace, flag, SAILOR__rt__flags_2);
    SAILOR__code__pop_cell(workspace, flag, SAILOR__rt__flags_1);
    SAILOR__code__pop_cell(workspace, flag, SAILOR__rt__flags_0);

    return;
}

// operate boolean
void SAILOR__code__operate__boolean(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID lower_bound, SAILOR__cell_ID value, SAILOR__cell_ID upper_bound, SAILOR__flag_ID invert_result, SAILOR__cell_ID resulting_boolean) {
    // get comparison result set to variable
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_within_range, lower_bound, value, upper_bound, resulting_boolean);

    // invert boolean
    SAILOR__code__operate(workspace, invert_result, SAILOR__ot__bits_xor, resulting_boolean, SAILOR__srt__constant__1, SAILOR__unused_cell_ID, resulting_boolean);

    return;
}

// operate flag
void SAILOR__code__operate__flag(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID lower_bound, SAILOR__cell_ID value, SAILOR__cell_ID upper_bound, SAILOR__flag_ID invert_result, SAILOR__cell_ID output_flag_ID) {
    // get comparison result set to variable
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_within_range, lower_bound, value, upper_bound, SAILOR__srt__temp__flag);

    // write flag
    SAILOR__code__operate(workspace, flag, SAILOR__ot__flag_set, SAILOR__srt__temp__flag, SAILOR__unused_cell_ID, SAILOR__unused_cell_ID, output_flag_ID);

    // invert flag
    SAILOR__code__operate(workspace, invert_result, SAILOR__ot__flag_invert, output_flag_ID, SAILOR__unused_cell_ID, SAILOR__unused_cell_ID, output_flag_ID);

    return;
}

// operate jump explicitly
void SAILOR__code__operate__jump__explicit(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID lower_bound, SAILOR__cell_ID value, SAILOR__cell_ID upper_bound, SAILOR__flag_ID invert_result, SAILOR__cell_ID jump_address) {
    // setup flag temp
    SAILOR__code__write_cell(workspace, (SAILOR__cell)SAILOR__sft__temp_0, SAILOR__srt__temp__flag_ID_0);

    // perform comparison
    SAILOR__code__operate__flag(workspace, flag, lower_bound, value, upper_bound, invert_result, SAILOR__srt__temp__flag_ID_0);

    // attempt jump
    SAILOR__code__jump__explicit(workspace, SAILOR__sft__temp_0, jump_address);

    return;
}

// operate jump dynamically
void SAILOR__code__operate__jump__dynamic(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID lower_bound, SAILOR__cell_ID value, SAILOR__cell_ID upper_bound, SAILOR__flag_ID invert_result, SAILOR__cell_ID jump_offset_cell) {
    // calculate offset
    SAILOR__code__calculate_dynamically__offset_address(workspace, flag, jump_offset_cell, SAILOR__srt__temp__address);

    // attempt jump
    SAILOR__code__operate__jump__explicit(workspace, flag, lower_bound, value, upper_bound, invert_result, SAILOR__srt__temp__address);

    return;
}

// operate jump statically
void SAILOR__code__operate__jump__static(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID lower_bound, SAILOR__cell_ID value, SAILOR__cell_ID upper_bound, SAILOR__flag_ID invert_result, SAILOR__offset jump_offset) {
    // write offset to cell
    SAILOR__code__write_cell(workspace, (SAILOR__cell)jump_offset, SAILOR__srt__temp__offset);

    // attempt jump
    SAILOR__code__operate__jump__dynamic(workspace, flag, lower_bound, value, upper_bound, invert_result, SAILOR__srt__temp__offset);

    return;
}

// retrieve an embedded buffer
void SAILOR__code__retrieve_embedded_buffer(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID program_offset, SAILOR__cell_ID buffer_start, SAILOR__cell_ID buffer_end) {
    // calculate buffer start
    SAILOR__code__calculate_dynamically__offset_address(workspace, flag, program_offset, SAILOR__srt__temp__address);

    // get buffer length
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__length), SAILOR__srt__temp__byte_count);
    SAILOR__code__address_to_cell(workspace, flag, SAILOR__srt__temp__address, SAILOR__srt__temp__byte_count, SAILOR__srt__temp__length);

    // calculate buffer data start
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__srt__temp__address, SAILOR__srt__constant__cell_byte_size, SAILOR__unused_cell_ID, buffer_start);

    // calculate buffer data end
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, buffer_start, SAILOR__srt__temp__length, SAILOR__unused_cell_ID, SAILOR__srt__temp__address);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_subtract, SAILOR__srt__temp__address, SAILOR__srt__constant__1, SAILOR__unused_cell_ID, buffer_end);

    return;
}

// setup context
void SAILOR__code__setup__context(SAILOR__workspace* workspace, SAILOR__cell_ID program_buffer_start, SAILOR__cell_ID program_buffer_end, SAILOR__cell_ID context_buffer_start, SAILOR__cell_ID context_buffer_end) {
    // setup allocation size
    SAILOR__code__write_cell(workspace, (SAILOR__cell)sizeof(SAILOR__context), SAILOR__srt__temp__write);

    // allocate context
    SAILOR__code__request_memory(workspace, SAILOR__srt__temp__write, context_buffer_start, context_buffer_end);

    // setup skeleton context
    // setup buffer start
    SAILOR__code__cell_to_cell(workspace, SAILOR__sft__always_run, context_buffer_start, SAILOR__srt__temp__address);
    SAILOR__code__cell_to_address(workspace, SAILOR__sft__always_run, program_buffer_start, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__temp__address);

    // setup current address
    SAILOR__code__operate(workspace, SAILOR__sft__always_run, SAILOR__ot__integer_add, SAILOR__srt__temp__address, SAILOR__srt__constant__cell_byte_size, SAILOR__unused_cell_ID, SAILOR__srt__temp__address);
    SAILOR__code__cell_to_address(workspace, SAILOR__sft__always_run, program_buffer_end, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__temp__address);

    // setup end address
    SAILOR__code__operate(workspace, SAILOR__sft__always_run, SAILOR__ot__integer_add, SAILOR__srt__temp__address, SAILOR__srt__constant__cell_byte_size, SAILOR__unused_cell_ID, SAILOR__srt__temp__address);
    SAILOR__code__cell_to_address(workspace, SAILOR__sft__always_run, program_buffer_start, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__temp__address);
    
    return;
}

// calculate buffer for cell range
void SAILOR__code__calculate__addresses_for_cell_range_from_context(SAILOR__workspace* workspace, SAILOR__flag_ID flag, SAILOR__cell_ID range_start, SAILOR__cell_ID range_end, SAILOR__cell_ID buffer_start, SAILOR__cell_ID buffer_end) {
    // get cell address range from context
    SAILOR__code__debug__get_current_context(workspace, SAILOR__srt__temp__buffer_0__start, SAILOR__srt__temp__buffer_0__end);

    // setup starting address
    SAILOR__code__write_cell(workspace, (SAILOR__cell)(SAILOR__u64)range_start, SAILOR__srt__temp__cell_ID);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_multiply, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__temp__cell_ID, SAILOR__unused_cell_ID, SAILOR__srt__temp__length);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__srt__temp__buffer_0__start, SAILOR__srt__temp__length, SAILOR__unused_cell_ID, buffer_start);

    // setup end address
    SAILOR__code__write_cell(workspace, (SAILOR__cell)(SAILOR__u64)range_end + 1, SAILOR__srt__temp__cell_ID);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_multiply, SAILOR__srt__constant__cell_byte_size, SAILOR__srt__temp__cell_ID, SAILOR__unused_cell_ID, SAILOR__srt__temp__length);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_subtract, SAILOR__srt__temp__length, SAILOR__srt__constant__1, SAILOR__unused_cell_ID, SAILOR__srt__temp__length);
    SAILOR__code__operate(workspace, flag, SAILOR__ot__integer_add, SAILOR__srt__temp__buffer_0__start, SAILOR__srt__temp__length, SAILOR__unused_cell_ID, buffer_end);

    return;
}

// debug, print buffer inline
void SAILOR__code__debug__print_buffer_inline(SAILOR__workspace* workspace, SAILOR__buffer buffer) {
    // print character by character
    for (SAILOR__u64 character = 0; character < SAILOR__calculate__buffer_length(buffer); character++) {
        // write character to cell
        SAILOR__code__write_cell(workspace, (SAILOR__cell)(SAILOR__u64)(((SAILOR__character*)buffer.start)[character]), SAILOR__srt__temp__write);

        // print character
        SAILOR__code__debug__putchar(workspace, SAILOR__srt__temp__write);
    }

    return;
}

#endif
