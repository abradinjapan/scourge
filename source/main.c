// sailor
#include "sailor.h"

// c
#include <stdio.h>

// print context
void MAIN__print__context(SAILOR__context* context) {
    u64 cell_index;
    u64 row_items;

    // setup variables
    cell_index = 0;

    // print program size
    printf("Program Size: [ %lu ]\n", ((SAILOR__u64)(*context).cells[SAILOR__rt__program_end_address] - (SAILOR__u64)(*context).cells[SAILOR__rt__program_start_address]));

    // print cells section header
    printf("Cells:\n");

    // print rows
    while (cell_index < SAILOR__rt__TOTAL_COUNT) {
        // set row items
        row_items = 0;

        // print padding
        printf("\t%lu: [", (SAILOR__u64)cell_index);

        // print columns
        while (cell_index < SAILOR__rt__TOTAL_COUNT && row_items < 8) {
            // print cell value
            printf(" %lu", (SAILOR__u64)(*context).cells[cell_index]);

            // next
            cell_index++;
            row_items++;
        }

        // print padding
        printf(" ]\n");
    }

    // print final padding
    printf("----------\n");

    return;
}

// entry point
int main(int argc, char** argv) {
    SAILOR__list files;
    SAILOR__bt debug_mode = SAILOR__bt__false;
    SAILOR__u64 current_argument = 1;
    COMPILER__error error;

    // init error
    error = COMPILER__create_null__error();

    // open files list
    files = COMPILER__open__list_with_error(sizeof(SAILOR__buffer) * 32, &error);
    if (COMPILER__check__error_occured(&error)) {
        printf("Error, could not open files list.\n");

        return 1;
    }

    // check if there are enough arguments
    if (argc > 1) {
        // check for debug mode
        if (SAILOR__calculate__buffer_contents_equal(SAILOR__open__buffer_from_string((u8*)"--debug", SAILOR__bt__false, SAILOR__bt__false), SAILOR__open__buffer_from_string((u8*)argv[current_argument], SAILOR__bt__false, SAILOR__bt__false))) {
            // enable debug mode
            debug_mode = SAILOR__bt__true;

            // skip to next input
            current_argument++;
        }

        // load files
        while (current_argument < (SAILOR__u64)argc) {
            // get file
            SAILOR__buffer file = SAILOR__move__file_to_buffer(SAILOR__open__buffer_from_string((u8*)argv[current_argument], SAILOR__bt__false, SAILOR__bt__true));

            // check for blank file
            if (SAILOR__check__empty_buffer(file)) {
                // file could no be opened
                printf("Error, file \"%s\" could not be opened.\n", (char*)SAILOR__open__buffer_from_string((u8*)argv[current_argument], SAILOR__bt__false, SAILOR__bt__true).start);

                goto clean_up;
            }

            // add file
            COMPILER__append__buffer_with_error(&files, file, &error);
            if (COMPILER__check__error_occured(&error) == SAILOR__bt__true) {
                printf("Error, could not add buffer to inputs list.");

                goto clean_up;
            }

            // next argument
            current_argument++;
        }

        // if files were passed
        if (SAILOR__check__current_within_range(SAILOR__calculate__current_from_list_filled_index(&files)) == SAILOR__bt__true) {
            // setup output
            SAILOR__buffer program = SAILOR__create_null__buffer();
            SAILOR__buffer debug_information = SAILOR__create_null__buffer();

            // run compiler
            COMPILER__compile__files(SAILOR__calculate__list_current_buffer(&files), SAILOR__bt__true, SAILOR__bt__true, debug_mode, SAILOR__bt__true, &program, &debug_information, &error);

            // if error
            if (COMPILER__check__error_occured(&error)) {
                // setup json error
                SAILOR__bt json_error_occured = SAILOR__bt__false;

                // get message
                SAILOR__buffer error_json = COMPILER__serialize__error_json(error, &json_error_occured);
                if (json_error_occured) {
                    printf("Failed to serialize json error, oops.\n");

                    goto clean_up;
                }

                // print error
                fflush(stdout);
                SAILOR__print__buffer(error_json);

                // deallocate error message
                SAILOR__close__buffer(error_json);
            // no error occured, run code
            } else {
                SAILOR__bt memory_error_occured = SAILOR__bt__false;

                // setup allocations
                SAILOR__allocations allocations = SAILOR__open__allocations(&memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but allocations failed to open.\n");

                    goto close_debug_information;
                }

                // add allocations
                SAILOR__remember__allocation(&allocations, program, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but program allocations failed to append.\n");

                    goto close_debug_information;
                }
                SAILOR__remember__allocation(&allocations, debug_information, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but debug allocations failed to append.\n");

                    goto close_allocations;
                }

                // setup context
                SAILOR__buffer context_buffer = SAILOR__open__buffer(sizeof(SAILOR__context));
                *(SAILOR__context*)context_buffer.start = SAILOR__setup__context(program);
                SAILOR__remember__allocation(&allocations, context_buffer, &memory_error_occured);
                if (memory_error_occured) {
                    printf("Internal Error: Program built successfully, but allocations failed to append.\n");
                
                    goto close_context;
                }

                // print debug
                if (debug_mode == SAILOR__bt__true) {
                    // print error json
                    SAILOR__print__buffer(debug_information);
                
                    // print header
                    printf("Running program...\n------------------\n");
                }

                // run code
                SAILOR__run__context(&allocations, (SAILOR__context*)context_buffer.start, SAILOR__define__run_forever);

                // print debug
                if (debug_mode == SAILOR__bt__true) {
                    printf("\n");
                }

                // close program
                SAILOR__close__buffer(program);

                // close context
                close_context:
                SAILOR__close__buffer(context_buffer);

                // close allocations
                close_allocations:
                SAILOR__close__allocations(&allocations);
            }

            // close debug information
            close_debug_information:
            SAILOR__close__buffer(debug_information);
        // if no files
        } else {
            printf("Error, no file paths were passed.\n");
        }

        // clean up
        clean_up:
        if (error.occured == SAILOR__bt__true) {
            COMPILER__close__error(error);
        }
        SAILOR__current current_file = SAILOR__calculate__current_from_list_filled_index(&files);
        while (SAILOR__check__current_within_range(current_file)) {
            SAILOR__close__buffer(*(SAILOR__buffer*)current_file.start);
            current_file.start += sizeof(SAILOR__buffer);
        }
        SAILOR__close__list(files);
    // not enough args
    } else {
        printf("Error, no arguments were passed.\n");

        return 1;
    }

    // exit
    return 0;
}
