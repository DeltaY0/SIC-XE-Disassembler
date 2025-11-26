#include "core/logger.h"
#include "util/opcode_parser.h"

#define OPCODEFILE "res/opcodes.txt"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

i32 main(i32 argc, char *argv[]) {

    LOGFMT(
        "MAIN",
        "version\t",
        YELLOW_TEXT(
            VERSION_MAJOR, ".",
            VERSION_MINOR, ".",
            VERSION_PATCH
        )
    )

    op::load_instructions(OPCODEFILE);
    LOGFMT(
        "OPCODE",
        GREEN_TEXT("Loading Operations Successful!")
    )

    string tstop = "00";
    if(argc > 1) tstop = argv[1];
    op::instruction tst;
    tst = op::instr_table[base::hextobin(tstop)];
    LOGFMT(
        "OPCODE",
        "operation 0x", tstop, ": ",
        PURPLE_TEXT("\n\tname: ", tst.mnemonic),
        YELLOW_TEXT("\n\tfmt:  ", tst.format)
    )

    return 0;
}