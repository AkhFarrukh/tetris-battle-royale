#include "tetris.hpp"


atomic<bool> keepRunning = true;
//Signal handlers
void handleSigint(int sig)
{
    keepRunning = false;
}

int main ()
{
    signal(SIGINT, handleSigint);

    unique_ptr<Tetris> tetris = make_unique<Tetris>(keepRunning);
    char ** argv;
    tetris->runTetris(0, argv);
    
    return 0;
}