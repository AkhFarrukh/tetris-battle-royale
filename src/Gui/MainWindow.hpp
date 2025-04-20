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
#include <QTimer>
#include "communication.hpp"
#include "playerFunctions.hpp"
#include "RenderEngineQt.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData);
    ~MainWindow();

    void setupRoomListScreen(const std::vector<RoomInfo>& roomsInfo);
    void setupMyRoomScreen(const RoomInfo& room, bool isOwner);


private slots:
    void showLoginScreen();
    void showAuthScreen();
    void handleAuth();
    void setupMainMenuScreen();
    void setupGameModeScreen();
    void sendMenuAction(MenuActions action, Modes mode);
    void showModeScreen(const QString& mode);
    void showGameModeScreen();
    void showMainMenuScreen();
    void closeEvent(QCloseEvent *event) override;
    void setupGameScreens();
    void updateGameScreen();
    void setGameScreen();

private:
    int sock;
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();
    void joinRoom(int roomID);
    void sendForceStart();
    void sendLeaveRoom();
    void requestRoomUpdate();
    void requestRoomList();

    QWidget* loginScreen = nullptr;
    QWidget* authScreen = nullptr;
    QWidget* mainMenuScreen = nullptr;
    QWidget* gameModeScreen = nullptr;
    QWidget* gameScreen = nullptr;

    QLineEdit* usernameInput = nullptr;
    QLineEdit* passwordInput = nullptr;
    QLineEdit* confirmPasswordInput = nullptr;

    QPushButton* playButton = nullptr;
    QPushButton* handleFriendsButton = nullptr;
    QPushButton* leaderboardButton = nullptr;

    QStackedWidget* stackedWidget = nullptr;
    RenderEngineQt* renderQt = nullptr;

    bool isRegisterMode = false;

    std::shared_ptr<Player::ThreadData> threadData;
    pthread_t threadID;


    QWidget* roomListScreen = nullptr;
    QWidget* myRoomScreen = nullptr;

};

#endif // MAINWINDOW_HPP
