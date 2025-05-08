#include "guiControlleur.hpp"

guiControlleur::guiControlleur(int sock, std::shared_ptr<Player::ThreadData> threadData)
    : QObject(), sock(sock), threadData(threadData) {}

guiControlleur::~guiControlleur(){}


void guiControlleur::sendCreateRoom() {
    MenuMessage menu;
    menu.mode = threadData->gameMode;
    menu.action = MenuActions::A_CreateRoom;

    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);

    if (!sendAll(sock, &type, sizeof(type)) ||
        !sendAll(sock, &size, sizeof(size)) ||
        !sendAll(sock, &menu, sizeof(MenuMessage))) {
        QMessageBox::critical(nullptr, "Erreur", "Échec de la création de la salle.");
        return;
    }
}

void guiControlleur::joinRoom(int roomID) {
    int type = MessageType::T_Join;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &roomID, sizeof(int));
}

void guiControlleur::sendForceStart() {
    MenuMessage menu;
    menu.action = MenuActions::A_ForceStart;
    // menu.mode = threadData->gameMode;
    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(MenuMessage));
}

void guiControlleur::sendLeaveRoom() {
    int type;
    int size = sizeof(int);

    if (threadData->isOwner) {
        type = MessageType::T_RoomDeleted;
    } else {
        type = MessageType::T_LeftRoom;
    }

    if (!sendAll(sock, &type, sizeof(int)) ||
        !sendAll(sock, &size, sizeof(int))) {
        QMessageBox::critical(nullptr, "Error", "Failed to leave room.");
        return;
    }

    threadData->mux.lock();
    threadData->userInLobby = false;
    threadData->roomInfoUpdated = false;
    threadData->isOwner = false;
    threadData->mux.unlock();

    requestRoomList();
}

void guiControlleur::requestRoomUpdate() {
    int type = MessageType::T_Refresh;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
}

void guiControlleur::requestRoomList() {
    MenuMessage menu;
    menu.mode = threadData->gameMode;
    menu.action = MenuActions::A_JoinRoom;

    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(MenuMessage));
}

void guiControlleur::handleAuth(const QString& username, const QString& password, bool isRegister, const QString& confirmPassword, bool isRegisterMode) {

    Connexion connexion;
    std::strncpy(connexion.username, username.toStdString().c_str(), sizeof(connexion.username) - 1); // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    std::strncpy(connexion.password, password.toStdString().c_str(), sizeof(connexion.password) - 1);

    int size = sizeof(struct Connexion);

    if (isRegisterMode) {
        // Vérifie la confirmation du mot de passe
        if (password != confirmPassword) {
            QMessageBox::warning(nullptr, "Erreur", "Les mots de passe ne correspondent pas.");
            return;
        }

        // Envoi d'une demande d'inscription
        int type = MessageType::T_Register;
        sendAll(sock, &type, sizeof(int));
        sendAll(sock, &size, sizeof(int));
        sendAll(sock, &connexion, sizeof(Connexion));

        int typeResponse, sizeResponse;
        if (!recvAll(sock, &typeResponse, sizeof(int)) || !recvAll(sock, &sizeResponse, sizeof(int))) {
            QMessageBox::critical(nullptr, "Erreur", "Erreur de communication avec le serveur.");
            return;
        }

        if (typeResponse != MessageType::T_AcceptConnexion) {
            QMessageBox::warning(nullptr, "Erreur", "Échec de l'inscription. Essayez un autre nom.");
            return;
        }

    } else {
        // Connexion simple
        int type = MessageType::T_Connexion;
        sendAll(sock, &type, sizeof(int));
        sendAll(sock, &size, sizeof(int));
        sendAll(sock, &connexion, sizeof(Connexion));

        int typeResponse, sizeResponse;
        if (!recvAll(sock, &typeResponse, sizeof(int)) || !recvAll(sock, &sizeResponse, sizeof(int))) {
            QMessageBox::critical(nullptr, "Erreur", "Erreur de communication avec le serveur.");
            return;
        }

        if (typeResponse != MessageType::T_AcceptConnexion) {
            QMessageBox::warning(nullptr, "Erreur", "Nom d'utilisateur ou mot de passe incorrect.");
            return;
        }
    }

    // Vérifie une double session dans tous les cas après connexion
    int doubleSession;
    int size_;
    if (!recvAll(sock, &doubleSession, sizeof(int)) || !recvAll(sock, &size_, sizeof(int))) {
        QMessageBox::critical(nullptr, "Erreur", "Erreur lors de la vérification de session.");
        return;
    }

    if (doubleSession == MessageType::T_RefuseConnexion) {
        QMessageBox::warning(nullptr, "Erreur", "Vous êtes déjà connecté sur une autre session.");
        return;
    }

    this->threadData->socket = sock;
    this->threadData->gameMode = Modes::Endless_; //this just a default mode, it will be overwritten later...
    this->threadData->userInGame = false;
    this->threadData->userInLobby = false;
    this->threadData->isGui = true;
    this->threadData->qtParent = this;

    pthread_create(&threadID, nullptr, guiControlleur::threadRoutineQtView, threadData.get());

    // Connexion réussie !
    // QMessageBox::information(this, "Succès", isRegisterMode ? "Inscription et connexion réussies !" : "Connexion réussie !");
    
}

void* guiControlleur::threadRoutineQtView(void* arg) {
    Player::ThreadData* threadData = static_cast<Player::ThreadData*>(arg);
    int sock = threadData->socket;
    MainWindow* mw = static_cast<MainWindow*>(threadData->qtParent);

    while (threadData->socket == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (!flag) {
        int type, size;
        if (!recvAll(sock, &type, sizeof(int)) || !recvAll(sock, &size, sizeof(int))) {
            perror("Disconnected or error");
            flag = true;
            break;
        }

        if (type == MessageType::T_Message) {
            Message message;
            if (!recvAll(sock, &message, size)) break;
            MainWindow* mw = static_cast<MainWindow*>(threadData->qtParent);
            if (!mw->chatHandler || !mw->chatHandler->getChatHistory()) break;
            QString sender = QString::fromUtf8(message.expediteur);
            QString msg = QString::fromUtf8(message.message);
            QMetaObject::invokeMethod(
                mw->chatHandler->getChatHistory(),
                [=]() {
                    mw->chatHandler->getChatHistory()->append("<b>" + sender + ": </b> " + msg);
                },
                Qt::QueuedConnection
            );            
        }

        else if (type == MessageType::T_Exit || type == MessageType::T_ForcedExit) {
            threadData->mux.lock();
            threadData->userInGame = false;
            threadData->userInLobby = false;
            threadData->isSpectator = false;
            threadData->mux.unlock();
            
            QMetaObject::invokeMethod(
                static_cast<QObject*>(threadData->qtParent),
                []() {
                    auto* mw = static_cast<MainWindow*>(qApp->activeWindow());
                    if (mw) mw->showGameOverPopup();
                }
            );            

            if (type == MessageType::T_ForcedExit) {
                flag = true;
                break;
            }
        
        }

        else if (type == MessageType::T_PlayerData || type == MessageType::T_EndGame) {
            std::vector<NetPlayerData> playersData;
            for (int i = 0; i < size; ++i) {
                NetPlayerData player;
                if (!recvAll(sock, &player.playerId, sizeof(player.playerId))) return nullptr;
                if (!recvAll(sock, &player.isGameOver, sizeof(player.isGameOver))) return nullptr;
                if (!recvAll(sock, &player.linesCleared, sizeof(player.linesCleared))) return nullptr;
                if (!recvAll(sock, &player.score, sizeof(player.score))) return nullptr;
                if (!recvAll(sock, &player.nrg, sizeof(player.nrg))) return nullptr;
                int gridSize;
                if (!recvAll(sock, &gridSize, sizeof(gridSize)) || gridSize < 0 || gridSize > 100000) break;
                player.grid.resize(gridSize);
                if (!recvAll(sock, player.grid.data(), gridSize * sizeof(Color))) return nullptr;
                if (type == MessageType::T_EndGame) player.endGame = true;
                playersData.push_back(player);
            }

            std::lock_guard<std::mutex> lock(threadData->mux);
            threadData->playersData = std::move(playersData);
            // threadData->clinetPlayerID = -1; 
            for (size_t i = 0; i < playersData.size(); ++i) {
                if (playersData.at(i).playerId == -1) {
                    threadData->clinetPlayerID = i;
                    break;
                }
            }            
        }

        else if (type == MessageType::T_GameStart) {
            std::cout << "t_gamgestart recived correctly" << endl;
            threadData->mux.lock();
            threadData->userInGame = true;
            threadData->userInLobby = true;
            threadData->mux.unlock();

            auto* mw = static_cast<MainWindow*>(threadData->qtParent);
            QMetaObject::invokeMethod(
                mw,
                [mw]() {
                    mw->setGameScreen();
                    mw->addMalusButtonsIfRoyal();
                },
                Qt::QueuedConnection
            );
                        
        }

        else if (type == MessageType::T_RoomInfo || type == MessageType::T_RoomJoinInfo) {
            RoomInfo info;
            if (!recvAll(sock, &info, size)) break;

            threadData->mux.lock();
            threadData->roomInfo = info;
            threadData->roomInfoUpdated = true;
            threadData->isOwner = (type == MessageType::T_RoomInfo);
            threadData->mux.unlock();
            std::cout << "thread data before sending" << threadData->isSpectator << endl;
            QMetaObject::invokeMethod(
                static_cast<QObject*>(mw),
                [info, isOwner = (type == MessageType::T_RoomInfo), mw, threadData]() {
                    if (threadData->isSpectator == 1) {
                        mw->setupSpectatorWaitingScreen(info.roomID);
                    } else {
                        mw->setupMyRoomScreen(info, isOwner);
                    }
                },
                Qt::QueuedConnection
            );
            // else {
            //     std::cout << "sent anyway" << threadData->isSpectator << endl;
            //     QMetaObject::invokeMethod(               
            //         static_cast<QObject*>(parent),
            //         [info, isOwner = (type == MessageType::T_RoomInfo), parent]() {
            //             parent->setupMyRoomScreen(info, isOwner);
            //         }
            //     );
            // }
            
        }

        else if (type == MessageType::T_RoomsInfo) {
            std::vector<RoomInfo> rooms(size);
            if (!recvAll(sock, rooms.data(), size * sizeof(RoomInfo))) break;

            int modeInt, modeSize;
            if (!recvAll(sock, &modeInt, sizeof(int))) break;
            if (!recvAll(sock, &modeSize, sizeof(int))) break;

            QMetaObject::invokeMethod(
                static_cast<QObject*>(mw),
                [rooms, mw]() {
                    mw->setupRoomListScreen(rooms);
                },
                Qt::QueuedConnection
            );
                    }

        else {
            std::cerr << "[QtView] Unhandled type: " << type << std::endl;
            char buffer[1024];
            recv(sock, buffer, std::min(size, 1024), 0);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return nullptr;
}

void guiControlleur::handleMalusBonus(int malusId) {
    static int chosenMalus = 0;
    static int chosenPlayer = 0;

    chosenMalus = malusId;

    if (chosenMalus == 6 || chosenMalus == 7) {
        int myself = -1;
        for (size_t i = 0; i < threadData->playersData.size(); ++i) {
            if (threadData->playersData[i].playerId == -1) {
                myself = static_cast<int>(i);
                break;
            }
        }
        if (myself == -1) return;

        sendMalusBonus(chosenMalus, myself);
        chosenMalus = 0;
        chosenPlayer = 0;
    } else {

        QStringList playerOptions;
        std::vector<int> playerIndices;
        for (size_t i = 0; i < threadData->playersData.size(); ++i) {
            if (threadData->playersData[i].playerId != -1) {
                playerOptions.push_back(QString::number(i));
                playerIndices.push_back(static_cast<int>(i));
            }
        }

        bool ok = false;
        QString chosen = QInputDialog::getItem(nullptr, "Select Target", "Choose target:", playerOptions, 0, false, &ok);

        if (!ok || chosen.isEmpty()) {
            return;
        }

        int selected = chosen.toInt();
        chosenPlayer = selected;

        sendMalusBonus(chosenMalus, chosenPlayer);

        chosenMalus = 0;
        chosenPlayer = 0;
    }
}

void guiControlleur::sendMalusBonus(int malusId, int targetPlayerIndex) {
    MalBonus mb;
    mb.malusB = Player::intToMalusBonus(malusId);
    mb.target = targetPlayerIndex;

    int type = MessageType::T_MalusBonus;
    int size = sizeof(MalBonus);

    if (!sendAll(sock, &type, sizeof(int)) ||
        !sendAll(sock, &size, sizeof(int)) ||
        !sendAll(sock, &mb, sizeof(MalBonus))) {
        QMessageBox::critical(nullptr, "Error", "Failed to send malus/bonus.");
    }
}

void guiControlleur::sendMenuAction(MenuActions action, Modes mode) {
    MenuMessage menu;
    menu.action = action;
    menu.mode = mode;
    threadData->mux.lock();
    threadData->socket = sock;
    threadData->gameMode = mode;
    threadData->isGui = true;
    // threadData->qtParent = this;
    threadData->userInGame = false;   // <-- will be set to true by threadRoutineView
    threadData->userInLobby = true;
    threadData->mux.unlock();


    if (!threadID) {
        pthread_create(&threadID, nullptr, guiControlleur::threadRoutineQtView, threadData.get());
    }
    
    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    
    if (!sendAll(sock, &type, sizeof(type)) ||
        !sendAll(sock, &size, sizeof(size)) ||
        !sendAll(sock, &menu, sizeof(MenuMessage))) {
        QMessageBox::critical(nullptr, "Erreur", "Échec de l'envoi du menu.");
        return;
    }    
}

void guiControlleur::spectateRoom(int roomID) {
    int type   = MessageType::T_JoinAsSpectator;
    int size   = sizeof(int);
    sendAll(sock, &type,   sizeof(type));
    sendAll(sock, &size,   sizeof(size));
    sendAll(sock, &roomID, sizeof(roomID));
    threadData->isSpectator = true;
}
