#include "dasm.h"

// constructor
sic::dasm::dasm(string objfile, string outfile) {
    locctr = 0;
    this->objfile = objfile;
    this->outfile = outfile;
}

// main dasm function
void sic::dasm::run() {

}

// internal functions
void sic::dasm::process_header(string record) {

}

void sic::dasm::process_text(string record) {

}

void sic::dasm::process_end(string record) {

}

sic::asmline sic::dasm::decode_instruction(string objcode) {

    sic::asmline res;

    // store the first 6 bits (first 2 hex digits)
    string opcodehex = objcode.substr(0, 2);

    u8 opcode = base::hextobin(opcodehex);
    opcode &= 0xFC; // Mask out the n and i bits to get the base opcode.

    if(op::instr_table.count(opcode) == 0) {
        string msg = "invalid opcode: " + opcodehex;
        throw ylib::Error(msg.c_str());
    }

    op::instruction instr = op::instr_table[opcode];

    res.inst = instr;
    res.objcode = objcode;

    // TODO: handle all the formats
    switch(instr.format) {
        case 1: {
            res.len = 1 * 8;

            // [opcode]
            //  8-bits
            res.operand = "";
        } break;

        case 2: {
            res.len = 2 * 8;
            // [opcode][r1][r2]
            //    8     4   4

        } break;

        case 3: {
            // TODO: handle format 3/4 diff
            // [opcode][nixbpe][disp/addr]
        } break;

        default:
            throw ylib::Error("invalid instruction format");
    }

    locctr += res.len;
    res.address = locctr;

    return res;
}
