#ifdef scroll
#undef scroll
#endif

#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
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

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::showAuthScreen);

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

    QPushButton *backButton = new QPushButton("Retour", this);

    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, this, &MainWindow::showLoginScreen);

    authScreen->setLayout(layout);
}

void MainWindow::showLoginScreen() {
    stackedWidget->setCurrentWidget(loginScreen);
}

void MainWindow::showAuthScreen() {
    stackedWidget->setCurrentWidget(authScreen);
}