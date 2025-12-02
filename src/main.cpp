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
    LOG_CHANGE_PRIORITY(LOG_ERROR);

    // load opcodes from the resource file
    try {
        op::load_instructions(OPCODES_FILE);
    }
    catch (const ylib::Error& e) {
        LLOG("[", RED_TEXT("ERROR"), "]:\t")
        LLOG(e.what())
        return e.errcode();
    }

    vector<string> args(argv, argv + argc);
    if(args.size() == 1 || (args.size() == 2 && (args[1] == "-?" || args[1] == "--version")))
    {
        ylib::output_project_info();
        exit(0);
    }

    if(args.size() == 2)
    {
        if(args[1] == "-h" || args[1] == "--help" || args[1] == "help" || args[1] == "h")
        {
            ylib::output_help_info(commands);
            exit(0);
        }
    }

    ylib::CommandInfo cmd_info;

    try {
        cmd_info = ylib::parse_cli(args, commands);
    }
    catch(ylib::Error &err) {
        LOGFMT(
            "CMD",
            RED_TEXT("error: couldn't parse commands.\n\t"),
            err.what()
        )
        exit(1);
    }

    try {
        cmd_info.call_function();
    }
    catch(ylib::Error &err) {
        LOGFMT(
            "CMD",
            RED_TEXT("error: couldn't call the command:"), cmd_info.cmd.name, "\n\t",
            err.what()
        )
        exit(1);
    }

    return 0;
}