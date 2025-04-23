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
#include <atomic>
#include "communication.hpp"
#include "playerFunctions.hpp"
#include "GridQt.hpp"
#include "userdatabasemanager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, int sock, std::shared_ptr<Player::ThreadData> threadData);
    ~MainWindow();

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
    void addMalusButtonsIfRoyal();
    void setGameScreen();
    void showLeaderboardScreen();

private:
    int sock;
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();
    void setupRoomListScreen(const std::vector<RoomInfo>& roomsInfo);
    void setupMyRoomScreen(const RoomInfo& room, bool isOwner);
    void joinRoom(int roomID);
    void sendForceStart();
    void sendLeaveRoom();
    void requestRoomUpdate();
    void requestRoomList();
    void sendCreateRoom();
    static void* threadRoutineQtView(void* arg);
    void handleMalusBonus(int malusId);
    

    QWidget* loginScreen = nullptr;
    QWidget* authScreen = nullptr;
    QWidget* mainMenuScreen = nullptr;
    QWidget* gameModeScreen = nullptr;
    QWidget* gameScreen = nullptr;
    QWidget* leaderboardScreen = nullptr;
    QLineEdit* usernameInput = nullptr;
    QLineEdit* passwordInput = nullptr;
    QLineEdit* confirmPasswordInput = nullptr;

    QPushButton* playButton = nullptr;
    QPushButton* handleFriendsButton = nullptr;
    QPushButton* leaderboardButton = nullptr;

    QStackedWidget* stackedWidget = nullptr;
    GridQt* renderQt = nullptr;

    bool isRegisterMode = false;

    std::shared_ptr<Player::ThreadData> threadData;
    pthread_t threadID;

    Database db{"./tetris.db"};
    UserDatabaseManager udm;

    QWidget* roomListScreen = nullptr;
    QWidget* myRoomScreen = nullptr;

};

#endif // MAINWINDOW_HPP
