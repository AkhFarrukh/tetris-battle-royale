#include "FriendsHandler.hpp"





FriendsHandler::FriendsHandler(QWidget* parent, const QString& currentUser, const std::vector<QString>& friendsList, int countRequests)
        : QWidget(parent), currentUser(currentUser) {

    
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
        QTableWidgetItem* itemName = new QTableWidgetItem(friendsList[i]);            // Crée l'élément pour le nom de l'ami

        friendsTableWidget->setItem(i, 0, itemNumber);  // Colonne 0 pour le numéro
        friendsTableWidget->setItem(i, 1, itemName);    // Colonne 1 pour le nom
    }

    layout->addWidget(friendsTableWidget);  // Ajout du tableau à l'interface

    addFriendButton = new QPushButton("Add new friend"); // Bouton pour ajouter un ami
    layout->addWidget(addFriendButton);    // bouton ajouté au layout

    friendRequestsButton = new QPushButton(QString("Friend requests (%1)").arg(countRequests));  // Bouton pour les demandes d'amis
    layout->addWidget(friendRequestsButton);    

    window->setLayout(layout);
    window->show();

}

FriendsHandler::~FriendsHandler() {}


/**
* @brief permet de rechercher le username d'un joueur.
* permet de faire la recherche via un champs et un bouton.
*/
void FriendsHandler::searchPlayerName() {

    if (!window->layout()) return;
    // pour effacer les anciennes widget
    if (usernameInput) usernameInput->deleteLater();
    if (infoLabel) infoLabel->deleteLater();

    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter player username");

    infoLabel = new QLabel();
    infoLabel->setStyleSheet("color: green; font-weight: bold;");

    QPushButton* searchPlayerButton = new QPushButton("Search");

    layout()->addWidget(usernameInput);
    layout()->addWidget(searchPlayerButton);
    layout()->addWidget(infoLabel);

    connect(searchPlayerButton, SIGNAL(clicked()), this, SLOT(searchNameButtonClicked));  // pour faire la connexion au bouton searchnamebuttonclicked

}


/**
* @brief Slot appelé quand un user clique sur le bouton de recherche.
* Ca va verifier l'entrée du user et lancer la recherche
* 
*/
void FriendsHandler::searchNameButtonClicked() {
    QString usernamePlayer = usernameInput->text();  // recuperer le texte ecrit par le user dans le champs de saisie

    // dans le cas où le champs de recherche est vide
    if (usernamePlayer.isEmpty()){
        infoLabel->setText("No username given");
        return;
    }

    // si l'utilisateur à entré son propre username
    if (usernamePlayer == this->currentUser){
        infoLabel->setStyleSheet("color: red;");
        infoLabel->setText("Please enter a name different from yours! ");
        return;
    }

    infoLabel->setStyleSheet("color: green;");
    infoLabel->setText("searching player...");

    // Effectuer la recherche après une courte interruption
    QTimer::singleShot(1000, this, SLOT(handleFriendsRequests()));
}




void FriendsHandler::showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList){
    
    if (requestsTable) {
        requestsTable->deleteLater();  // suppression
    }

    // cree table pour pouvoir afficher sous forme de colonnes les requêtes (3 colonnes)
    QTableWidget* requestsTable = new QTableWidget(requestsReceivedList.size() + requestSentList.size(), 3);

    requestsTable->horizontalHeader()->setVisible(false);
    requestsTable->horizontalHeader()->setStretchLastSection(true); // Étire la dernière colonne

    int row = 0;

    for (const auto& request_received: requestsReceivedList){
        QString requestReceived_name = QString::fromStdString(request_received);
        requestsTable->setItem(row, 0, new QTableWidgetItem(requestReceived_name)); // colonne 0 nom
        requestsTable->setItem(row, 1, new QTableWidgetItem("Received")); // col 1 type de requete (received ou sent)

        // bouton accept et refuse
        QWidget* actionWidget = new QWidget;
        QPushButton* acceptButton = new QPushButton("Accept");
        QPushButton* refuseButton = new QPushButton("Refuse");
        
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->addWidget(acceptButton);
        layout->addWidget(refuseButton);
        actionWidget->setLayout(layout);

        requestsTable->setCellWidget(row, 2, actionWidget); //2 boutons pour la deuxieme colonnes (accept / refuse)

        //propriétés des boutons accept et refuse
        acceptButton->setProperty("username", requestReceived_name);
        acceptButton->setProperty("type", "received");
        refuseButton->setProperty("username", requestReceived_name);
        refuseButton->setProperty("type", "received");

        //liaison à leur slot
        connect(acceptButton, &QPushButton::clicked, this, &FriendsHandler::acceptFriendRequest);
        connect(refuseButton, &QPushButton::clicked, this, &FriendsHandler::refuseFriendRequest);
        //connect(cancelButton, &QPushButton::clicked, this, &FriendsHandler::refuseRequest);


        row++;
    }

    qobject_cast<QVBoxLayout*>(window->layout())->addWidget(requestsTable);

    for (const auto& request_sent : requestSentList) {
        QString requestSentTo = QString::fromStdString(request_sent);
        requestsTable->setItem(row, 0, new QTableWidgetItem(requestSentTo));  // colonne 0 nom
        requestsTable->setItem(row, 1, new QTableWidgetItem("Sent"));     // colonne 1 type

        // Bouton unique cancel pour annuler la demande
        QPushButton* cancelButton = new QPushButton("Cancel");
        
        //proprietes du bouton
        cancelButton->setProperty("username", requestSentTo);
        cancelButton->setProperty("type", "sent");

        //connect(cancelButton, &QPushButton::clicked, this, &FriendsHandler::refuseRequest);
        requestsTable->setCellWidget(row, 2, cancelButton); // colonne 2 : bouton "Cancel"
        row++;
    }
    
    // Ajout de la table dans l'interface (layout déjà utilisé par la fenêtre)
    layout()->addWidget(requestsTable);

}



void FriendsHandler::acceptFriendRequest() {

    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;  // Si il n est pas valide, on stop

    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    db.addFriend(currentUser.toStdString(), requesterPlayer.toStdString());                          // ajout dans la db
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());                // retire la requete qui a été traitée

    QMessageBox::information(window, "Request Accepted", "you have accepted the friend request !");

    handleFriendsRequests();
}


void FriendsHandler::refuseFriendRequest() {
    
    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;  // Si il n est pas valide, on stop
    
    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());                // retire la requete qui a été traitée
    QMessageBox::information(window, "Request Denied", "you have denied the the friend request !");
    showFriendsRequests(db.requestReceivedList(currentUser.toStdString()), db.requestSentList(currentUser.toStdString()));  // mettre à jour affichage des requêtes
}



/**
*@brief Gerer les requetes d'amis.
*/
void FriendsHandler::handleFriendsRequests() {
    
    // Récupérer les demandes reçues et envoyées depuis la base de données
    std::vector<string> requests_received_list = db.requestReceivedList(currentUser.toStdString());
    std::vector<string> requests_sent_list = db.requestSentList(currentUser.toStdString());

    // Afficher les demandes d'amis reçues et envoyées dans l'interface graphique
    showFriendsRequests(requests_received_list, requests_sent_list);

    // Connexion des boutons pour accepter, refuser ou annuler une demande
    /*
    connect(acceptButton, &QPushButton::clicked, this, &FriendsHandler::acceptRequest);
    connect(refuseButton, &QPushButton::clicked, this, &FriendsHandler::refuseRequest);
    connect(cancelButton, &QPushButton::clicked, this, &FriendsHandler::cancelRequest);
    */
    
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











