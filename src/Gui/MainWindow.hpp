#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#ifdef border
#undef border
#endif

#ifdef scroll
#undef scroll
#endif

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <csignal>
#include <QCloseEvent>
#include "communication.hpp"
#include "playerFunctions.hpp"
#include "RenderEngineQt.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData);
    ~MainWindow();

    private slots:
    void showLoginScreen(); // Affiche la page de connexion
    void showAuthScreen();  // Affiche la page Username/Password
    void handleAuth();
    void setupMainMenuScreen();
    void setupGameModeScreen();
    void sendMenuAction(MenuActions action, Modes mode);
    void showModeScreen(const QString& mode);
    void showGameModeScreen();
    void showMainMenuScreen();
    void closeEvent(QCloseEvent *event) override;
    void setupGameScreens();

private:
    int sock;
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();

    QWidget *endlessScreen = nullptr;
    RenderEngineQt* renderQt = nullptr;

    QStackedWidget *stackedWidget;

    // Widgets de la page Login/Register
    QWidget *loginScreen;

    // Widgets de la page Username/Password
    QWidget *authScreen;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QLineEdit *confirmPasswordInput = nullptr; // champ en plus si Register
    QWidget *mainMenuScreen;
    QWidget *selectGameModeScreen;
    QWidget* gameModeScreen;
    QPushButton *playButton;
    QPushButton *handleFriendsButton;
    QPushButton *leaderboardButton;
    // Mode actuel (login ou register)
    bool isRegisterMode = false;

    // Thread de communication
    std::shared_ptr<Player::ThreadData> threadData;
    pthread_t threadID;
    
};

#endif // MAINWINDOW_HPP
