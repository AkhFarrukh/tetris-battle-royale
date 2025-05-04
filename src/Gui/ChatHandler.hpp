#ifndef CHATHANDLER_HPP
#define CHATHANDLER_HPP



#include <iostream>
#include <QWidget>
#include <QStringList>
#include <QListWidget>
#include <QCloseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTimer>
#include <QLineEdit>
#include <QVector>
#include <vector>
#include <QMessageBox>
#include <QApplication>
#include <QStackedWidget>

#include "../Database/database.hpp"
#include "../Model/userdatabasemanager.hpp"
#include <atomic>
#include <mutex>



extern std::mutex interfaceMutex;
extern Database db;
extern UserDatabaseManager udm;

class MainWindow;

class ChatHandler: public QWidget {

    Q_OBJECT

    private:
        QString currentUser;

        QWidget *callerWindow = nullptr;
        QTextEdit *chatHistory = nullptr;

        QStackedWidget* stackedWidget;
        QWidget *chatScreen = nullptr;
        QWidget *friendsListScreen = nullptr;
        int sock;
        MainWindow* mainWindow;

        void setupFriendsListScreen(const std::vector<QString>& friendsList);
        void openChatWith(const QString& friendName);


    public:

        ChatHandler(int sock, MainWindow* mainWindow, QWidget* parent = nullptr, const QString& currentUser = "", const std::vector<QString>& friendsList = {}, QWidget* callerWindow = nullptr);
        ~ChatHandler() override;
        QTextEdit *getChatHistory();
        //void showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList);

        //QWidget* getWindowWidget() const;

    /*public slots:
        void searchNameButtonClicked();
        void acceptFriendRequest();
        void refuseFriendRequest();
        void handleFriendsRequests();*/
};







#endif //CHATHANDLER_HPP
