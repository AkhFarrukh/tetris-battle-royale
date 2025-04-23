#ifdef border
#undef border
#endif

#ifdef scroll
#undef scroll
#endif

#ifdef timeout
#undef timeout
#endif

// MainWindow.cpp
#include "MainWindow.hpp"
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QScreen>
#include <QMetaObject>
#include <QDebug>
#include <atomic>
#include <QInputDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

extern std::atomic<bool> flag;


MainWindow::MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData)
    : QMainWindow(parent), sock(sock), threadData(threadData) {

    setupUI();
    resize(600, 500);
    move(QGuiApplication::primaryScreen()->geometry().center() - frameGeometry().center());


    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGameScreen);
    timer->start(100);

}


MainWindow::~MainWindow() {
    flag = true;
    shutdown(sock, SHUT_RD);
    if (threadID) pthread_join(threadID, nullptr);
}

void MainWindow::setupUI() {

    // Création du widget central avec QStackedWidget
    auto centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #a066cc;"); // violet doux
    auto layout = new QVBoxLayout(centralWidget);

    stackedWidget = new QStackedWidget(this);

    // Configuration des écrans
    setupLoginScreen();
    setupAuthScreen();
    setupMainMenuScreen();
    setupGameModeScreen();
    setupGameScreens();

    // Ajouter les pages au QStackedWidget
    stackedWidget->addWidget(loginScreen);
    stackedWidget->addWidget(authScreen);
    stackedWidget->addWidget(mainMenuScreen); // facultatif si tu le fais déjà dans setup
    stackedWidget->addWidget(gameModeScreen); // idem
    stackedWidget->addWidget(gameScreen);

    layout->addWidget(stackedWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Afficher l'écran de connexion au départ
    stackedWidget->setCurrentWidget(loginScreen);

}

void MainWindow::setupLoginScreen() {
    loginScreen = new QWidget(this);
    loginScreen->setStyleSheet("background-color: #a066cc;"); // fond violet
    auto layout = new QVBoxLayout(loginScreen);

    QLabel *titleLabel = new QLabel("Welcome !", this);
    layout->addWidget(titleLabel);

    QPushButton *loginButton = new QPushButton("Login", this);
    loginButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *registerButton = new QPushButton("Register", this);
    registerButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert

    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    connect(loginButton, &QPushButton::clicked, [this]() {
        isRegisterMode = false;
        showAuthScreen();
    });

    connect(registerButton, &QPushButton::clicked, [this]() {
        isRegisterMode = true;
        showAuthScreen();
    });

    loginScreen->setLayout(layout);
}

void MainWindow::setupAuthScreen() {
    authScreen = new QWidget(this);
    authScreen->setStyleSheet("background-color: #a066cc;"); // fond violet
    auto layout = new QVBoxLayout(authScreen);

    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameInput = new QLineEdit(this);
    QLabel *passwordLabel = new QLabel("Password:", this);
    passwordInput = new QLineEdit(this);
    passwordInput->setEchoMode(QLineEdit::Password);

    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);


    if (isRegisterMode) {
        QLabel *confirmLabel = new QLabel("Confirm password:", this);
        confirmPasswordInput = new QLineEdit(this);
        confirmPasswordInput->setEchoMode(QLineEdit::Password);
        layout->addWidget(confirmLabel);
        layout->addWidget(confirmPasswordInput);
    } else {
        confirmPasswordInput = nullptr; // On efface s’il venait d’un Register
    }

    QPushButton *submitButton = new QPushButton(isRegisterMode ? "Register" : "Login", this);
    submitButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *backButton = new QPushButton("Retour", this);
    backButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    layout->addWidget(submitButton);
    layout->addWidget(backButton);

    connect(submitButton, &QPushButton::clicked, this, &MainWindow::handleAuth);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showLoginScreen);


    authScreen->setLayout(layout);
}

void MainWindow::showLoginScreen() {
    stackedWidget->setCurrentWidget(loginScreen);
}

void MainWindow::showAuthScreen() {
    if (authScreen) {
        stackedWidget->removeWidget(authScreen);
        authScreen->deleteLater(); // évite les fuites mémoire
        authScreen = nullptr;
    }

    setupAuthScreen();
    stackedWidget->addWidget(authScreen);
    stackedWidget->setCurrentWidget(authScreen);
}

void MainWindow::setupMainMenuScreen() {
    mainMenuScreen = new QWidget(this);
    mainMenuScreen->setStyleSheet("background-color: #a066cc;"); // fond violet
    auto layout = new QVBoxLayout(mainMenuScreen);

    QLabel *titleLabel = new QLabel("Main Menu", this);
    layout->addWidget(titleLabel);

    QPushButton *playButton = new QPushButton("Play", this);
    playButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *friendsButton = new QPushButton("Handle Friends", this);
    friendsButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *leaderboardButton = new QPushButton("Leaderboard (Endless Mode)", this);
    leaderboardButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert

    layout->addWidget(playButton);
    layout->addWidget(friendsButton);
    layout->addWidget(leaderboardButton);

    connect(playButton, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);
    // connect(friendsButton, &QPushButton::clicked, this, nullptr);
    connect(leaderboardButton, &QPushButton::clicked, this, &MainWindow::showLeaderboardScreen);



    mainMenuScreen->setLayout(layout);
    stackedWidget->addWidget(mainMenuScreen); // Important !
}

void MainWindow::setupGameModeScreen() {
    gameModeScreen = new QWidget(this);
    gameModeScreen->setStyleSheet("background-color: #a066cc;");
    auto layout = new QVBoxLayout(gameModeScreen);

    QLabel *titleLabel = new QLabel("Select a Game Mode:", this);
    layout->addWidget(titleLabel);

    QPushButton *endlessButton = new QPushButton("Endless Mode", this);
    QPushButton *classicButton = new QPushButton("Classic Mode", this);
    QPushButton *duelButton = new QPushButton("Duel Mode", this);
    QPushButton *royalButton = new QPushButton("Royal Competition Mode", this);
    QPushButton *backButton = new QPushButton("Retour", this);

    QList<QPushButton*> buttons = {endlessButton, classicButton, duelButton, royalButton, backButton};
    for (auto* btn : buttons) {
        btn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
        layout->addWidget(btn);
    }

    connect(endlessButton, &QPushButton::clicked, this, [this]() {
        showModeScreen("endless");
    });

    connect(classicButton, &QPushButton::clicked, this, [this]() {
        showModeScreen("classic");
    });

    connect(duelButton, &QPushButton::clicked, this, [this]() {
        showModeScreen("duel");
    });

    connect(royalButton, &QPushButton::clicked, this, [this]() {
        showModeScreen("royal");
    });

    connect(backButton, &QPushButton::clicked, this, [this]() {
        showMainMenuScreen();
    });

    gameModeScreen->setLayout(layout);
    stackedWidget->addWidget(gameModeScreen);
}

void MainWindow::sendMenuAction(MenuActions action, Modes mode) {
    Menu menu;
    menu.action = action;
    menu.mode = mode;
    threadData->mux.lock();
    threadData->socket = sock;
    threadData->gameMode = mode;
    threadData->isGui = true;
    threadData->qtParent = this;
    threadData->userInGame = false;   // <-- will be set to true by threadRoutineView
    threadData->userInLobby = true;
    threadData->mux.unlock();


    if (!threadID) {
        pthread_create(&threadID, nullptr, MainWindow::threadRoutineQtView, threadData.get());
    }
    
    int type = MessageType::T_Menu;
    int size = sizeof(Menu);
    
    if (!sendAll(sock, &type, sizeof(type)) ||
        !sendAll(sock, &size, sizeof(size)) ||
        !sendAll(sock, &menu, sizeof(Menu))) {
        QMessageBox::critical(this, "Erreur", "Échec de l'envoi du menu.");
        return;
    }    
}

void MainWindow::setGameScreen() {
    stackedWidget->setCurrentWidget(gameScreen);
}

void MainWindow::setupGameScreens() {
    gameScreen = new QWidget(this);
    auto layout = new QVBoxLayout(gameScreen);

    renderQt = new GridQt(gameScreen);
    renderQt->setThreadData(threadData);
    layout->addWidget(renderQt);

    gameScreen->setLayout(layout);
    stackedWidget->addWidget(gameScreen);
}

void MainWindow::updateGameScreen() {
    if (!renderQt || !threadData) return;
    std::lock_guard<std::mutex> lock(threadData->mux);
    if (!threadData->userInGame) return;
    renderQt->renderGames(threadData->playersData);
    stackedWidget->setCurrentWidget(gameScreen);
}


void MainWindow::showModeScreen(const QString& mode) {
    if (mode == "endless") {
        sendMenuAction(MenuActions::A_Play, Modes::Endless_);
    } else if (mode == "classic") {
        sendMenuAction(MenuActions::A_JoinRoom, Modes::Classic_);
    } else if (mode == "duel") {
        sendMenuAction(MenuActions::A_JoinRoom, Modes::Duel_);
    } else if (mode == "royal") {
        sendMenuAction(MenuActions::A_JoinRoom, Modes::Royal_);
    } else {
        qDebug() << "Mode inconnu: " << mode;
    }
}


void MainWindow::showMainMenuScreen() {
    stackedWidget->setCurrentWidget(mainMenuScreen);
}

void MainWindow::showGameModeScreen() {
    stackedWidget->setCurrentWidget(gameModeScreen);
}

void MainWindow::setupRoomListScreen(const std::vector<RoomInfo>& roomsInfo) {
    if (roomListScreen) delete roomListScreen;

    roomListScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(roomListScreen);
    QLabel* title = new QLabel("Available Rooms", this);
    layout->addWidget(title);

    if (roomsInfo.empty()) {
        layout->addWidget(new QLabel("No rooms available."));
    } else {
        for (const auto& r : roomsInfo) {
            QString text = QString("RoomID: %1 | Players: %2 | Started: %3")
                .arg(r.roomID)
                .arg(r.currentPlayersCount)
                .arg(r.isStarted ? "YES" : "NO");
            QPushButton* joinBtn = new QPushButton(text, this);

            connect(joinBtn, &QPushButton::clicked, [this, r]() {
                joinRoom(r.roomID);
            });

            layout->addWidget(joinBtn);
        }
    }

    QPushButton* createBtn = new QPushButton("Create Room");
    QPushButton* refreshBtn = new QPushButton("Refresh");
    QPushButton* backBtn = new QPushButton("Back");
    layout->addWidget(createBtn);
    layout->addWidget(refreshBtn);
    layout->addWidget(backBtn);

    // Handle create
    connect(createBtn, &QPushButton::clicked, this, [this]() {
        sendCreateRoom();
    });

    // Keep existing
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::requestRoomList);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        requestRoomList();  // define this to re-fetch rooms from server
    });

    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    stackedWidget->addWidget(roomListScreen);
    stackedWidget->setCurrentWidget(roomListScreen);
}

void MainWindow::sendCreateRoom() {
    Menu menu;
    menu.mode = threadData->gameMode;
    menu.action = MenuActions::A_CreateRoom;

    int type = MessageType::T_Menu;
    int size = sizeof(Menu);

    if (!sendAll(sock, &type, sizeof(type)) ||
        !sendAll(sock, &size, sizeof(size)) ||
        !sendAll(sock, &menu, sizeof(Menu))) {
        QMessageBox::critical(this, "Erreur", "Échec de la création de la salle.");
        return;
    }

    qDebug() << "Create room request sent";
}

void MainWindow::setupMyRoomScreen(const RoomInfo& room, bool isOwner) {
    if (myRoomScreen) delete myRoomScreen;

    myRoomScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(myRoomScreen);

    layout->addWidget(new QLabel(QString("Room ID: %1").arg(room.roomID)));
    layout->addWidget(new QLabel(QString("Players: %1").arg(room.currentPlayersCount)));
    layout->addWidget(new QLabel(QString("Started: %1").arg(room.isStarted ? "YES" : "NO")));

    QPushButton* forceStartBtn = new QPushButton("Force Start");
    QPushButton* refreshBtn = new QPushButton("Refresh");
    QPushButton* leaveBtn = new QPushButton("Leave Room");

    layout->addWidget(forceStartBtn);
    layout->addWidget(refreshBtn);
    layout->addWidget(leaveBtn);

    connect(forceStartBtn, &QPushButton::clicked, this, [this]() {
        sendForceStart(); // define this
    });

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        requestRoomUpdate(); // define this
    });

    connect(leaveBtn, &QPushButton::clicked, this, [this]() {
        sendLeaveRoom(); // define this
    });

    stackedWidget->addWidget(myRoomScreen);
    stackedWidget->setCurrentWidget(myRoomScreen);
}

void MainWindow::joinRoom(int roomID) {
    int type = MessageType::T_Join;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &roomID, sizeof(int));
}

void MainWindow::sendForceStart() {
    Menu menu;
    menu.action = MenuActions::A_ForceStart;
    // menu.mode = threadData->gameMode;
    int type = MessageType::T_Menu;
    int size = sizeof(Menu);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(Menu));
}

void MainWindow::sendLeaveRoom() {
    int type = MessageType::T_LeftRoom;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
}

void MainWindow::requestRoomUpdate() {
    int type = MessageType::T_Refresh;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
}

void MainWindow::requestRoomList() {
    Menu menu;
    menu.mode = threadData->gameMode;
    menu.action = MenuActions::A_JoinRoom;

    int type = MessageType::T_Menu;
    int size = sizeof(Menu);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(Menu));
}

void MainWindow::handleAuth() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    Connexion connexion;
    std::strncpy(connexion.username, username.toStdString().c_str(), sizeof(connexion.username) - 1);
    std::strncpy(connexion.password, password.toStdString().c_str(), sizeof(connexion.password) - 1);

    int size = sizeof(struct Connexion);

    if (isRegisterMode) {
        // Vérifie la confirmation du mot de passe
        QString confirmPassword = confirmPasswordInput->text();
        if (password != confirmPassword) {
            QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
            passwordInput->clear();
            confirmPasswordInput->clear();
            return;
        }

        // Envoi d'une demande d'inscription
        int type = MessageType::T_Register;
        sendAll(sock, &type, sizeof(int));
        sendAll(sock, &size, sizeof(int));
        sendAll(sock, &connexion, sizeof(Connexion));

        int typeResponse, sizeResponse;
        if (!recvAll(sock, &typeResponse, sizeof(int)) || !recvAll(sock, &sizeResponse, sizeof(int))) {
            QMessageBox::critical(this, "Erreur", "Erreur de communication avec le serveur.");
            return;
        }

        if (typeResponse != MessageType::T_AcceptConnexion) {
            QMessageBox::warning(this, "Erreur", "Échec de l'inscription. Essayez un autre nom.");
            return;
        }

        // L'inscription est acceptée => tentative de connexion
        type = MessageType::T_Connexion;
        sendAll(sock, &type, sizeof(int));
        sendAll(sock, &size, sizeof(int));
        sendAll(sock, &connexion, sizeof(Connexion));

        if (!recvAll(sock, &typeResponse, sizeof(int)) || !recvAll(sock, &sizeResponse, sizeof(int))) {
            QMessageBox::critical(this, "Erreur", "Erreur de connexion après l'inscription.");
            return;
        }

        if (typeResponse != MessageType::T_AcceptConnexion) {
            QMessageBox::critical(this, "Erreur", "Inscription réussie, mais la connexion a échoué.");
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
            QMessageBox::critical(this, "Erreur", "Erreur de communication avec le serveur.");
            return;
        }

        if (typeResponse != MessageType::T_AcceptConnexion) {
            QMessageBox::warning(this, "Erreur", "Nom d'utilisateur ou mot de passe incorrect.");
            return;
        }
    }

    // Vérifie une double session dans tous les cas après connexion
    int doubleSession;
    int size_;
    if (!recvAll(sock, &doubleSession, sizeof(int)) || !recvAll(sock, &size_, sizeof(int))) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la vérification de session.");
        return;
    }

    if (doubleSession == MessageType::T_RefuseConnexion) {
        QMessageBox::warning(this, "Erreur", "Vous êtes déjà connecté sur une autre session.");
        return;
    }

    this->threadData->socket = sock;
    this->threadData->gameMode = Modes::Endless_; //this just a default mode, it will be overwritten later...
    this->threadData->userInGame = false;
    this->threadData->userInLobby = false;
    this->threadData->isGui = true;
    this->threadData->qtParent = this;
    
    pthread_create(&threadID, nullptr, MainWindow::threadRoutineQtView, threadData.get());

    //Start periodic UI refresh from threadData
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        std::lock_guard<std::mutex> lock(threadData->mux);

        // std::cout << "-------------timer----------------" << endl;

        if (threadData->roomInfoUpdated) {
            threadData->roomInfoUpdated = false;
            setupMyRoomScreen(threadData->roomInfo, threadData->isOwner);
        }

        if (threadData->userInGame) {
            renderQt->renderGames(threadData->playersData);
            stackedWidget->setCurrentWidget(gameScreen);
        }
    });
    timer->start(100); // every 100ms


    // Connexion réussie !
    QMessageBox::information(this, "Succès", isRegisterMode ? "Inscription et connexion réussies !" : "Connexion réussie !");
    showMainMenuScreen();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(sock, &exitType, sizeof(int));
    int bytesSentSize = sendAll(sock, &size_, sizeof(int));
    event->accept();
}

void* MainWindow::threadRoutineQtView(void* arg) {
    Player::ThreadData* threadData = static_cast<Player::ThreadData*>(arg);
    int sock = threadData->socket;
    auto* parent = static_cast<MainWindow*>(threadData->qtParent);

    while (threadData->socket == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (!flag) {
        std::cout << "realy ?";
        int type, size;
        if (!recvAll(sock, &type, sizeof(int)) || !recvAll(sock, &size, sizeof(int))) {
            perror("Disconnected or error");
            flag = true;
            break;
        }

        if (type == MessageType::T_Message) {
            Message message;
            if (!recvAll(sock, &message, size)) break;
        }

        else if (type == MessageType::T_Exit || type == MessageType::T_ForcedExit) {
            threadData->mux.lock();
            threadData->userInGame = false;
            threadData->userInLobby = false;
            threadData->mux.unlock();
            flag = true;
            break;
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
        }

        else if (type == MessageType::T_GameStart) {
            threadData->mux.lock();
            threadData->userInGame = true;
            threadData->userInLobby = true;
            threadData->mux.unlock();

            QMetaObject::invokeMethod(
                static_cast<QObject*>(parent),
                [parent]() {
                    parent->addMalusButtonsIfRoyal();
                    parent->setGameScreen();
                }
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

            QMetaObject::invokeMethod(
                static_cast<QObject*>(parent),
                [info, isOwner = (type == MessageType::T_RoomInfo), parent]() {
                    parent->setupMyRoomScreen(info, isOwner);
                }
            );
        }

        else if (type == MessageType::T_RoomsInfo) {
            std::vector<RoomInfo> rooms(size);
            if (!recvAll(sock, rooms.data(), size * sizeof(RoomInfo))) break;

            int modeInt, modeSize;
            if (!recvAll(sock, &modeInt, sizeof(int))) break;
            if (!recvAll(sock, &modeSize, sizeof(int))) break;

            QMetaObject::invokeMethod(
                static_cast<QObject*>(parent),
                [rooms, parent]() {
                    parent->setupRoomListScreen(rooms);
                }
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

void MainWindow::addMalusButtonsIfRoyal() {
    if (threadData && threadData->gameMode == Modes::Royal_ && gameScreen) {
        QHBoxLayout* malusBonusLayout = new QHBoxLayout();

        QStringList labels = {
            "1. Speed Up Enemy",
            "2. Reverse Controls",
            "3. Freeze Controls",
            "4. Blackout",
            "5. Thunder",
            "6. Block 1x1",
            "7. Slow Down"
        };

        for (int i = 0; i < labels.size(); ++i) {
            QPushButton* btn = new QPushButton(labels[i], this);
            btn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold;");
            malusBonusLayout->addWidget(btn);

            connect(btn, &QPushButton::clicked, this, [this, i]() {
                handleMalusBonus(i + 1);
            });
        }

        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(gameScreen->layout());
        if (mainLayout) {
            mainLayout->addLayout(malusBonusLayout);
        }
    }
}


void MainWindow::handleMalusBonus(int malusId) {
    int target = -1;

    // Bonus: auto-target self
    if (malusId == 6 || malusId == 7) {
        for (const auto& p : threadData->playersData) {
            if (p.playerId == -1) {
                target = threadData->clinetPlayerID;
                break;
            }
        }
    } else {
        // Malus: select another target
        QStringList playerOptions;
        for (const auto& p : threadData->playersData) {
            if (p.playerId != -1) {
                playerOptions << QString("Player %1").arg(p.playerId);
            }
        }

        bool ok = false;
        QString chosen = QInputDialog::getItem(this, "Select Target Player",
            "Choose player to apply malus:", playerOptions, 0, false, &ok);

        if (!ok || chosen.isEmpty()) return;

        QRegularExpression re("Player (\\d+)");
        QRegularExpressionMatch match = re.match(chosen);
        if (match.hasMatch()) {
            target = match.captured(1).toInt();
        } else {
            QMessageBox::warning(this, "Error", "Invalid player selection.");
            return;
        }
    }

    if (target == -1) {
        QMessageBox::warning(this, "Error", "Could not determine target player.");
        return;
    }

    // Send malus/bonus to the server
    MalBonus mb;
    mb.malusB = static_cast<MalusBonus>(malusId);
    mb.target = target;

    int type = MessageType::T_MalusBonus;
    int size = sizeof(MalBonus);

    if (!sendAll(sock, &type, sizeof(int)) ||
        !sendAll(sock, &size, sizeof(int)) ||
        !sendAll(sock, &mb, sizeof(MalBonus))) {
        QMessageBox::critical(this, "Error", "Failed to send malus/bonus.");
    }
}


void MainWindow::showLeaderboardScreen() {
    if (leaderboardScreen) {
        delete leaderboardScreen;
        leaderboardScreen = nullptr;
    }

    leaderboardScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(leaderboardScreen);

    QLabel* title = new QLabel("🏆 Leaderboard: Top Endless Mode Players 🏆", this);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: gold;");
    layout->addWidget(title);

    std::vector<std::pair<std::string, int>> scores = udm.getTopScores(5);

    if (scores.empty()) {
        layout->addWidget(new QLabel("No scores recorded yet.", this));
    } else {
        for (size_t i = 0; i < scores.size(); ++i) {
            QString entry = QString("%1. %2 - %3 points")
                                .arg(i + 1)
                                .arg(QString::fromStdString(scores[i].first))
                                .arg(scores[i].second);
            QLabel* scoreLabel = new QLabel(entry, this);
            scoreLabel->setStyleSheet("color: white; font-size: 14px;");
            layout->addWidget(scoreLabel);
        }
    }

    QPushButton* backBtn = new QPushButton("Back", this);
    backBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showMainMenuScreen);
    layout->addWidget(backBtn);

    leaderboardScreen->setLayout(layout);
    stackedWidget->addWidget(leaderboardScreen);
    stackedWidget->setCurrentWidget(leaderboardScreen);
}
