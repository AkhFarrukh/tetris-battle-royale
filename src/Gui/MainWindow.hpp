#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, int sock=-1);
    ~MainWindow();

    private slots:
        void showLoginScreen(); // Affiche la page de connexion
    void showAuthScreen();  // Affiche la page Username/Password
    void handleAuth();
    void setupMainMenuScreen();
    void setupGameModeScreen();
    void showGameModeScreen();
    void showMainMenuScreen();
private:
    int sock;
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();

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
    bool isRegisterMode = false;
};

#endif // MAINWINDOW_HPP
