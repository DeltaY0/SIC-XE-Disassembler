#pragma once

#include "../core/logger.h"
#include "../core/error.h"

#include "../util/base.h"
#include "../util/cli.h"

namespace sic {

inline static string trim(const string& str) {
    const string whitespace = " \t\r\n";
    auto first = str.find_first_not_of(whitespace);

    if (string::npos == first) return "";

    auto last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

class linker{
private:
    vector<string> obj_files;

    map<string, u32> estab; // external symbol table
    vector<u8> memory; // final memory (including all progs)

    // state vars
    u32 prog_addr;  // starting addr for the whole program (combined)
    u32 cs_addr;    // starting addr of the current control section (file)
    u32 total_len;  // total length of the linked program

    // helpers
    void pass1();
    void pass2();

    // pass 1 -> record parsers
    void parse_header(string record, u32 &curr_cs_len);
    void parse_define(string record);

    // pass 2 -> record parsers
    void parse_text(string record);
    void parse_modify(string record);

    // helper for modification recs (nibble = half byte)
    void apply_mod(u32 addr, u32 len_nibbles, char sign, string symbol);

public:
    linker();

    void add_file(string filename);
    void run(u32 start_addr = 0x00000);

    // getters for priv fields (where's C# {get; private set} ??? im crying)
    vector<u8> get_memory() const { return memory; }
    u32 get_total_len() const { return total_len; }
    map<string, u32> get_estab() const { return estab; }

};

} // namespace sic