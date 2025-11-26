#include "core/logger.h"
#include "util/opcode_parser.h"
#include "core/error.h"

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

    string input_file = argv[1];
    string output_file = argv[2];

    try {
        // load opcodes from the resource file
        op::load_instructions("res/opcodes.txt");

        LOGFMT(
            "OPCODE",
            GREEN_TEXT("Loading Operations Successful!"), "\n"
        )

        // TODO: Call the disassembler to do the work
        // ex: sic::dasm::process(input_file, output_file);

    } catch (const ylib::Error& e) {
        LLOG("[", RED_TEXT("ERROR"), "]:\t")
        LLOG(e.what())
        return e.errcode();
    }

    return 0;
}