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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --gui                         Launch with GUI interface" << std::endl;
    std::cout << "  --ip <server_ip>              Connect to a specific server IP (default: 127.0.0.1)" << std::endl;
    std::cout << "Example: " << programName << " --ip 192.168.129.8 --gui" << std::endl;
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
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            delete[] newArgv;
            return 0;
        } else {
            // Keep other arguments (e.g., --gui)
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