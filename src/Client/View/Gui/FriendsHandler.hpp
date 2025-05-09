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

#include "Database.hpp" 
#include "UserDatabaseManager.hpp"
#include <atomic>
#include <mutex>
#include <string>


extern std::mutex interfaceMutex;

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

        UserDatabaseManager udm;


    public:
        FriendsHandler(QWidget* parent = nullptr, const QString& currentUser = "", const std::vector<QString>& friendsList = {}, int countRequests = 0);   // jsuis obligé de mettre "" car scurrentuser doit etre assigné
        ~FriendsHandler() override;

        void showFriendsRequests(const std::vector<std::string>& requestsReceivedList, const std::vector<std::string>& requestSentList);
    
    public slots:
        void searchNameButtonClicked();
        void acceptFriendRequest();
        void refuseFriendRequest();
        void removeFriend();
        void handleFriendsRequests();
};





#endif 