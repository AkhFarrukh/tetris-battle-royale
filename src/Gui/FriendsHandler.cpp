#include "FriendsHandler.hpp"
#include "MainWindow.hpp"





FriendsHandler::FriendsHandler(QWidget* parent, const QString& currentUser, const std::vector<QString>& friendsList, int countRequests)
    : QWidget(parent), currentUser(currentUser), countRequests(countRequests) {
    
    stackedWidget = new QStackedWidget(this);
    
    // Créer les différents écrans
    setupFriendsListScreen(friendsList);
    setupAddFriendScreen();
    setupRequestsScreen();
    
    // Ajouter les écrans au stackedWidget
    stackedWidget->addWidget(friendsListScreen);
    stackedWidget->addWidget(addFriendScreen);
    stackedWidget->addWidget(requestsScreen);
    
    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
    
    // Afficher l'écran principal au début
    stackedWidget->setCurrentWidget(friendsListScreen);
}

FriendsHandler::~FriendsHandler() {
    
}



/**
* @brief Slot/méthode appelé quand un user clique sur le bouton de recherche.
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

    // verifier si la personne existe
    std::string player = usernamePlayer.toStdString();
    int friendId = db.getUserId(player.c_str());
    if(friendId == -1){
        infoLabel->setStyleSheet("color: red;");
        infoLabel->setText("User not found");
        return;
    }

    //verif si deja ami ou requete deja envoyée
    int requestStatus = db.verifyRequestFriend(currentUser.toStdString(), usernamePlayer.toStdString());
    if (requestStatus == 1) {
        infoLabel->setStyleSheet("color: red;");
        infoLabel->setText("You are already friends!");
        return;
    } else if (requestStatus == 2) {
        infoLabel->setStyleSheet("color: red;");
        infoLabel->setText("Request already sent!");
        return;
    }
    // envoyer la requetes
    db.requestFriend(currentUser.toStdString(), usernamePlayer.toStdString());
    infoLabel->setStyleSheet("color: green;");
    infoLabel->setText("Friend request sent!");
    countRequests = db.getNumberFriendRequestsReceived(currentUser.toStdString());  
    friendRequestsButton->setText(QString("Friend requests (%1)").arg(countRequests));


    // Effectuer la recherche après une courte interruption
    //QTimer::singleShot(1000, this, SLOT(handleFriendsRequests()));
}


/**
 * @brief Configure l'écran qui montre la liste d' amis.
 *
 * Crée un tableau listant les amis fournis, avec leurs numéros et username.
 * gestion de boutons pour accéder à l'ecran "add friend" et "friend request",
 * et pour revenir au menu principal.
 *
 * @param friendsList Vecteur qui contient les username des amis à afficher.
 */
void FriendsHandler::setupFriendsListScreen(const std::vector<QString>& friendsList) {
    friendsListScreen = new QWidget(this);  //widget pour ecran qui affichera liste d amis
    QVBoxLayout* layout = new QVBoxLayout(friendsListScreen);

    QLabel* titleLabel = new QLabel("Friends list:", this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // Tableau des amis (existant)
    friendsTableWidget = new QTableWidget(this);
    friendsTableWidget->setColumnCount(2);
    friendsTableWidget->setRowCount(friendsList.size());
    
    //cacche en-tetes de colonnes
    friendsTableWidget->horizontalHeader()->setVisible(false);  
    friendsTableWidget->verticalHeader()->setVisible(false);

    //parcourt liste d'amis et remplit tableau d'amis
    for(size_t i = 0; i < friendsList.size(); ++i) {
        QTableWidgetItem* itemNumber = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem* itemName = new QTableWidgetItem(friendsList[i]);
        friendsTableWidget->setItem(i, 0, itemNumber);   // col 0  le numero de l ami
        friendsTableWidget->setItem(i, 1, itemName);    // col 1   le username de lami
    } 
    layout->addWidget(friendsTableWidget);

    // Boutons
    addFriendButton = new QPushButton("Add new friend", this);
    friendRequestsButton = new QPushButton(QString("Friend requests (%1)").arg(countRequests), this);
    
    layout->addWidget(addFriendButton);
    layout->addWidget(friendRequestsButton);

    // 2 Connexions des boutons pour qu'ils realisent action demandée qd on click sur eux
    connect(addFriendButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(addFriendScreen);
    });
    connect(friendRequestsButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(requestsScreen);
        
        handleFriendsRequests();
    });
    friendsListScreen->setLayout(layout);
}


/**
 * @brief Configure l'écran d'ajout d'ami.
 *
 * Crée ecran qui permet au user d'entrer un username pour envoyer une requ^te
 * champs de recherche créé + label d'info, boutons pour lancer recherche et revenir en arrière.
 */
void FriendsHandler::setupAddFriendScreen() {
    addFriendScreen = new QWidget(this);   // nouveau widget
    QVBoxLayout* layout = new QVBoxLayout(addFriendScreen);

    QLabel* title = new QLabel("Add New Friend", this);  //label pour afficher sous titre
    QFont titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    layout->addWidget(title);

    usernameInput = new QLineEdit(this);   // créé champs pour entrer username
    usernameInput->setPlaceholderText("Enter player username");
    layout->addWidget(usernameInput);

    //label ppur affiche informations (messages)
    infoLabel = new QLabel(this);
    infoLabel->setStyleSheet("color: green; font-weight: bold;");
    layout->addWidget(infoLabel);

    //boutons
    QPushButton* searchButton = new QPushButton("Search", this);
    QPushButton* backButton = new QPushButton("Back", this);
    
    // placer en horizontal boutons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(backButton);
    layout->addLayout(buttonLayout);

    // Connexions bouton pour pouvoir return en arriere
    connect(backButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(friendsListScreen);
        usernameInput->clear();
        infoLabel->clear();
    });
    connect(searchButton, &QPushButton::clicked, this, &FriendsHandler::searchNameButtonClicked);
    addFriendScreen->setLayout(layout);
}


/**
 * @brief Va afficher et gerer la liste des requêtes d amis recues et envoyées.
 * 
 * Nettoyage de  l'affichage déjà existant (pas le titre),
 * tableau créé pour afficher les requ^tes
 * Ajoute les requêtes reçues avec boutons Accept/Refuse
 * Ajoute les demandes envoyées avec bouton Cancel à coté
 * configurisation du tableau (esthetique)
 * ajoute un bouton Back si y en a pas.
 *
 * @param requestsReceivedList Liste des demandes/requêtes reçues
 * @param requestSentList Liste des requêtes  envoyées
 * 
 */
void FriendsHandler::showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList) {
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(requestsScreen->layout());
    
    if(!layout){
        layout = new QVBoxLayout(requestsScreen);
        requestsScreen->setLayout(layout);
    }
    // Supprimer les widgets sauf le 1er  (titre)
    while (layout->count() > 1) {
        QLayoutItem* item = layout->takeAt(1);
        if (item) {
            if (QWidget* w = item->widget()) {
                w->deleteLater();
            }
                delete item;  //supprime elem de layout
        }
    }
    // Créé la table pour pouvoir afficher demandes d amiis
    QTableWidget* requestsTable = new QTableWidget(requestsReceivedList.size() + requestSentList.size(), 3 , this) ;
    requestsTable->horizontalHeader()->setVisible(false);
    requestsTable->horizontalHeader()->setStretchLastSection(true);

    int row = 0;  // ligne courante , incrementée apres

    // Requêtes reçues
    for (const auto& request_received : requestsReceivedList) {
        QString requestReceived_name = QString::fromStdString(request_received);    // conversion nom (username) en qtstring pour pouvoir l'utiliser
        requestsTable->setItem(row, 0, new QTableWidgetItem(requestReceived_name));
        requestsTable->setItem(row, 1, new QTableWidgetItem("Received"));         // indique que c'est une requete qu on a recue

        // Widget avec boutons Accept/Refuse
        QWidget* actionWidget = new QWidget(this);
        QPushButton* acceptButton = new QPushButton("Accept", this);
        QPushButton* refuseButton = new QPushButton("Refuse", this);

        // pour placer boutons accept/refuse de maniere horizontale
        QHBoxLayout* hLayout = new QHBoxLayout(actionWidget);
        hLayout->addWidget(acceptButton);
        hLayout->addWidget(refuseButton);
        actionWidget->setLayout(hLayout);
        requestsTable->setCellWidget(row, 2, actionWidget); // placer les boutons

        //propriétés des boutons 
        acceptButton->setProperty("username", requestReceived_name);
        acceptButton->setProperty("type", "received");
        refuseButton->setProperty("username", requestReceived_name);
        refuseButton->setProperty("type", "received");

        //connexion boutons aux methodes pour gerer le click du user
        connect(acceptButton, &QPushButton::clicked, this, &FriendsHandler::acceptFriendRequest);
        connect(refuseButton, &QPushButton::clicked, this, &FriendsHandler::refuseFriendRequest);

        row++;
    }
    // Requêtes envoyées
    for (const auto& request_sent : requestSentList) {
        QString requestSentTo = QString::fromStdString(request_sent);
        requestsTable->setItem(row, 0, new QTableWidgetItem(requestSentTo));
        requestsTable->setItem(row, 1, new QTableWidgetItem("Sent"));

        //pour annuler demande
        QPushButton* cancelButton = new QPushButton("Cancel", this);
        cancelButton->setProperty("username", requestSentTo);
        cancelButton->setProperty("type", "sent");
        connect(cancelButton, &QPushButton::clicked, this, &FriendsHandler::refuseFriendRequest);
        requestsTable->setCellWidget(row, 2, cancelButton);  // placer le bouton cancel

        row++;
    }
    layout->insertWidget(1, requestsTable);

    // va redimensionner colonnes
    requestsTable->setColumnWidth(0, 150); // Colonne nom
    requestsTable->setColumnWidth(1, 80);  // Colonne (received / sent)

    bool hasButtonBack = false; // pour savoir si il y a un bouton de retour present à l ecran
    // verifie si un bouton de type QPushButton et de nom "backButton" est present dans layout
    for (int i = 0; i < layout->count(); ++i) {
        if (qobject_cast<QPushButton*>(layout->itemAt(i)->widget()) && layout->itemAt(i)->widget()->objectName() == "backButton") {
            hasButtonBack = true;
            break;
        }
    }
    if(!hasButtonBack){   // creation du bouton pour retourner en arriere
        QPushButton* backButton = new QPushButton("Back", this);
        backButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
        layout->addWidget(backButton);
        connect(backButton, &QPushButton::clicked, [this]() {
            stackedWidget->setCurrentWidget(friendsListScreen);});
    }
}



/**
 * @brief Configure l'écran de gestion des demandes d'amis.
 */
void FriendsHandler::setupRequestsScreen() {
    requestsScreen = new QWidget(this);  // pour l ecran des demandes d'amis
    QVBoxLayout* layout = new QVBoxLayout(requestsScreen);

    QLabel* title = new QLabel("Friend Requests", this);
    QFont titleFont = title->font();
    titleFont.setBold(true);     // active le gras
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    layout->addWidget(title);

    requestsScreen->setLayout(layout);
}


/**
 * @brief Accepte demande/requête d ami.
 * 
 * Le bouton qui a déclenché est recupéré (accept),
 * on recupère le username de la personne qui a envoyé la requete,
 * ajoute en ami dans la db la personne
 * la requête est supprimee de la db.
 */
void FriendsHandler::acceptFriendRequest() {

    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button){ 
        return;  // Si il n est pas valide, on stop
    }
    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    db.addFriend(currentUser.toStdString(), requesterPlayer.toStdString());           // ajout dans la db
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());  // retire la requete qui a été traitée

    QMessageBox::information(this, "Request Accepted", "you have accepted the friend request !");

    std::vector<std::string> friends = db.friendsList(currentUser.toStdString());
    std::vector<QString> qtFriends;
    for (const auto& f : friends) {
        qtFriends.push_back(QString::fromStdString(f));
    }

    friendsTableWidget->clearContents();
    friendsTableWidget->setRowCount(qtFriends.size());

    for(size_t i = 0; i < qtFriends.size(); ++i) {
        QTableWidgetItem* itemNumber = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem* itemName = new QTableWidgetItem(qtFriends[i]);
        friendsTableWidget->setItem(i, 0, itemNumber);
        friendsTableWidget->setItem(i, 1, itemName);
    }

    /** 
    QVBoxLayout* oldLayout = qobject_cast<QVBoxLayout*>(friendsListScreen->layout());
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }
    */

    //setupFriendsListScreen(qtFriends);

    handleFriendsRequests(); // Rafraîchir aussi les demandes
    stackedWidget->setCurrentWidget(friendsListScreen);
    

}


/**
 * @brief refuser demande d ami.
 * 
 * Le bouton qui a déclenché est recupéré,
 * on recupère le username de la personne qui a envoyé la requete,
 * la requête est supprimée de la db
 * l'affichage des friends requests est mis à jour.
 */
void FriendsHandler::refuseFriendRequest() {
    
    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button){
        return;  // Si il n est pas valide, on stop
    }   
    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());     // retire la requete qui a été traitée
    QMessageBox::information(this, "Request Denied", "you have denied the the friend request !");
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
}



QWidget* FriendsHandler::getWindowWidget() const{
    return window;
}















