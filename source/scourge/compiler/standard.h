#ifndef SCOURGE__standard__buffering
#define SCOURGE__standard__buffering

/* Include */
// anvil
#include "../../anvil.h"

// standard files
#include "../../../build_temps/scourge-print.c"
#include "../../../build_temps/scourge-cast.c"
#include "../../../build_temps/scourge-buffer.c"
#include "../../../build_temps/scourge-current.c"
#include "../../../build_temps/scourge-list.c"
#include "../../../build_temps/scourge-context.c"
#include "../../../build_temps/scourge-check.c"
#include "../../../build_temps/scourge-error.c"
#include "../../../build_temps/scourge-json.c"
#include "../../../build_temps/scourge-time.c"
#include "../../../build_temps/scourge-anvil.c"
#include "../../../build_temps/scourge-compile.c"
#include "../../../build_temps/scourge-just_run.c"

/* Turn C Files Into Buffers */
// bufferify any file
ANVIL__buffer STANDARD__bufferify__any_file(unsigned char* buffer, unsigned int length) {
    return ANVIL__create__buffer(buffer, ANVIL__calculate__address_from_buffer_index(buffer, length - 1));
}

#endif
