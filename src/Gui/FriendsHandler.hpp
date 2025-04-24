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
        QWidget* window;
        QLabel* titleLabel;
        QLabel* infoLabel;  // message d'info à l'utilisateur

        QTableWidget* friendsTableWidget;
        QPushButton* addFriendButton;
        QPushButton* friendRequestsButton;

        QLineEdit* usernameInput; // champs qui va contenir le username recherché
        QString currentUser; 


    
    public:

        FriendsHandler(QWidget* parent = nullptr, const std::vector<QString>& friendsList = {}, int countRequests = 0);
        
        void searchPlayerName() ;
        void handleFriendRequests();
        
        QWidget* getWindowWidget() const;
        QTableWidget* getFriendsTableWidget() const;
        QPushButton* getAddFriendButton() const;
        QPushButton* getFriendRequestButton() const;
    
    
    public slots:
        void searchNameButtonClicked();

};





#endif 