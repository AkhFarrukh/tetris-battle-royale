#include "tetris.hpp"


//Constructor
Tetris::Tetris():socketServer{-1}, isInGame{false}, username{""}{};

//Destructor
Tetris::~Tetris()
{
    exitTetris();
}

//Methods
/**
 * @brief Properly notify the server that the player's app is closing
 * 
 */
void Tetris::exitTetris() {
    if(socketServer != -1){
        if(isInGame){
            int type = MessageType::T_Input;
            int size = sizeof(InputMessage);
            InputMessage inputMessage;
            inputMessage.input = EXIT_KEY;
            sendAll(socketServer, &type, sizeof(int));
            sendAll(socketServer, &size, sizeof(int));
            sendAll(socketServer, &inputMessage, sizeof(InputMessage));
        }
    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    sendAll(socketServer, &exitType, sizeof(int));
    sendAll(socketServer, &size_, sizeof(int));
    }
}

/**
 * @brief Connect to the server
 * 
 * @return int : socket fd
 */
int Tetris::createSocket() {
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer == -1) {
        perror("Erreur lors de la création du socket : ");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide : ");
        close(socketServer);
        return -1;
    }

    if (connect(socketServer, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Échec de la connexion : ");
        close(socketServer);
        return -1;
    }
    return socketServer;
}



int Tetris::runTetris(int argc, char** argv) {
    // ==== 1. Connexion au serveur ====
    socketServer = createSocket();
    if (socketServer < 0) return 1;

    pthread_t threadID;
    std::shared_ptr<Player::ThreadData> threadData = std::make_shared<Player::ThreadData>();
    threadData->servSocket = socketServer;
    //Game loop
    if (argc > 1 && std::string(argv[1]) == "--gui") 
    {
        threadData->isGui = true;
        return runGuiApp(argc, argv, socketServer, threadData);
    } 
    else 
    {
        InterfaceTerminal terminal;
        terminal.setSock(socketServer);
        //Identification auprès de la db
        vector<std::string> dataConnexion = terminal.handleConnexionOption();
        bool retFlag;
        int retVal = authentification(dataConnexion, threadData, socketServer, threadID, retFlag);
        if (retFlag)
            return retVal;
        while(isInGame && keepRunning){
            MenuController menuController (terminal, threadData);
            menuController.handleMenu();
        }

        keepRunning = false;//thread principal a terminé et va demander à threadLecture de se terminer.
        shutdown(socketServer, SHUT_RD);
        pthread_join(threadID, nullptr);

        //on remet ces fct dans leurs etats initiaux (utile notamment si on a fermé le programme avec un SIGINT)
        nocbreak();
        keypad(stdscr, false);
        curs_set(1);
        nodelay(stdscr, false);
        retVal = system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
        if (!WIFEXITED(retVal)) {
            std::cout << "stty echo failed" << std::endl;
        }
        close(socketServer); // Fermer le socket après utilisation
        std::string command = "clear";
        retVal = system(command.c_str());
        if (!WIFEXITED(retVal)) {
            std::cout << "clear failed" << std::endl;
        }
        return 0;

    }
}

int Tetris::authentification(std::vector<std::string> &dataConnexion, std::shared_ptr<Player::ThreadData> &threadData, int socketServer, pthread_t &threadID, bool &retFlag)
{
    retFlag = true;
    if (dataConnexion.size() == 2)
    {
        username = dataConnexion[0];
        isInGame = true;
        threadData->userInGame = false;
        threadData->socket = socketServer;
        threadData->userInLobby = false;
        if (pthread_create(&threadID, nullptr, Player::threadRoutineView, threadData.get()) < 0)
        {
            perror("Error on thread creation : ");
            close(socketServer); // Fermer le socket après utilisation
            return 0;
        }
    }
    else
    {
        std::cout << "Echec de la connexion au compte.\n";
        nocbreak();
        keypad(stdscr, false);
        curs_set(1);
        nodelay(stdscr, false);
        int ret = system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
        if (!WIFEXITED(ret)) {
            std::cout << "stty echo failed" << std::endl;
        }
        close(socketServer);         // Fermer le socket après utilisation
        std::string command = "clear";
        ret = system(command.c_str());
        if (!WIFEXITED(ret)) {
            std::cout << "clear failed" << std::endl;
        }
        return 0;
    }
    retFlag = false;
    return 0;
}
