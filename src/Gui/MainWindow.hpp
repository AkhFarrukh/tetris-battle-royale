#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <csignal>
#include <QCloseEvent>
#include <QScreen>
#include <QTextEdit>

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidgetItem>
#include <QListWidget>
#include <QTextEdit>
#include <QGuiApplication>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, int sock=-1);
    ~MainWindow();

    private slots:
        void showLoginScreen(); // Affiche la page de connexion
        void showfriendsListScreen();
    void showAuthScreen();  // Affiche la page Username/Password
    void handleAuth();
    void setupMainMenuScreen();
    void setupGameModeScreen();
    void showGameModeScreen();
    void showMainMenuScreen();
    void closeEvent(QCloseEvent *event) override;
    void showChatScreen();

private:
    int sock;
    QWidget *callerWindow = nullptr;
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();
    void setupFriendsListScreen();
    void openChatWith(const QString& friendName);

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
    QWidget *chatScreen;
    QPushButton *playButton;
    QPushButton *handleFriendsButton;
    QPushButton *leaderboardButton;
    QWidget *friendsListScreen;

    bool isRegisterMode = false;
};

#endif // MAINWINDOW_HPP
