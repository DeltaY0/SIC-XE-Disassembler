#include  "../util/opcode_parser.h"

namespace sic {

struct asmline {
    i32 address;
    op::instruction inst;
    string operand;
    string objcode;

    i32 len; // length in bits
};


class dasm {
private:
    i32 locctr; // location counter
    string objfile;
    string outfile;

    void process_header(string record);
    void process_text(string record);
    void process_end(string record);

    asmline decode_instruction(string obj_code_snippet);

public:

    dasm(string objfile, string outfile);
    void run();
};

} // namespace sic