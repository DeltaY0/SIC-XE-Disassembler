#pragma once

#include "../core/defines.h"
#include "../core/error.h"
#include "../core/logger.h"

#include "../util/base.h"

namespace op {

struct instruction {
    string mnemonic;
    u8 opcode; // opcode is 8-bits
    u8 format; // only 2 bits needed

    instruction() {}
    instruction(string n, u8 opc, u8 fmt)
        :mnemonic(n), opcode(opc), format(fmt)
    {}
};

// global map to store the instructions
// maps opcode -> instruction (since we are disassembling)
inline map<u8, instruction> instr_table;

const map<u8, string> reg_table = {
    {0, "A"},
    {1, "X"},
    {2, "L"},
    {3, "B"},
    {4, "S"},
    {5, "T"},
    {6, "F"},
    {8, "PC"},
    {9, "SW"}
};

inline void load_instructions(string filepath) {
    ifstream file(filepath);
    
    if(!file.is_open()) {
        string msg = "couldn't open file at " + filepath;
        throw ylib::Error(msg.c_str());
    }

    string line;
    while(getline(file, line)) {
        stringstream ss(line);

        string mnemonic;
        string opcode;
        u8 format;

        ss >> mnemonic >> opcode >> format;

        u8 opc = base::hextobin<u8>(opcode);

        instruction inst(
            mnemonic,
            opc,
            format
        );

        instr_table[opc] = inst;
    }
}

};