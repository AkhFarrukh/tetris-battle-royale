#ifndef FRIENDSHANDLER_HPP
#define FRIENDSHANDLER_HPP

#include <iostream>
#include <QWidget>
#include <QStringList>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTimer>
#include <QLineEdit>
#include <QVector>
#include <QMessageBox>
#include <QStackedWidget>

#include "../Database/database.hpp" 
#include "../Model/userdatabasemanager.hpp"
#include <atomic>
#include <mutex>


extern std::mutex interfaceMutex;
extern Database db;
extern UserDatabaseManager udm;

class FriendsHandler: public QWidget {

    Q_OBJECT

    private:
        QString currentUser; 
        int countRequests;   // nombre de demande d'amis
        
        QWidget* window;
        QLabel* titleLabel;
        QLabel* infoLabel;  // message d'info à l'utilisateur
        
        QPushButton* addFriendButton;
        QPushButton* friendRequestsButton;

        QTableWidget* friendsTableWidget;
        QTableWidget* requestsTable;

        QLineEdit* usernameInput; // champs qui va contenir le username recherché
        QListWidget* requestsListWidget;  // Liste des demandes d'amis

        QStackedWidget* stackedWidget;
        QWidget* friendsListScreen;
        QWidget* addFriendScreen;
        QWidget* requestsScreen;

        void setupFriendsListScreen(const std::vector<QString>& friendsList);
        void setupAddFriendScreen();
        void setupRequestsScreen();


    public:
        FriendsHandler(QWidget* parent = nullptr, const QString& currentUser = "", const std::vector<QString>& friendsList = {}, int countRequests = 0);
        ~FriendsHandler() override;

    
        void showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList);
        
        QWidget* getWindowWidget() const;
    
    public slots:
        void searchNameButtonClicked();
        void acceptFriendRequest();
        void refuseFriendRequest();
        void handleFriendsRequests();
};





#endif 