#ifdef scroll
#undef scroll
#endif

#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include "communication.hpp"
#include "MainWindow.hpp"


MainWindow::MainWindow(QWidget* parent, int sock) : QMainWindow(parent) , sock(sock){
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    // Création du widget central avec QStackedWidget
    auto centralWidget = new QWidget(this);
    auto layout = new QVBoxLayout(centralWidget);

    stackedWidget = new QStackedWidget(this);

    // Configuration des écrans
    setupLoginScreen();
    setupAuthScreen();

    // Ajouter les pages au QStackedWidget
    stackedWidget->addWidget(loginScreen);
    stackedWidget->addWidget(authScreen);

    layout->addWidget(stackedWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Afficher l'écran de connexion au départ
    stackedWidget->setCurrentWidget(loginScreen);
}

void MainWindow::setupLoginScreen() {
    loginScreen = new QWidget(this);
    auto layout = new QVBoxLayout(loginScreen);

    QLabel *titleLabel = new QLabel("Welcome !", this);
    layout->addWidget(titleLabel);

    QPushButton *loginButton = new QPushButton("Login", this);
    QPushButton *registerButton = new QPushButton("Register", this);

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
    QPushButton *backButton = new QPushButton("Retour", this);
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
    stackedWidget->setCurrentWidget(authScreen);
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

    int size = sizeof(Connexion);

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
    //user = username.toStdString();
    QMessageBox::information(this, "Succès", isRegisterMode ? "Inscription et connexion réussies !" : "Connexion réussie !");
    // TODO : aller à la page suivante (menu, partie, etc.)
}
