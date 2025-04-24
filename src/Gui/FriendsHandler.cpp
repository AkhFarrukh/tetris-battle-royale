#include "FriendsHandler.hpp"


FriendsHandler::FriendsHandler(QWidget* parent, const std::vector<QString>& friendsList, int countRequests){
    window = new QWidget(parent);
    window->setWindowTitle("Friends List");
    window->resize(400, 500);

    QVBoxLayout* layout = new QVBoxLayout(window);

    titleLabel = new QLabel("Friends list:");
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);               // mettre en gras
    titleFont.setPointSize(14);            // taille police
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // pour pouvoir afficher amis sous forme de colonnes
    friendsTableWidget = new QTableWidget();
    friendsTableWidget->setColumnCount(2);  // 2 colonnes : Numéro et Nom
    friendsTableWidget->setRowCount(friendsList.size());  // Le nombre de lignes est égal au nombre d'amis

    // Masquer les en-têtes des colonnes et des lignes
    friendsTableWidget->horizontalHeader()->setVisible(false);
    friendsTableWidget->verticalHeader()->setVisible(false);

    // parcourt les amis dans la liste
    for(size_t i = 0; i < friendsList.size(); ++i){
        QTableWidgetItem* itemNumber = new QTableWidgetItem(QString::number(i + 1));  // Crée l'élément pour le numéro de l'ami
        QTableWidgetItem* itemName = new QTableWidgetItem(friendsList[i]); // Crée l'élément pour le nom de l'ami

        friendsTableWidget->setItem(i, 0, itemNumber);  // Colonne 0 pour le numéro
        friendsTableWidget->setItem(i, 1, itemName);    // Colonne 1 pour le nom
    }

    layout->addWidget(friendsTableWidget);  // Ajout du tableau à l'interface

    
    addFriendButton = new QPushButton("Add new friend"); // Bouton pour ajouter un ami
    layout->addWidget(addFriendButton); // bouton ajouté au layout

   
    friendRequestsButton = new QPushButton(QString("Friend requests (%1)").arg(countRequests));  // Bouton pour les demandes d'amis
    layout->addWidget(friendRequestsButton);    

    window->setLayout(layout);
    window->show();

}

QWidget* FriendsHandler::getWindowWidget() const{
    return window;
}

QTableWidget* FriendsHandler::getFriendsTableWidget() const{
    return friendsTableWidget;
}

QPushButton* FriendsHandler::getAddFriendButton() const{
    return addFriendButton;
}

QPushButton* FriendsHandler::getFriendRequestButton() const{
    return friendRequestsButton;
}





































