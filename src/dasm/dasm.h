#pragma once
#include  "opcode_parser.h"
#include "../util/cli.h"

namespace sic {

struct asmline {
    i32 address;
    op::instruction inst;
    string operand;
    string objcode;

    i32 len; // length in bytes

    // symtab specific
    bool is_mem_ref = false;
    bool indexed = false;
    u32 target_address = 0;
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

    // symtab
    map<u32, string> symtab;
    u32 label_counter = 0;

    // dissambly output (vector of asmlines to store the program)
    vector<asmline> assembly;

    // helpers
    string get_label(u32 addr);
    
    // main methods
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
    void write_symtab_to_file();
};

} // namespace sic