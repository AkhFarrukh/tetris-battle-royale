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

#include "../Database/database.hpp" //changer en "database.hpp" si probleme
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
        
        QWidget* window;
        QLabel* titleLabel;
        QLabel* infoLabel;  // message d'info à l'utilisateur
        
        QPushButton* addFriendButton;
        QPushButton* friendRequestsButton;

        QTableWidget* friendsTableWidget;
        QTableWidget* requestsTable;

        QLineEdit* usernameInput; // champs qui va contenir le username recherché
        QListWidget* requestsListWidget;  // Liste des demandes d'amis


    public:
        FriendsHandler(QWidget* parent = nullptr, const QString& currentUser = "", const std::vector<QString>& friendsList = {}, int countRequests = 0);
        ~FriendsHandler() override;

        
        void searchPlayerName() ;
        void showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList);
        
        QWidget* getWindowWidget() const;
        QTableWidget* getFriendsTableWidget() const;
        QPushButton* getAddFriendButton() const;
        QPushButton* getFriendRequestButton() const;
    
    
    public slots:
        void searchNameButtonClicked();
        void acceptFriendRequest();
        void refuseFriendRequest();
        void handleFriendsRequests();
};





#endif 