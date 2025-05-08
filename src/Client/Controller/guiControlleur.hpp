#ifndef GUICONTROLLEUR_HPP
#define GUICONTROLLEUR_HPP

#include <iostream>
#include "FriendsHandler.hpp"
#include "ChatHandler.hpp"
#include "communication.hpp"
#include "playerFunctions.hpp"
#include "GridQt.hpp"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <atomic>
#include <QMainWindow>
#include "MainWindow.hpp"


extern std::atomic<bool> flag;


class guiControlleur : public QObject {
    Q_OBJECT
private:
    int sock;
    std::shared_ptr<Player::ThreadData> threadData;
    pthread_t threadID;
    void sendCreateRoom();
    void joinRoom(int roomID);
    void sendForceStart();
    void sendLeaveRoom();
    void requestRoomUpdate();
    void requestRoomList();
    void handleAuth(const QString& username, const QString& password, bool isRegister, const QString& confirmPassword = "", bool isRegisterMode = false);
    static void* threadRoutineQtView(void* arg);
    void handleMalusBonus(int malusId);
    void sendMalusBonus(int malusId, int targetPlayerIndex);
    void handleFriendsMenu();
    void handleChatMenu();
    void sendMenuAction(MenuActions action, Modes mode);
    void spectateRoom(int roomID);
    
    FriendsHandler* friendsHandler = nullptr;
    ChatHandler* chatHandler = nullptr;
        
public:
    guiControlleur(int sock, std::shared_ptr<Player::ThreadData> threadData);
    ~guiControlleur();
};

#endif // GUICONTROLLEUR_HPP