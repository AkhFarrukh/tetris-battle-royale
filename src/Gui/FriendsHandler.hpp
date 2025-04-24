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
#include "../Database/database.hpp" //changer en "database.hpp" si probleme
#include "../Model/userdatabasemanager.hpp"
#include <atomic>
#include <mutex>


extern std::mutex interfaceMutex;
extern Database db;
extern UserDatabaseManager udm;

class FriendsHandler {

    private:
        QWidget* window;
        QLabel* titleLabel;
        QTableWidget* friendsTableWidget;
        QPushButton* addFriendButton;
        QPushButton* friendRequestsButton;

    
    public:

        FriendsHandler(QWidget* parent = nullptr, const std::vector<QString>& friendsList = {}, int countRequests = 0);
        
        QWidget* getWindowWidget() const;
        QTableWidget* getFriendsTableWidget() const;
        QPushButton* getAddFriendButton() const;
        QPushButton* getFriendRequestButton() const;
};





#endif 