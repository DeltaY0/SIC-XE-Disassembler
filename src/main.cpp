#include "dasm/dasm.h"
#include "cmd/cmd.h"

#include "cmd/handlers.h"

#define OPCODES_FILE "res/opcodes.txt"

#pragma region commands

using Cmd = ylib::Command;
using CmdArg = ylib::CommandArgument;

vector<Cmd> commands = {
    // general
    Cmd("help", "view help information"),
    // dasm
    Cmd("dasm", "<file> [args...]\tDisassemble a SIC/XE object file into assembly source", {
        // input (can be positional or via flag)
        CmdArg("input", "path to input object file (.obj)", "-i", "--input"),
        // output is optional (default to out.asm)
        CmdArg("output", "path to output source file (.asm) [default: out.asm]", "-o", "--output"),
        // symbol table (pass 1 of linker output) to make code readable
        CmdArg("symtab", "path to external symbol table for label resolution", "-s", "--symtab", ylib::ValueType::STRING),
    }, sic::cli::handle_dasm),

    // linker
    Cmd("link", "[args...]\tLink multiple object files into a single loadable memory image", {
        // inputs (comma separated)
        CmdArg("inputs", "input object files (comma separated or without flags)", "-i", "--inputs"),
        // final memory view/executable
        CmdArg("output", "path to output executable/memory-dump", "-o", "--output"),
        // starting load address
        CmdArg("address", "starting load address [hex] (e.g., 4000)", "-a", "--addr"),
        // export global symbol table
        CmdArg("export", "export global symbol table to file", "-e", "--export-estab")
    }, sic::cli::handle_linker),
};

#pragma endregion

i32 main(i32 argc, char *argv[]) {

    // TODO: implement command line tool from ylib
    //          to handle subcommands like dasm, or link, help, etc...


    // handling commands

    vector<string> args(argv, argv + argc);
    // NOTE: to look for a value in the commandArgument map remember: 
    //          the key is the name of the argument.
    

    // end handling commands

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