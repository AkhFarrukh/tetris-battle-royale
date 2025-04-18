#ifdef scroll
#undef scroll
#endif

#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QScreen>
#include "communication.hpp"
#include "MainWindow.hpp"



MainWindow::MainWindow(QWidget* parent, int sock) : sock(sock), QMainWindow(parent){
    setupUI();
    // Taille plus grande
    resize(600, 500); // ou setFixedSize si tu veux fixer la taille

    // Centrage sur l'écran principal
    move(QGuiApplication::primaryScreen()->geometry().center() - frameGeometry().center());
}

MainWindow::~MainWindow() {}


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
    setupFriendsListScreen();

    // Ajouter les pages au QStackedWidget
    stackedWidget->addWidget(loginScreen);
    stackedWidget->addWidget(authScreen);
    stackedWidget->addWidget(mainMenuScreen);
    stackedWidget->addWidget(gameModeScreen);
    stackedWidget->addWidget(friendsListScreen);


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

void MainWindow::setupGameModeScreen() {
    gameModeScreen = new QWidget(this);
    gameModeScreen->setStyleSheet("background-color: #a066cc;"); // fond violet
    auto layout = new QVBoxLayout(gameModeScreen);

    QLabel *titleLabel = new QLabel("Select a Game Mode:", this);
    layout->addWidget(titleLabel);

    QPushButton *endlessButton = new QPushButton("Endless Mode", this);
    endlessButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *classicButton = new QPushButton("Classic Mode", this);
    classicButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *duelButton = new QPushButton("Duel Mode", this);
    duelButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *royalButton = new QPushButton("Royal Competition Mode", this);
    royalButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert
    QPushButton *backButton = new QPushButton("Retour", this);  // <-- Ajout du bouton retour
    backButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;"); // bouton vert


    layout->addWidget(endlessButton);
    layout->addWidget(classicButton);
    layout->addWidget(duelButton);
    layout->addWidget(royalButton);
    layout->addWidget(backButton);  // <-- Ajout dans le layout

    connect(backButton, &QPushButton::clicked, this, &MainWindow::showMainMenuScreen); // <-- Connexion du bouton

    gameModeScreen->setLayout(layout);
    stackedWidget->addWidget(gameModeScreen); // Important !
}

void MainWindow::showMainMenuScreen() {
    stackedWidget->setCurrentWidget(mainMenuScreen);
}

void MainWindow::showGameModeScreen() {
    stackedWidget->setCurrentWidget(gameModeScreen);
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

    QPushButton *chatButton = new QPushButton(this);
	chatButton->setText(" Chat");

	chatButton->setIcon(QIcon::fromTheme("mail-message-new")); // Peut être remplacé par un chemin QIcon(":/icons/chat.png")

	chatButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 4px; border-radius: 6px;");
	chatButton->setIconSize(QSize(20, 20));

	layout->addWidget(chatButton);

	connect(chatButton, &QPushButton::clicked, [this]() {
        callerWindow = mainMenuScreen;//new QWidget(this);
    	stackedWidget->setCurrentWidget(friendsListScreen);
});

    layout->addWidget(playButton);
    layout->addWidget(friendsButton);
    layout->addWidget(leaderboardButton);

    connect(playButton, &QPushButton::clicked, this, &MainWindow::showGameModeScreen);

    mainMenuScreen->setLayout(layout);
    stackedWidget->addWidget(mainMenuScreen); // Important !
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
