#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <atomic>

extern std::atomic<bool> keepRunning; // to stop the program (if ctrl C for example)

#endif // GLOBAL_HPP