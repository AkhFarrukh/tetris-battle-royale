#include "tetris.hpp"
#include "global.hpp"
#include <atomic>

using namespace std;

//Signal handlers
void handleSigint(int sig)
{
    keepRunning = false;
}

int main ()
{
    signal(SIGINT, handleSigint);

    unique_ptr<Tetris> tetris = make_unique<Tetris>();
    char ** argv;
    tetris->runTetris(0, argv);
    
    return 0;
}