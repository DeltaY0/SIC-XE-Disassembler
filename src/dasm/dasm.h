#pragma once
#include  "opcode_parser.h"
#include "../util/cli.h"

#include <vector>
using std::vector;

namespace sic {

struct asmline {
    i32 address;
    op::instruction inst;
    string operand;
    string objcode;

    i32 len; // length in bytes
};

class dasm {
private:
    string prog_name;
    u32 start_addr;
    u32 prog_len;

    string objfile;

    string asmfile;
    string symtabfile;

    u32 locctr; // location counter
    vector<u8> memory; // memory map to split the object code to bytes
    vector<bool> is_initialized; // true = code/data exist here. false = RESW/RESB

    // dissambly output (vector of asmlines to store the program)
    vector<asmline> assembly;

    void process_obj_file();
    void process_header(string record);
    void process_text(string record);
    void process_end(string record);

    asmline decode_instruction(const u32 &address);

public:

    dasm(string objfile, string asmfile, string symtabfile);
    void run();
    void disassemble();
    void write_asm_to_file();
    void generate_symbol_table();
};

} // namespace sic