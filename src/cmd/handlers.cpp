#include "handlers.h"

#include "../core/logger.h"
#include "../util/base.h"

#include "../dasm/dasm.h"
#include "../linker/linker.h"

#include <iomanip>

namespace sic::cli {

void handle_dasm(vector<string> &cmdIn, map<string, string> &args) {

    // handle input file (obj)
    string input_file;
    if (args.count("input")) {
        input_file = args["input"];
    } 
    else if (!cmdIn.empty()) {
        input_file = cmdIn[0];
    } 
    else {
        throw ylib::Error("DASM: No input file specified. Use -i <file> or pass the filename directly.");
    }

    // output file
    string output_file = "out.asm";
    if (args.count("output")) {
        output_file = args["output"];
    }

    // symtab file
    string symtab_file = "out.sym";
    if (args.count("symtab")) {
        symtab_file = args["symtab"];
    }

    // trim just in case of whitespace
    input_file  = sic::trim(input_file);
    output_file = sic::trim(output_file);
    symtab_file = sic::trim(symtab_file);

    // run dasm
    sic::dasm tool(input_file, output_file, symtab_file);
    tool.run();
}

void HandleLinker(vector<string> &cmdIn, map<string, string> &args) {
    sic::linker tool;
    vector<string> files_to_link;

    // --- parse input obj files ---
    // check for comma seperated values (-i flag)
    if (args.count("inputs")) {
        stringstream ss(args["inputs"]);
        string segment;
        while(getline(ss, segment, ',')) {
            // REFACTOR: Using sic::trim instead of manual loops
            string clean_seg = sic::trim(segment);
            if (!clean_seg.empty()) {
                files_to_link.push_back(clean_seg);
            }
        }
    }

    // add any positional (stdin) arguments
    for(const auto &f : cmdIn) {
        string clean_f = sic::trim(f);
        if(!clean_f.empty()) {
            files_to_link.push_back(clean_f);
        }
    }

    if (files_to_link.empty()) {
        throw ylib::Error("Linker: No input files specified. Use -i <files> or pass filenames directly.");
    }

    // add files to the linker
    for(const auto &file : files_to_link) {
        tool.add_file(file);
    }

    // parsse lead address
    u32 start_addr = 0x0000;
    if (args.count("address")) {
        // Trim before conversion to ensure hextobin works
        string addr_str = sic::trim(args["address"]);
        start_addr = base::hextobin<u32>(addr_str);
    }

    // run the linker
    tool.run(start_addr);

    // output
    if (args.count("output")) {
        tool.write_memory_to_file(sic::trim(args["output"]));
    }

    if (args.count("export")) {
        tool.write_estab_to_file(sic::trim(args["export"]));
    }
}

} // namespace sic::cli