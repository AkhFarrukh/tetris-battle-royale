#include "Tetris.hpp"
#include "Global.hpp"
#include <atomic>

using namespace std;

//Signal handlers
void handleSigint(int sig)
{
    keepRunning = false;
}

int main (int argc, char** argv)
{
    signal(SIGINT, handleSigint);

    unique_ptr<Tetris> tetris = make_unique<Tetris>();
    tetris->runTetris(argc, argv);
    
    return 0;
}