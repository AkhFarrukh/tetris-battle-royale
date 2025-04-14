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

    unique_ptr<Tetris> tetris = make_unique<Tetris>();
    while (keepRunning)
    {

    }
    return 0;
}