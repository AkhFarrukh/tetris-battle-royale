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


MainWindow::MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData)
    : QMainWindow(parent), sock(sock), threadData(threadData) {

    setupUI();
    resize(600, 500);
    move(QGuiApplication::primaryScreen()->geometry().center() - frameGeometry().center());

    // this->threadData->socket = sock;
    // this->threadData->gameMode = Modes::Endless_;
    // this->threadData->userInGame = false;  // Will be set later
    // this->threadData->userInLobby = false;
    // this->threadData->isGui = true;
    // this->threadData->qtParent = this;
    
    // pthread_create(&threadID, nullptr, Player::threadRoutineView, this->threadData.get());

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
    // stackedWidget->addWidget(endlessScreen);
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
    // 🧠 Setup threadData before waiting for T_GameStart
    threadData->mux.lock();
    threadData->socket = sock;
    threadData->gameMode = mode;
    threadData->isGui = true;
    threadData->qtParent = this;
    threadData->userInGame = false;   // <-- will be set to true by threadRoutineView
    threadData->userInLobby = true;
    threadData->mux.unlock();


    int type = MessageType::T_Menu;
    int size = sizeof(Menu);

    std::cout<< "--------->sendMenuAction called<----------" <<endl;

    if (!sendAll(sock, &type, sizeof(int)) ||
        !sendAll(sock, &size, sizeof(int)) ||
        !sendAll(sock, &menu, sizeof(Menu))) {
        QMessageBox::critical(this, "Erreur", "Échec de l'envoi du menu.");
        return;
    }

    if (mode == Modes::Endless_) {
        int typeResponse, sizeResponse;
        if (!recvAll(sock, &typeResponse, sizeof(int)) ||
            !recvAll(sock, &sizeResponse, sizeof(int))) {
            QMessageBox::critical(this, "Erreur", "Pas de réponse du serveur.");
            return;
        }

        else if (typeResponse == MessageType::T_GameStart) {
            threadData->mux.lock();
            threadData->userInGame = true;
            threadData->userInLobby = true;
            threadData->mux.unlock();
        
            if (threadData->isGui && threadData->qtParent) {
                QMetaObject::invokeMethod(static_cast<QObject*>(threadData->qtParent), [this]() {
                    static_cast<MainWindow*>(threadData->qtParent)->setGameScreen();
                });
            }
        
            // continue;
        }        
    }

    if (action == MenuActions::A_JoinRoom) {
        int typeResponse;
        int nbRooms;
    
        if (!recvAll(sock, &typeResponse, sizeof(int)) ||
            !recvAll(sock, &nbRooms, sizeof(int))) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la réception des rooms.");
            return;
        }
    
        if (typeResponse != MessageType::T_RoomsInfo) {
            QMessageBox::warning(this, "Aucune salle", "Aucune salle disponible.");
            return;
        }
    
        std::vector<RoomInfo> rooms(nbRooms);
        if (!recvAll(sock, rooms.data(), nbRooms * sizeof(RoomInfo))) {
            QMessageBox::critical(this, "Erreur", "Impossible de recevoir les infos de salle.");
            return;
        }
    
        qDebug() << "Nombre de salles reçues : " << nbRooms;
    }
    
}

void MainWindow::setGameScreen() {
    stackedWidget->setCurrentWidget(gameScreen);
}

void MainWindow::setupGameScreens() {
    gameScreen = new QWidget(this);
    auto layout = new QVBoxLayout(gameScreen);

    renderQt = new RenderEngineQt(gameScreen);
    renderQt->setThreadData(threadData);
    layout->addWidget(renderQt);

    gameScreen->setLayout(layout);
    stackedWidget->addWidget(gameScreen);
}

void MainWindow::updateGameScreen() {
    if (!renderQt || !threadData) return;
    std::lock_guard<std::mutex> lock(threadData->mux);
    qDebug() << "updateGameScreen() playersData size:" << threadData->playersData.size();
    if (!threadData->userInGame) return;
    renderQt->renderGames(threadData->playersData);
    stackedWidget->setCurrentWidget(gameScreen);
}


void MainWindow::showModeScreen(const QString& mode) {
    if (mode == "endless") {
        sendMenuAction(MenuActions::A_Play, Modes::Endless_);
    } else if (mode == "classic") {
        sendMenuAction(MenuActions::A_Play, Modes::Classic_);
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
    
    pthread_create(&threadID, nullptr, Player::threadRoutineView, this->threadData.get());

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

    QPushButton* refreshBtn = new QPushButton("Refresh");
    QPushButton* backBtn = new QPushButton("Back");
    layout->addWidget(refreshBtn);
    layout->addWidget(backBtn);

    connect(refreshBtn, &QPushButton::clicked, this, [this]() {
        requestRoomList();  // define this to re-fetch rooms from server
    });

    connect(backBtn, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    stackedWidget->addWidget(roomListScreen);
    stackedWidget->setCurrentWidget(roomListScreen);
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
