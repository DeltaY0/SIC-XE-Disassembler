#pragma once

#include "../core/defines.h"

namespace sic::cli {

// callback for 'dasm' command
void handle_dasm(std::vector<std::string> &cmdIn, std::map<std::string, std::string> &args);

// callback for 'link' command
void handle_linker(std::vector<std::string> &cmdIn, std::map<std::string, std::string> &args);

} // namespace sic::cli