#include "linker.h"

// helpers


// public functions
sic::linker::linker() : prog_addr(0), cs_addr(0), total_len(0) {}

void sic::linker::add_file(string filename) {
    obj_files.push_back(filename);
}

void sic::linker::run(u32 start_addr) {
    this->prog_addr = start_addr;
    
    cli::init_progress_bar();

    cli::draw_progress(0, 50,  "[linker] starting pass 1...");
    pass1();

    cli::draw_progress(50, 100, "[linker] starting pass 2...");
    pass2();

    cli::reset_terminal();
    
    LOGFMT(
        "LINKER",
        GREEN_TEXT("linking successful!\n")
    )
}

void sic::linker::write_memory_to_file(string filepath) {
    ofstream out(filepath, std::ios::binary);
    
    if (!out.is_open()) {
        throw ylib::Error("Linker: Could not open output file " + filepath);
    }

    // write raw binary memory
    out.write(reinterpret_cast<const char*>(memory.data()), memory.size());
    out.close();

    LOGFMT("LINKER", "Memory dump saved to: ", CYAN_TEXT(filepath), "\n");
}

void sic::linker::write_estab_to_file(string filepath) {
    ofstream out(filepath);

    if (!out.is_open()) {
        throw ylib::Error("linker: Could not open export file " + filepath);
    }

    using std::left, std::setw, std::endl;

    // Header
    out << left << setw(10) << "SYMBOL" 
        << left << setw(10) << "ADDRESS" << endl;
    out << "--------------------" << endl;
    
    // Data
    for (const auto &[sym, addr] : estab) {
        out << left << setw(10) << sym 
            << base::bintohex(addr, 6) << endl;
    }
    out.close();

    LOGFMT("LINKER", "symbol table exported to: ", CYAN_TEXT(filepath), "\n");
}

// private functions
void sic::linker::pass1() {
    // start control section at the beginning
    cs_addr = prog_addr;

    for(const auto &filepath : obj_files) {
        ifstream file(filepath);
        if(!file.is_open()) {
            throw ylib::Error("linker cannot open obj file at: " + filepath);
        }

        string line;
        u32 cs_len = 0; // length of this current file
        
        while(getline(file, line)) {
            if(line.empty()) continue;

            string clean_line = "";
            for (char c : line) {
                if (c != '^' && c != '\r') {
                    clean_line += c;
                }
            }

            if(clean_line.empty()) continue;

            switch (clean_line[0])
            {
            case 'H':
                parse_header(clean_line, cs_len);
                break;
            case 'D':
                parse_define(clean_line);
                break;
            case 'E':
                break;
            default:
                break;
            }
        }

        file.close();

        // advance to the next available memory slot
        cs_addr += cs_len;
    }

    total_len = cs_addr - prog_addr;
}

void sic::linker::pass2() {
    // reset to start
    cs_addr = prog_addr;

    // alloc memory -> clear + resize and fill with garbage (0xFF)
    memory.clear();
    memory.resize(prog_addr + total_len, 0xFF);

    for(auto const &filepath : obj_files) {
        ifstream file(filepath);
        if(!file.is_open()) {
            throw ylib::Error("linker pass 2 cannot open obj file at: " + filepath);
        }

        string line;
        u32 curr_cs_len = 0;
        while(getline(file, line)) {
            if(line.empty()) continue;

            string clean_line = "";
            for (char c : line) {
                if (c != '^' && c != '\r') {
                    clean_line += c;
                }
            }

            if(clean_line.empty()) continue;

            char rec = clean_line[0];

            if(rec == 'H') {
                string len_str = clean_line.substr(13, 6);
                curr_cs_len = base::hextobin<u32>(len_str);
            }
            else if(rec == 'T') {
                parse_text(clean_line);
            }
            else if(rec == 'M') {
                parse_modify(clean_line);
            }
        }

        file.close();

        // advance to next memory slot
        cs_addr += curr_cs_len;
    }
}

// parsing records
void sic::linker::parse_header(string record, u32 &curr_cs_len) {
    // H ^ PROGNAME ^ START ^ LENGTH
    // 0   1          7       13

    string prog_name = sic::trim(record.substr(1, 6));

    // maps prog_name -> start address of current control section
    if(!prog_name.empty()) {
        if(estab.count(prog_name) > 0) {
            throw ylib::Error("duplicate global symbol: " + prog_name);
        }

        estab[prog_name] = cs_addr;
    }

    string len_str = record.substr(13, 6);
    curr_cs_len = base::hextobin<u32>(len_str);
}

void sic::linker::parse_define(string record) {
    // D ^ SYM1 ^ ADDR1 ^ SYM2 ^ ADDR2 ...
    // starts at idx = 1. pairs of 12 chars = [symbol(6), addr(6)]
    
    u64 idx = 1;
    
    // loop over each entry until we run out
    while(idx + 12 <= record.length()) {

        // extract symbol
        string sym = sic::trim(record.substr(idx, 6));

        // extract relative address
        string addr_str = record.substr(idx + 6, 6);
        u32 rel_addr = base::hextobin<u32>(addr_str);

        // calc absolute address
        if(!sym.empty()) {
            if(estab.count(sym) > 0) {
                throw ylib::Error("duplicate global symbol: " + sym);
            }

            estab[sym] = cs_addr + rel_addr;
        }

        // move to next entry
        idx += 12;
    }
}

void sic::linker::parse_text(string record) {
    // T ^ START ^ LEN ^ CODE...
    // 0   1       7     9

    // get start addr (relative to file)
    string start_str = record.substr(1, 6);
    u32 rel_addr = base::hextobin<u32>(start_str);

    // get length
    string len_str = record.substr(7, 2);
    u32 len = base::hextobin<u32>(len_str);

    // get code
    string objcode = record.substr(9);

    for(u32 i = 0; i < len; i++) {
        // safety
        if((2 * i) + 2 > objcode.length()) break;

        string byte_str = objcode.substr(2 * i, 2);
        u8 val = base::hextobin<u8>(byte_str);

        // physical address
        u32 phys_addr = cs_addr + rel_addr + i;

        if(phys_addr < memory.size()) {
            memory[phys_addr] = val;
        }
        else {
            LOGFMT("LINKER", RED_TEXT("Fatal Error: Memory Overflow"));
            throw ylib::Error("sicxe memory overflow");
        }
    }
}

void sic::linker::parse_modify(string record) {
    // M ^ ADDR ^ LEN ^ SIGN ^ SYMBOL
    // 0   1      7     9      10

    // address to mod
    string addr_str = record.substr(1, 6);
    u32 rel_addr = base::hextobin<u32>(addr_str);

    // length (in half-bytes/nibbles)
    string len_str = record.substr(7, 2);
    u32 len_nibbles = base::hextobin<u32>(len_str);

    // sign (+ or -)
    char sign = record[9];

    // symbol to plus/minus
    string sym = sic::trim(record.substr(10));

    // apply the mod
    apply_mod(cs_addr + rel_addr, len_nibbles, sign, sym);

}

void sic::linker::apply_mod(u32 addr, u32 len_nibbles, char sign, string sym) {
    // validate symbol
    if(estab.count(sym) == 0) {
        LOGFMT("LINKER", RED_TEXT("Error: Undefined Global Symbol: ") + sym)
        return;
    }

    u32 sym_val = estab[sym];

    // check bounds
    if(addr + 2 >= memory.size()) {
        LOGFMT(
            "LINKER",
            RED_TEXT("Error: Modification out of bounds at "),
            base::bintohex(addr, 6)
        )

        return;
    }

    // read data (big endian)
    u32  b1 = memory[addr]; // not u8 cuz we are shifting by 16
    u16  b2 = memory[addr + 1];
    u8   b3 = memory[addr + 2];

    u32 curr_val = (b1 << 16) | (b2 << 8) | b3;

    if(len_nibbles == 5) {
        // fmt 4 address
        // only handle least 20 bits
        // don't touch most 4 bits (xbpe flags)

        u32 flags_mask = 0xF00000; // the most 4 bits we don't touch
        u32 addr_mask  = 0x0FFFFF; // the actual address

        u32 flags = curr_val & flags_mask;
        u32 addr_field = curr_val & addr_mask;

        // symbol calc
        addr_field += (sign == '+' ? sym_val : -sym_val);

        addr_field &= addr_mask; // reapply the mask just in case

        // put it together
        curr_val = flags | addr_field;
    }
    else if(len_nibbles == 6) {
        // standard word (24 bits)

        curr_val += (sign == '+' ? sym_val : -sym_val);

        curr_val &= 0xFFFFFF; // keep as 24 bits
    }
    else {
        LOGFMT(
            "LINKER", 
            YELLOW_TEXT("Warning: Unsupported M record length: "),
            len_nibbles
        );

        return;
    }

    // write to mem (big endian)
    memory[addr]     = (curr_val >> 16) & 0xFF;
    memory[addr + 1] = (curr_val >> 8)  & 0xFF;
    memory[addr + 2] =  curr_val        & 0xFF;
}
