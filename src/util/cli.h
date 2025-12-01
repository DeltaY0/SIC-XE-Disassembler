#pragma once

#include "../core/defines.h"
#include "../core/logger.h"

#include <thread>
#include <mutex>

#define SLEEP_TIME_MS 5

namespace cli {

inline void init_progress_bar() {
    std::cout << "\033[?25l";
}

inline void reset_terminal() {
    std::cout << "\033[?25h"; 
    std::cout << std::endl;
}

inline string cursor = "|/-\\";

inline void draw_progress(i32 start_p, i32 end_p, std::string label, i32 width = 40) {
    using namespace std::chrono;

    LDEBUG(true, "loading progress bar\n")
    
    for (i32 p = start_p; p <= end_p; ++p) {
        double ratio = p / 100.0;
        i32 filled = ratio * width;

        // start line + yellow color
        std::cout << "\r" << "[" << TEXT_YELLOW;
        
        // draw filled part
        for (i32 i = 0; i < filled; ++i) std::cout << "#";
        std::cout << cursor[p % 4]; 

        // draw empty part
        for (i32 i = filled; i < width; ++i) std::cout << " ";
        
        // reset color, pri32 percentage and label
        std::cout << TEXT_WHITE << "]" << std::setw(3) 
                  << p << "% " << label
                  << std::string(20, ' ') << std::flush;

        // fake delay
        std::this_thread::sleep_for(milliseconds(SLEEP_TIME_MS));
    }
}

} // namespace cli