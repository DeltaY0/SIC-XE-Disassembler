#include "core/logger.h"
#include "core/error.h"

#include "util/opcode_parser.h"
#include "dasm/dasm.h"

#define OPCODES_FILE "res/opcodes.txt"

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

    if (argc < 3) {
        LLOG("[", RED_TEXT("USAGE ERROR"), "]:\t")
        LLOG(
            YELLOW_TEXT("Usage: "),
            "ysicxe <input_object_file> <output_assembly_file>\n"
        )

        return 1;
    }

    string infile = argv[1];
    string outfile = argv[2];

    try {
        // load opcodes from the resource file
        op::load_instructions(OPCODES_FILE);

        LOGFMT(
            "OPCODE",
            GREEN_TEXT("Loading Operations Successful!"), "\n"
        )

        // call the disassembler to do the work
        sic::dasm dasm(infile, outfile);
        dasm.run();

    } catch (const ylib::Error& e) {
        LLOG("[", RED_TEXT("ERROR"), "]:\t")
        LLOG(e.what())
        return e.errcode();
    }

    return 0;
}