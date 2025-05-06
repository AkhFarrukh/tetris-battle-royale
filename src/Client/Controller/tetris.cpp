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
    int bytesSentType = sendAll(socketServer, &exitType, sizeof(int));
    int bytesSentSize = sendAll(socketServer, &size_, sizeof(int));
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
            //Getting the selected MenuMessage in terminal
            int selectedMode = terminal.start();
            if (selectedMode == -1){
                //Quit game
                int exitType = MessageType::T_ForcedExit;
                int size_ = sizeof(int);
                int bytesSentType = sendAll(socketServer, &exitType, sizeof(int));
                int bytesSentSize = sendAll(socketServer, &size_, sizeof(int));
                isInGame = false;
            }
            else{
                /*
                *SHOULD BE ELSE IF AND CHANGE DEPENDING ON MenuMessage CLICKED
                */
                //Ask to start a game
                int type = MessageType::T_Menu;
                int size = sizeof(MenuMessage);
                //Defining the struct we will send  
                MenuMessage MenuMessage;
                MenuMessage.action = MenuActions::A_Play;
                if(selectedMode == 0){
                    MenuMessage.mode = Modes::Endless_;
                    //napms(500);
                }
                else if(selectedMode == 10){
                    MenuMessage.mode = Modes::Classic_;
                    MenuMessage.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 11){
                    MenuMessage.mode = Modes::Classic_;
                    MenuMessage.action = MenuActions::A_JoinRoom;
                }
                else if(selectedMode == 20){
                    MenuMessage.mode = Modes::Duel_;
                    MenuMessage.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 21){
                    MenuMessage.mode = Modes::Duel_;
                    MenuMessage.action = MenuActions::A_JoinRoom;
                }
                else if(selectedMode == 30){
                    MenuMessage.mode = Modes::Royal_;
                    MenuMessage.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 31){
                    MenuMessage.mode = Modes::Royal_;
                    MenuMessage.action = MenuActions::A_JoinRoom;
                }
                //Sending info to server
                if(sendAll(socketServer, &type, sizeof(int)) == false){
                    perror("Error sending MenuMessage info : ");
                    return 1;
                }
                if(sendAll(socketServer, &size, sizeof(int)) == false){
                    perror("Error sending MenuMessage info : ");
                    return 1;
                }
                if(sendAll(socketServer, &MenuMessage, sizeof(MenuMessage)) == false){
                    perror("Error sending MenuMessage info : ");
                    return 1;
                }
                threadData->mux.lock();
                threadData->userInLobby = true;
                threadData->gameMode = MenuMessage.mode;
                threadData->mux.unlock();

                Player::sendInput(threadData);
                threadData->mux.lock();
                threadData->userInGame = false;
                threadData->userInLobby = false;
                threadData->mux.unlock();
                napms(600);
                if(MenuMessage.action == MenuActions::A_CreateRoom){
                    int sendType = MessageType::T_RoomDeleted;
                    int sendSize = sizeof(int);
                    if(!sendAll(socketServer, &sendType, sizeof(sendType))){keepRunning = false; break;}
                    if(!sendAll(socketServer, &sendSize, sizeof(sendSize))){keepRunning = false; break;}
                }

            }
        }

        keepRunning = false;//thread principal a terminé et va demander à threadLecture de se terminer.
        shutdown(socketServer, SHUT_RD);
        pthread_join(threadID, nullptr);

        ///**********************************************************************************
        // * START GAME
        // *********************************************************************************/

        //on remet ces fct dans leurs etats initiaux (utile notamment si on a fermé le programme avec un SIGINT)
        nocbreak();
        keypad(stdscr, false);
        curs_set(1);
        nodelay(stdscr, false);
        system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
        close(socketServer); // Fermer le socket après utilisation
        std::string command = "clear";
        int result = system(command.c_str());
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
        system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
        close(socketServer);         // Fermer le socket après utilisation
        std::string command = "clear";
        int result = system(command.c_str());
        return 0;
    }
    retFlag = false;
    return 0;
}
