#include "dasm/dasm.h"

#define OPCODES_FILE "res/opcodes.txt"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 10

i32 main(i32 argc, char *argv[]) {

    // TODO: linker/loader
    //          implement linker and loader for program relocation
    //          implement support for Modification records

    // TODO: better dasm
    //          where you provide also the symbol table for better disassembling
    //          would handle data inside any text record and base-relative addressing.

    LOG_CHANGE_PRIORITY(LOG_ERROR);

    LOGFMT(
        "MAIN",
        "version\t",
        YELLOW_TEXT(
            VERSION_MAJOR, ".",
            VERSION_MINOR, ".",
            VERSION_PATCH
        )
    )

    if (argc < 4) {
        LLOG("[", RED_TEXT("USAGE ERROR"), "]:\t")
        LLOG(
            YELLOW_TEXT("Usage: "),
            "ysicxe <input_object_file> <output_assembly_file> <output_symtab_file>\n"
        )

        return 1;
    }

    string objfile = argv[1];
    string asmfile = argv[2];
    string symtabfile = argv[3];

    try {
        // load opcodes from the resource file
        op::load_instructions(OPCODES_FILE);

        LOGFMT(
            "OPCODE",
            GREEN_TEXT("loading operations successful!"), "\n"
        )

        // call the disassembler to do the work
        sic::dasm dasm(objfile, asmfile, symtabfile);
        dasm.run();

    } catch (const ylib::Error& e) {
        LLOG("[", RED_TEXT("ERROR"), "]:\t")
        LLOG(e.what())
        return e.errcode();
    }

    return 0;
}