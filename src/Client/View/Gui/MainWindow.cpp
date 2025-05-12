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
#include "FriendsHandler.hpp"
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QScreen>
#include <QMetaObject>
#include <atomic>
#include <QInputDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "Database.hpp" 

extern std::atomic<bool> flag;



MainWindow::MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData)
: QMainWindow(parent), sock(sock), threadData(threadData){

    setupUI();
    resize(600, 500);
    move(QGuiApplication::primaryScreen()->geometry().center() - frameGeometry().center());

    controller = new GuiController(sock, threadData);
    threadData->qtParent = this;
    
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
    centralWidget->setStyleSheet("background-color:rgb(35, 34, 43);"); // violet doux
    auto layout = new QVBoxLayout(centralWidget);

    stackedWidget = new QStackedWidget(this);

    // Configuration des écrans
    setupLoginScreen();
    setupAuthScreen();
    setupMainMenuScreen();
    setupGameModeScreen();
    setupGameScreens();
    setupFriendsScreen();
    setupFriendsListScreen();

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
    loginScreen->setStyleSheet("background-color:rgb(35, 34, 43);"); // fond violet
    auto layout = new QVBoxLayout(loginScreen);

    QLabel *titleLabel = new QLabel("TETRIS ROYALE !", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;"); // mettre en gras
    titleLabel->setAlignment(Qt::AlignCenter);  //  Centrer
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);  // centrer dans layout

    QPushButton *loginButton = new QPushButton("Login", this);
    loginButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *registerButton = new QPushButton("Register", this);
    registerButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert

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
    authScreen->setStyleSheet("background-color:rgb(35, 34, 43);"); // fond violet
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
    submitButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *backButton = new QPushButton("Retour", this);
    backButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert
    layout->addWidget(submitButton);
    layout->addWidget(backButton);

    connect(submitButton, &QPushButton::clicked, this, &MainWindow::handleAuth);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showLoginScreen);


    authScreen->setLayout(layout);
}

void MainWindow::handleAuth() {
    QString username_ = usernameInput->text();
    QString password = passwordInput->text();
    QString confirmPassword = confirmPasswordInput ? confirmPasswordInput->text() : "";

    if (username_.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }


    if(controller->handleAuth(username_, password, confirmPassword, isRegisterMode)){
    username = username_.toStdString();
    //Start periodic UI refresh from threadData
    auto timer = new QTimer(nullptr);
    connect(timer, &QTimer::timeout, this, [this]() {
        std::lock_guard<std::mutex> lock(threadData->mux);
        if (threadData->roomInfoUpdated) {
            threadData->roomInfoUpdated = false;
            setupMyRoomScreen(threadData->roomInfo);
        }
        if (threadData->userInGame) {
            renderQt->renderGames(threadData->playersData);
            stackedWidget->setCurrentWidget(gameScreen);
        }
    });
    timer->start(100);
    
    // passwordInput->clear();
    // confirmPasswordInput->clear();
    showMainMenuScreen();
}
}


void MainWindow::showLoginScreen() {
    stackedWidget->setCurrentWidget(loginScreen);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (threadData && threadData->isOwner) {
        int roomDeletedType = MessageType::T_RoomDeleted;
        int size_ = sizeof(int);
        sendAll(sock, &roomDeletedType, sizeof(int));
        sendAll(sock, &size_, sizeof(int));
    }

    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    sendAll(sock, &exitType, sizeof(int));
    sendAll(sock, &size_, sizeof(int));
    event->accept();
}

void MainWindow::showAuthScreen() {
    if (authScreen) {
        stackedWidget->removeWidget(authScreen);
        authScreen->deleteLater(); // évite les fuites mémoire
        authScreen = nullptr;
    }

    // stackedWidget->setCurrentWidget(authScreen);

    // if (usernameInput) usernameInput->clear();
    // if (passwordInput) passwordInput->clear();
    // if (confirmPasswordInput) confirmPasswordInput->clear();


    setupAuthScreen();
    stackedWidget->addWidget(authScreen);
    stackedWidget->setCurrentWidget(authScreen);
}

void MainWindow::setupMainMenuScreen() {
    mainMenuScreen = new QWidget(this);
    mainMenuScreen->setStyleSheet("background-color:rgb(35, 34, 43);"); // fond violet
    auto layout = new QVBoxLayout(mainMenuScreen);

    QLabel *titleLabel = new QLabel("Main MenuMessage", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;"); 
    titleLabel->setAlignment(Qt::AlignCenter);  
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);  
    

    QPushButton *playButton = new QPushButton("Play", this);
    playButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *friendsButton = new QPushButton("Handle Friends", this);
    friendsButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *leaderboardButton = new QPushButton("Leaderboard (Endless Mode)", this);
    leaderboardButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;"); // bouton vert

    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);

    layout->addWidget(playButton);
    layout->addWidget(friendsButton);
    layout->addWidget(leaderboardButton);

    connect(playButton, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);
    connect(friendsButton, &QPushButton::clicked, this, &MainWindow::handleFriendsMenu);
    connect(leaderboardButton, &QPushButton::clicked, this, &MainWindow::showLeaderboardScreen);



    mainMenuScreen->setLayout(layout);
    stackedWidget->addWidget(mainMenuScreen);
}

void MainWindow::setupGameModeScreen() {
    gameModeScreen = new QWidget(this);
    gameModeScreen->setStyleSheet("background-color:rgb(35, 34, 43);");
    auto layout = new QVBoxLayout(gameModeScreen);

    QLabel *titleLabel = new QLabel("Select a Game Mode:", this);
    layout->addWidget(titleLabel);

    QPushButton *endlessButton = new QPushButton("Endless Mode", this);
    QPushButton *classicButton = new QPushButton("Classic Mode", this);
    QPushButton *duelButton = new QPushButton("Duel Mode", this);
    QPushButton *royalButton = new QPushButton("Royal Competition Mode", this);
    QPushButton *backButton = new QPushButton("Retour", this);

    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

    QList<QPushButton*> buttons = {endlessButton, classicButton, duelButton, royalButton, backButton};
    for (auto* btn : buttons) {
        btn->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;");
        layout->addWidget(btn);
    }



	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);

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


/**
 * @brief Va configurer écran de gestion d amis.
 * 
 * Ecran créé un bouton de retour au main menu
 * Et ecran ajouté stackwidget pour pouvoir bien naviguer entre ecrans
 * 
 */
void MainWindow::setupFriendsScreen(){
    friendsScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(friendsScreen);

    QPushButton* backButton = new QPushButton("Back to MenuMessage", this);
    backButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 8px;");
    
    layout->addWidget(backButton);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showMainMenuScreen);   // connecte bouton "back to menu" pour pouvoir retourner en arriere, au menu
    

    friendsScreen->setLayout(layout);
    stackedWidget->addWidget(friendsScreen);
}

void MainWindow::setupFriendsListScreen(){
    friendsListScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(friendsListScreen);

    friendsListScreen->setLayout(layout);
    stackedWidget->addWidget(friendsListScreen);
}


void MainWindow::setGameScreen() {
    stackedWidget->setCurrentWidget(gameScreen);
}

void MainWindow::setupGameScreens() {
    gameScreen = new QWidget(this);
    auto mainLayout = new QVBoxLayout(gameScreen);

    QWidget* centerContainer = new QWidget(this);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerContainer);

    renderQt = new GridQt(gameScreen);
    renderQt->setThreadData(threadData);
    centerLayout->addWidget(renderQt);

    malusBonusLayout = new QHBoxLayout();
    centerLayout->addLayout(malusBonusLayout);

    mainLayout->addWidget(centerContainer);
    gameScreen->setLayout(mainLayout);
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
        controller->sendMenuAction(MenuActions::A_Play, Modes::Endless_);
    } else if (mode == "classic") {
        controller->sendMenuAction(MenuActions::A_JoinRoom, Modes::Classic_);
    } else if (mode == "duel") {
        controller->sendMenuAction(MenuActions::A_JoinRoom, Modes::Duel_);
    } else if (mode == "royal") {
        controller->sendMenuAction(MenuActions::A_JoinRoom, Modes::Royal_);
    }
}


void MainWindow::showMainMenuScreen() {
    stackedWidget->setCurrentWidget(mainMenuScreen);
}

void MainWindow::showGameModeScreen() {
    stackedWidget->setCurrentWidget(gameModeScreen);
}

//<<<<<<< HEAD
void MainWindow::spectateRoom(int roomID) {
    int type   = MessageType::T_JoinAsSpectator;
    int size   = sizeof(int);
    sendAll(sock, &type,   sizeof(type));
    sendAll(sock, &size,   sizeof(size));
    sendAll(sock, &roomID, sizeof(roomID));

    {
        std::lock_guard<std::mutex> lk(threadData->mux);
        threadData->isSpectator = true;
    }
    
    setupSpectatorWaitingScreen(roomID);
}


//=======
//>>>>>>> feature/gui-clean
void MainWindow::setupSpectatorWaitingScreen(int roomID) {
    std::cout << "thread data in specDis" << threadData->isSpectator << endl;
    if (spectatorWaitingScreen) {
        delete spectatorWaitingScreen;
        stackedWidget->removeWidget(spectatorWaitingScreen);
        spectatorWaitingScreen = nullptr;
    }

    spectatorWaitingScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(spectatorWaitingScreen);

    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);

    // Room ID Label
    QLabel* roomLabel = new QLabel(QString("Spectating Room %1").arg(roomID), this);
    layout->addWidget(roomLabel, 0, Qt::AlignCenter);

    QPushButton* refreshBtn = new QPushButton("Refresh");
    QPushButton* leaveBtn = new QPushButton("Leave Spectation", this);

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        controller->requestRoomUpdate(); // define this
    });
    connect(leaveBtn, &QPushButton::clicked, this, [this]() {
        controller->sendLeaveRoom();
        if (myRoomScreen) {
            stackedWidget->removeWidget(myRoomScreen);
            delete myRoomScreen;
            myRoomScreen = nullptr;
        }

    });
    
    layout->addWidget(refreshBtn);
    layout->addWidget(leaveBtn);

    stackedWidget->addWidget(spectatorWaitingScreen);
    stackedWidget->setCurrentWidget(spectatorWaitingScreen);
}

void MainWindow::setupRoomListScreen(const std::vector<RoomInfo>& roomsInfo) {
    std::cout << "thread data in listscreen" << threadData->isSpectator << endl;
    if (roomListScreen) delete roomListScreen;

    roomListScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(roomListScreen);
    QLabel* title = new QLabel("Available Rooms", this);
    layout->addWidget(title);

    if (roomsInfo.empty()) {
        layout->addWidget(new QLabel("No rooms available."));
    } else {
        for (const auto& r : roomsInfo) {
            auto* row = new QWidget(this);
            auto* rowLayout = new QHBoxLayout(row);

            // 1) Room label
            QString text = QString("Room %1 | Players: %2 | Started: %3")
                               .arg(r.roomID)
                               .arg(r.currentPlayersCount)
                               .arg(r.isStarted ? "YES" : "NO");
            auto* roomLabel = new QLabel(text, this);
            rowLayout->addWidget(roomLabel);

            // 2) Join button
            auto* joinBtn = new QPushButton("Join", this);
            rowLayout->addWidget(joinBtn);
            connect(joinBtn, &QPushButton::clicked, [this, r]() {
                controller->joinRoom(r.roomID);
            });

            // 3) Spectate button
            auto* specBtn = new QPushButton("Spectate", this);
            rowLayout->addWidget(specBtn);
            connect(specBtn, &QPushButton::clicked, [this, r]() {
                controller->spectateRoom(r.roomID);
            });
            rowLayout->addStretch();

            layout->addWidget(row);
        }
    }
    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);
    QPushButton* createBtn = new QPushButton("Create Room");
    QPushButton* refreshBtn = new QPushButton("Refresh");
    QPushButton* backBtn = new QPushButton("Back");
    layout->addWidget(createBtn);
    layout->addWidget(refreshBtn);
    layout->addWidget(backBtn);

    // Handle create
    connect(createBtn, &QPushButton::clicked, this, [this]() {
        controller->sendCreateRoom();
    });

    // Keep existing
    // connect(refreshBtn, &QPushButton::clicked, controller, &GuiController::requestRoomList);
    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        controller->requestRoomList();
    });
    
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        controller->requestRoomList();  // define this to re-fetch rooms from server
    });

    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    stackedWidget->addWidget(roomListScreen);
    stackedWidget->setCurrentWidget(roomListScreen);
}


void MainWindow::setupMyRoomScreen(const RoomInfo& room) {
    if (myRoomScreen) {
        stackedWidget->removeWidget(myRoomScreen);
        delete myRoomScreen;
        myRoomScreen = nullptr;
    }
    std::cout << "thread data in roomscreen" << threadData->isSpectator << endl;
    

    myRoomScreen = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(myRoomScreen);

    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);

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
        controller->sendForceStart(); // define this
    });

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        controller->requestRoomUpdate(); // define this
    });

    connect(leaveBtn, &QPushButton::clicked, this, [this]() {
        controller->sendLeaveRoom();
        if (myRoomScreen) {
            stackedWidget->removeWidget(myRoomScreen);
            delete myRoomScreen;
            myRoomScreen = nullptr;
        }

    });

    stackedWidget->addWidget(myRoomScreen);
    stackedWidget->setCurrentWidget(myRoomScreen);
}

//<<<<<<< HEAD
void MainWindow::joinRoom(int roomID) {
    int type = MessageType::T_Join;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &roomID, sizeof(int));
}

void MainWindow::sendForceStart() {
    MenuMessage menu;
    menu.action = MenuActions::A_ForceStart;
    // menu.mode = threadData->gameMode;
    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(MenuMessage));
}

void MainWindow::sendLeaveRoom() {
    int type;
    int size = sizeof(int);

    if (threadData->isOwner) {
        type = MessageType::T_RoomDeleted;
    } else {
        type = MessageType::T_LeftRoom;
    }

    if (!sendAll(sock, &type, sizeof(int)) ||
        !sendAll(sock, &size, sizeof(int))) {
        QMessageBox::critical(this, "Error", "Failed to leave room.");
        return;
    }

    // Remove room screen UI
    if (myRoomScreen) {
        stackedWidget->removeWidget(myRoomScreen);
        delete myRoomScreen;
        myRoomScreen = nullptr;
    }

    threadData->mux.lock();
    threadData->userInLobby = false;
    threadData->roomInfoUpdated = false;
    threadData->isOwner = false;
    threadData->mux.unlock();

    requestRoomList();
}


void MainWindow::requestRoomUpdate() {
    int type = MessageType::T_Refresh;
    int size = sizeof(int);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
}

void MainWindow::requestRoomList() {
    MenuMessage menu;
    menu.mode = threadData->gameMode;
    menu.action = MenuActions::A_JoinRoom;

    int type = MessageType::T_Menu;
    int size = sizeof(MenuMessage);
    sendAll(sock, &type, sizeof(int));
    sendAll(sock, &size, sizeof(int));
    sendAll(sock, &menu, sizeof(MenuMessage));
}

/*void MainWindow::handleAuth() {
    QString username_qt = usernameInput->text();   // ++++++++++++++++++++++++++++++++++++++++++++++
    QString password = passwordInput->text();

    if (username_qt.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    Connexion connexion;
    std::strncpy(connexion.username, username_qt.toStdString().c_str(), sizeof(connexion.username) - 1); // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

    username = username_qt.toStdString();
    userId_ = udm.getUserId(username.c_str());




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
    // QMessageBox::information(this, "Succès", isRegisterMode ? "Inscription et connexion réussies !" : "Connexion réussie !");
    showMainMenuScreen();
}*/

/*void MainWindow::closeEvent(QCloseEvent *event) {
    if (threadData && threadData->isOwner) {
        int roomDeletedType = MessageType::T_RoomDeleted;
        int size_ = sizeof(int);
        sendAll(sock, &roomDeletedType, sizeof(int));
        sendAll(sock, &size_, sizeof(int));
    }

    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(sock, &exitType, sizeof(int));
    int bytesSentSize = sendAll(sock, &size_, sizeof(int));
    event->accept();
}*/

void* MainWindow::threadRoutineQtView(void* arg) {
    Player::ThreadData* threadData = static_cast<Player::ThreadData*>(arg);
    int sock = threadData->socket;
    auto* parent = static_cast<MainWindow*>(threadData->qtParent);

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
            if(!parent->chatHandler || !parent->chatHandler->getChatHistory())break;
            QString sender = QString::fromUtf8(message.expediteur);
            QString msg = QString::fromUtf8(message.message);
            QMetaObject::invokeMethod(parent->chatHandler->getChatHistory(), [=]() {
            parent->chatHandler->getChatHistory()->append("<b>" + sender + ": </b> " + msg);
            }, Qt::QueuedConnection);
        }

        else if (type == MessageType::T_Exit || type == MessageType::T_ForcedExit) {
        //    if (type == MessageType::T_ForcedExit) {
        //        threadData->mux.lock();
        //        threadData->userInGame = false;
        //        threadData->userInLobby = false;
        //        threadData->mux.unlock();
        //        flag = true;
        //        break;
        //    } else {
        //        // ibrahim irhamni
        //        threadData->mux.lock();
        //        threadData->userInGame = false;
        //        threadData->userInLobby = false;
        //        threadData->mux.unlock();
        //        continue;
        //    }
            threadData->mux.lock();
            threadData->userInGame = false;
            threadData->userInLobby = false;
            threadData->isSpectator = false;
            threadData->mux.unlock();

            QMetaObject::invokeMethod(
                static_cast<QObject*>(parent),
                [parent, this]() {
                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(parent, "Game Over",
                            "Game has ended. Return to the main menu?",
                            QMessageBox::Yes | QMessageBox::No);
                    if (reply == QMessageBox::Yes) {
                        controller->sendLeaveRoom();
                        parent->showMainMenuScreen();
                    }
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
                    threadData->clinetPlayerID = static_cast<int>(i);
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

            QMetaObject::invokeMethod(
                static_cast<QObject*>(parent),
                [parent]() {
                    parent->addMalusButtonsIfRoyal();
                    parent->setGameScreen();
                }
            );
            
        }

        if (type == MessageType::T_RoomInfo) {
            // this is the *host*’s view
            RoomInfo info;
            recvAll(sock, &info, size);
            QMetaObject::invokeMethod(parent, [=]() {
              parent->setupMyRoomScreen(info);
            });
        }
        else if (type == MessageType::T_RoomJoinInfo) {
            // this is when *anyone* joins (player or spectator)
            RoomInfo info;
            recvAll(sock, &info, size);
        
            // safely grab the spectator flag
            bool spect = false;
            {
              std::lock_guard<std::mutex> lk(threadData->mux);
              spect = threadData->isSpectator;
            }
        
            QMetaObject::invokeMethod(parent, [=]() {
              if (spect)
                parent->setupSpectatorWaitingScreen(info.roomID);
              else
                parent->setupMyRoomScreen(info);
            });
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

//=======
//>>>>>>> feature/gui-clean
void MainWindow::addMalusButtonsIfRoyal() {
    if (threadData && threadData->gameMode == Modes::Royal_ && gameScreen && malusBonusLayout) {
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
                controller->handleMalusBonus(i + 1);
            });
        }
    }
}


void MainWindow::showGameOverPopup() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Game Over", 
            "Game has ended. Return to the main menu?",
            QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        controller->sendLeaveRoom();
        showMainMenuScreen();
    }
}

/**
 * @brief Gère l'affichage et la mise à jour du menu d'amis.
 * 
 * Cette méthode va nettoyer l'ancien FriendsHandler s'il existe,
 * Récupère le nombre de demandes d'amis en attente
 * Crée un nouveau FriendsHandler avec les données actuelles
 * Gère le layout de l'écran des amis - nouveau layout créé si besoin
 * Place l'écran des amis dans le stackedWidget
 * 
 * @note Réinitialise complètement l'interface des amis à chaque appel
 * 
 */
void MainWindow::handleFriendsMenu() {
    // Supprimera l'ancien friendsHandler s'il existe
    if (friendsHandler) {
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(friendsScreen->layout());
        if (layout){
            layout->removeWidget(friendsHandler);
        }
        friendsHandler->deleteLater();
        friendsHandler = nullptr;
    }

    std::vector<std::string> friends = udm.getFriendsList(username);
    std::vector<QString> qtFriends;

    for(const auto& f: friends) {
        qtFriends.push_back(QString::fromStdString(f));
    }
    
    int requestCount = static_cast<int>(udm.getReceivedRequestsList(username).size());  // compte nombre de demandes d ami recues
    
    // Créer le nouveau handler
    friendsHandler = new FriendsHandler(friendsScreen, QString::fromStdString(username), qtFriends, requestCount);
    // pour recuperer le layout deja existant
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(friendsScreen->layout());

    if (!layout) {   // si aucun existe, creer un nouveau
        layout = new QVBoxLayout(friendsScreen);
        friendsScreen->setLayout(layout);
    }
    // Ajouter le frieindshandler
    layout->insertWidget(0, friendsHandler);
    stackedWidget->setCurrentWidget(friendsScreen);
}

void MainWindow::handleChatMenu() {
    // Supprimera l'ancien friendsHandler s'il existe
    if (chatHandler) {
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(friendsListScreen->layout());
        if (layout){
            layout->removeWidget(chatHandler);
        }
        chatHandler->deleteLater();
        chatHandler = nullptr;
    }
    std::vector<std::string> friends = udm.getFriendsList(username);
    std::vector<QString> qtFriends;

    for(const auto& f: friends) {
        qtFriends.push_back(QString::fromStdString(f));
    }

    //int requestCount = static_cast<int>(udm.requestReceivedList(username).size());  // compte nombre de demandes d ami recues
    // Créer le nouveau handler
    QWidget* currentWidget = stackedWidget->currentWidget();
    chatHandler = new ChatHandler(sock, this, friendsListScreen, QString::fromStdString(username), qtFriends, currentWidget);
    // pour recuperer le layout deja existant
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(friendsListScreen->layout());

    if (!layout) {   // si aucun existe, creer un nouveau
        layout = new QVBoxLayout(friendsListScreen);
        friendsListScreen->setLayout(layout);
    }
    // Ajouter le frieindshandler
    layout->insertWidget(0, chatHandler);
    stackedWidget->setCurrentWidget(friendsListScreen);
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
    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new"));

	chatButton->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));
    chatButton->setFixedSize(80, 30);

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, this, &MainWindow::handleChatMenu);
    QPushButton* backBtn = new QPushButton("Back", this);
    backBtn->setStyleSheet("background-color:rgb(13, 154, 201); color: black; font-weight: bold;");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showMainMenuScreen);
    layout->addWidget(backBtn);

    leaderboardScreen->setLayout(layout);
    stackedWidget->addWidget(leaderboardScreen);
    stackedWidget->setCurrentWidget(leaderboardScreen);
}

void MainWindow::showfriendsListScreen() {
    stackedWidget->setCurrentWidget(friendsListScreen);
}

void MainWindow::showChatScreen() {
    stackedWidget->setCurrentWidget(chatScreen);
}

QStackedWidget* MainWindow::getStackedWidget(){
  return stackedWidget;
}