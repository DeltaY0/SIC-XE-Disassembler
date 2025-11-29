#include "dasm.h"

// constructor
sic::dasm::dasm(string objfile, string outfile) {
    locctr = 0;
    this->objfile = objfile;
    this->outfile = outfile;
}

// main dasm function
void sic::dasm::run() {

    // hide cursor in terminal
    std::cout << "\033[?25l";

    // phase 1: loader
    // read HTE recs and fill memory map
    cli::draw_progress(0, 30, "loading obj file...");

    process_obj_file();

    // phase 2: disassembler
    // iterate through memory map, handle gaps, and decode instructions
    cli::draw_progress(30, 70, "decoding instructions...");

    disassemble();

    // phase 3: saving
    // write the formatted assembly to a file
    cli::draw_progress(70, 100, "writing to file");

    write_asm_to_file();

    // reset cursor in terminal
    std::cout << "\033[?25h"; 
    std::cout << std::endl;

    LOGFMT(
        "MAIN",
        GREEN_TEXT("disassembly successful!\n"),
        "\toutput saved to: ", outfile, "\n"
    )
}

void sic::dasm::disassemble() {
    u32 curr = start_addr;
    u32 end = start_addr + prog_len;

    assembly.clear();

    while(curr < end) {

        // case: gap exists (resw/resb)
        if(curr < is_initialized.size() && !is_initialized[curr]) {
            asmline gapline;
            gapline.address = curr;

            // calc gap size
            u32 gap_start = curr;
            while(curr < end && !is_initialized[curr]) {
                curr++;
            }
            u32 size = curr - gap_start;
            gapline.len = size;

            // heuristic to determine resw or resb (innacurate)
            if(size % 3 == 0) {
                gapline.inst.mnemonic = "RESW";
                gapline.operand = std::to_string(size / 3);
            } else {
                gapline.inst.mnemonic = "RESB";
                gapline.operand = std::to_string(size);
            }

            gapline.objcode = ""; // no object code generated
            assembly.push_back(gapline);
            continue;
        }

        // case: instruction
        asmline line = decode_instruction(curr);

        assembly.push_back(line);

        // advance
        curr += std::max(line.len, (i32)1);
    }
}

void sic::dasm::write_asm_to_file() {
    ofstream out(outfile);

    using std::left, std::endl, std::setw;

    // table header
    out << left << setw(8)  << "LOC"      // location
        << left << setw(10) << "LABEL"    // label (if any)
        << left << setw(10) << "MNEMONIC" // instruction
        << left << setw(18) << "OPERAND"  // operands
        << "OBJ CODE"                     // hex at the end
        << endl;

    out << "-------------------------------------------------------------" << endl;

    out << left << setw(8)  << "" 
        << left << setw(10) << "" 
        << left << setw(10) << "BLOCK" 
        << prog_name << endl;

    for(const auto &line : assembly) {
        // column 1 - loc
        out << left << setw(8) << base::bintohex(line.address, 4);

        // column 2 - label 
        // TODO: surprise: we don't have labels yet!
        out << left << setw(10) << "";

        // column 3 - mnemonic
        out << left << setw(10) << line.inst.mnemonic;

        // column 4 - operand
        out << left << setw(18) << line.operand;

        // column 5 - obj code
        out << line.objcode;

        out << endl;
    }

    // table footer
    out << "-------------------------------------------------------------" << endl;
    out << left << setw(8)  << ""
        << left << setw(10) << ""
        << left << setw(10) << "END"
        << prog_name << endl;

    out.close();
}

// internal functions
void sic::dasm::process_obj_file() {
    ifstream file(objfile);
    
    LDEBUG(true, "\nopenning obj file for parsing...\n")
    if(!file.is_open()) {
        string msg = "couldn't open .obj file at " + objfile;
        throw ylib::Error(msg.c_str());
    }

    LDEBUG(true, GREEN_TEXT("\nopenned obj file sucessfuly...\n"))

    string line;
    while(getline(file, line)) {
        if(line.empty()) continue; // ignore empty lines

        string clean_line = "";
        for (char c : line) {
            if (c != '^' && c != '\r') {
                clean_line += c;
            }
        }

        LDEBUG(true, "\noutputting HTE line:\t", clean_line, "\n")

        char rec_type = clean_line[0];

        switch (rec_type)
        {
        case 'H':
            process_header(clean_line);
            break;
        case 'T':
            process_text(clean_line);
            break;
        case 'M':
            // TODO: process modifications for linker/loader
            break;
        case 'E':
            process_end(clean_line);
            break;
        
        default:
            // ignore anything else
            break;
        }

        // just stop if you read E
        if(rec_type == 'E') break;
    }

    file.close();
}

void sic::dasm::process_header(string record) {
    // H^PROGNAME^STARTADDR^LENGTH
    // 0 1        7         13

    LDEBUG(true, "processing header\t", record, "\n\tlength: ", record.length(), "\n");
    
    // prog name: 6 characters
    string prog_name = record.substr(1, 6);
    this->prog_name = prog_name;

    // start: 6 characters
    string start_addr_str = record.substr(7, 6);
    u32 start_addr = base::hextobin<u32>(start_addr_str);

    string prog_len_str = record.substr(13, 6);
    u32 prog_len = base::hextobin<u32>(prog_len_str);
    this->prog_len = prog_len;

    // len = end - start => end = len + start
    u32 end_addr = prog_len + start_addr;

    // initialize memory map with the proper size (with extra padding)
    memory = vector<u8>(end_addr + 10, 0);
    is_initialized = vector<bool>(end_addr + 10, false);

    // init class variables
    this->start_addr = start_addr;
    this->locctr = start_addr;
}

void sic::dasm::process_text(string record) {
    // ex: T^000000^00^..............
    //       ^----- 6 characters

    LDEBUG(true, "processing text\t", record, "\n\tlength: ", record.length(), "\n");
    
    string start_addr_str = record.substr(1, 6);
    u32 curr_addr = base::hextobin<u32>(start_addr_str);

    // length
    string record_len_str = record.substr(7, 2);
    u32 record_len = base::hextobin<u32>(record_len_str);

    string object_code = record.substr(9);
    for(i32 i = 0; i < record_len; i++) {
        if (2 * i + 1 >= object_code.length())
            break;

        string byte_str = object_code.substr(2 * i, 2);

        if (!isxdigit(byte_str[0]) || !isxdigit(byte_str[1]))
            continue; // skip invalid garbage

        u8 val = base::hextobin<u8>(byte_str);

        // write this byte to the memory map
        if(curr_addr + i < memory.size()) {
            memory[curr_addr + i] = val;
            is_initialized[curr_addr + i] = true;
        }
    }
}

void sic::dasm::process_end(string record) {
    // E^000000
    if (record.length() >= 7) {
        string start_exec_str = record.substr(1, 6);
        this->locctr = base::hextobin<u32>(start_exec_str);
    }
}

sic::asmline sic::dasm::decode_instruction(const u32 &addr)
{
    asmline line;
    line.address = addr;

    // shouldn't happend but just in case
    if(addr >= memory.size()) {
        line.len = 0; // end of program
        return line;
    }

    // get byte from memory map
    u8 byte1 = memory[addr];
    u8 opcode = byte1 & 0xFC; // mask off the least 2 bits (n i flags)
    
    // unkown opcode
    if(op::instr_table.find(opcode) == op::instr_table.end()) {
        // unkown opcode. handle as data
        line.inst.mnemonic = "BYTE";
        line.len = 1;
        line.objcode = base::bintohex(byte1, 2); // width 2 characters
        line.operand = "X'" + line.objcode + "'"; // X'00'
        return line;
    }

    op::instruction inst = op::instr_table[opcode];
    line.inst = inst;

    // format 1
    if(inst.format == 1) {
        line.len = 1;
        line.objcode = base::bintohex(byte1, 2);
        return line;
    }

    // format 2
    if(inst.format == 2) {
        line.len = 2;
        if (addr + 1 >= memory.size()) { line.len=0; return line; }
        u8 byte2 = memory[addr + 1];
        line.objcode = base::bintohex(byte1, 2) + base::bintohex(byte2, 2);

        // [opcode][r1][r2]
        //    8     4   4
        u8 r1 = (byte2 >> 4) & 0xF; // discard r2 and mask off opcode bits
        u8 r2 = byte2 & 0xF;

        // get register names, if register is unkown just put U
        string reg1 = op::reg_table.count(r1) ? op::reg_table.at(r1) : "U";
        string reg2 = op::reg_table.count(r2) ? op::reg_table.at(r2) : "U";

        if(inst.mnemonic == "CLEAR" || inst .mnemonic == "TIXR" || inst.mnemonic == "SVC") {
            // one operand instructions
            line.operand = reg1;
        } else {
            line.operand = reg1 + ", " + reg2;
        }

        return line;
    }

    if (addr + 2 >= memory.size()) { line.len=0; return line; }
    
    // format 3/4
    u8 byte2 = memory[addr + 1];
    u8 byte3 = memory[addr + 2];

    // [opcode][nixbpe][disp]
    //               ^----- in byte 2 000e0000
    // n i x b p e
    //     0 0 0 e 0 0 0 0

    // check if extended flag is on
    bool ext = (byte2 >> 4) & 1;

    if(ext) {
        // fmt 4: [opcode][nixbpe][addr]
        line.len = 4;
        line.inst.mnemonic = "+" + inst.mnemonic;
        line.inst.format = 4;

        if (addr + 3 >= memory.size()) { line.len=0; return line; }
        u8 byte4 = memory[addr + 3];
        line.objcode = base::bintohex(byte1, 2) + base::bintohex(byte2, 2) + 
                       base::bintohex(byte3, 2) + base::bintohex(byte4, 2);

        // address calculation (full 20 bits)
        // [opcode][nixbpe][addr]
        //                 16 bits = bytes 2 and 3
        // add the 4 bits from byte 2
        u32 full_addr = (byte2 & 0xF) << 16;
        full_addr |= (byte3 << 8);
        full_addr |= byte4;

        // n i flags -> n = 0, i = 1
        if(!(byte1 & 0b0010) && (byte1 & 0b0001))
            line.operand = "#"; // immediate

        // n i flags -> n = 1, i = 0
        if((byte1 & 0b0010) && !(byte1 & 0b0001))
            line.operand = "@"; // indirect

        if (!(byte1 & 0b0010) && (byte1 & 0b0001))
            line.operand += std::to_string(full_addr); // show Value
        else
            line.operand += base::bintohex(full_addr, 4); // show Address

        // indexed
        if((byte2 >> 7) & 1) line.operand += ", X";
    }
    else {
        // fmt 3: [opcode][nixbpe][disp]

        line.len = 3;
        line.objcode = base::bintohex(byte1, 2) +
            base::bintohex(byte2, 2) + base::bintohex(byte3, 2);

        // disp = 12 bits = 4 bits from byte2 | byte3
        i32 disp = ((byte2 & 0b1111) << 8) | byte3;

        // if disp is negative -> extend 2's complement
        if (disp & 0x800) disp |= 0xFFFFF000;

        // addressing
        // x b p e x x x x
        // 0 b p 0 0 0 0 0
        bool pc_rel =   (byte2 >> 5) & 1;
        bool base_rel = (byte2 >> 6) & 1;

        i32 target = disp;
        if(pc_rel) target += (addr + 3);

        // we don't have the content of Base register....
        // we would need a symbol table
        string addr_suffix = "";
        if (base_rel) {
            addr_suffix = "+ (B)"; 
        }

        // n i flags -> n = 0, i = 1
        if(!(byte1 & 0b0010) && (byte1 & 0b0001))
            line.operand = "#"; // immediate

        // n i flags -> n = 1, i = 0
        if((byte1 & 0b0010) && !(byte1 & 0b0001))
            line.operand = "@"; // indirect

        if (!(byte1 & 0b0010) && (byte1 & 0b0001))
            line.operand += std::to_string(target); // show Value
        else
            line.operand += base::bintohex(target, 4) + addr_suffix; // show Address


        // indexed
        if((byte2 >> 7) & 1) line.operand += ", X";
    }

    return line;
}
