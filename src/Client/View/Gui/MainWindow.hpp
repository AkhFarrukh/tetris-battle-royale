#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#ifdef border
#undef border
#endif

#ifdef scroll
#undef scroll
#endif

#ifdef timeout
#undef timeout
#endif


#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent>
#include <QTextEdit>
#include <QTimer>
#include <atomic>
#include "communication.hpp"
#include "playerFunctions.hpp"
#include "GridQt.hpp"
#include "FriendsHandler.hpp"
#include "ChatHandler.hpp"
#include "guiControlleur.hpp"
#include <memory>

class guiControlleur;  // ✅ forward declaration



class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData);
    ~MainWindow();
    QStackedWidget* getStackedWidget();
    void showGameOverPopup();
    void setGameScreen();
    void addMalusButtonsIfRoyal();
    void setupSpectatorWaitingScreen(int roomID);
    FriendsHandler* friendsHandler = nullptr;
    ChatHandler* chatHandler = nullptr;
    void setupMyRoomScreen(const RoomInfo& room, bool isOwner);
    void setupRoomListScreen(const std::vector<RoomInfo>& roomsInfo);

private slots:
    void showLoginScreen();
    void showAuthScreen();
    void setupMainMenuScreen();
    void setupGameModeScreen();
    void setupFriendsScreen();
    void showModeScreen(const QString& mode);
    void showGameModeScreen();
    void showMainMenuScreen();
    void closeEvent(QCloseEvent *event) override;
    void setupGameScreens();
    void updateGameScreen();
    void showLeaderboardScreen();
    void showChatScreen();
    void showfriendsListScreen();
    


private:
    QWidget *callerWindow = nullptr;
    QTextEdit *chatHistory = nullptr;
    int sock;
    std::string username;
    int userId_;

    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();
    void handleFriendsMenu();
    void setupFriendsListScreen();
    void handleAuth();
    void openChatWith(const QString& friendName);
    void handleChatMenu();

    QWidget* loginScreen = nullptr;
    QWidget* authScreen = nullptr;
    QWidget* mainMenuScreen = nullptr;
    QWidget* gameModeScreen = nullptr;
    QWidget* friendsScreen = nullptr;
    QWidget* gameScreen = nullptr;
    QWidget* leaderboardScreen = nullptr;
    QLineEdit* usernameInput = nullptr;
    QLineEdit* passwordInput = nullptr;
    QLineEdit* confirmPasswordInput = nullptr;
    QWidget* spectatorWaitingScreen = nullptr;

    QPushButton* playButton = nullptr;
    QPushButton* handleFriendsButton = nullptr;
    QPushButton* leaderboardButton = nullptr;
    QWidget *chatScreen = nullptr;
    QWidget *friendsListScreen = nullptr;

    QStackedWidget* stackedWidget = nullptr;
    GridQt* renderQt = nullptr;

    bool isRegisterMode = false;

    std::shared_ptr<Player::ThreadData> threadData;
    pthread_t threadID;

    UserDatabaseManager udm;
    guiControlleur* controller;

    QWidget* roomListScreen = nullptr;
    QWidget* myRoomScreen = nullptr;

};

#endif // MAINWINDOW_HPP
