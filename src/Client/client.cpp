#include "Tetris.hpp"
#include "Global.hpp"
#include <atomic>
#include <string>
#include <vector>

using namespace std;

// Signal handlers
void handleSigint(int sig)
{
    keepRunning = false;
}


int main(int argc, char** argv)
{
    signal(SIGINT, handleSigint);

    // Default IP
    string serverIP = "127.0.0.1";

    // Parse command-line arguments to extract the IP while preserving the order of other arguments
    char** newArgv = new char*[argc]; // This will store the filtered arguments
    int newArgc = 0;

    // First, copy the program name
    newArgv[newArgc++] = argv[0];

    // Then process the remaining arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            // Found IP argument, store it
            serverIP = argv[i + 1];
            i++; // Skip the value in the next iteration
        } else {
            // Keep other arguments (--gui)
            newArgv[newArgc++] = argv[i];
        }
    }

    cout << "Connecting to server at: " << serverIP << endl;

    // Create Tetris with the specified IP
    unique_ptr<Tetris> tetris = make_unique<Tetris>(serverIP);

    // Run the application with the filtered arguments
    int result = tetris->runTetris(newArgc, newArgv);

    // Clean up
    delete[] newArgv;

    return result;
}