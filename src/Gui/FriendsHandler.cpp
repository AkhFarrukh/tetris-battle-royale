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

FriendsHandler::~FriendsHandler() {}



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



void FriendsHandler::setupFriendsListScreen(const std::vector<QString>& friendsList) {
    friendsListScreen = new QWidget();  //widget pour ecran qui affichera liste d amis
    QVBoxLayout* layout = new QVBoxLayout(friendsListScreen);

    // Titre
    QLabel* titleLabel = new QLabel("Friends list:");
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // Tableau des amis (existant)
    friendsTableWidget = new QTableWidget();
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
    addFriendButton = new QPushButton("Add new friend");
    friendRequestsButton = new QPushButton(QString("Friend requests (%1)").arg(countRequests));
    QPushButton* goBackButton = new QPushButton("Back to Menu");
    //goBackButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
    
    layout->addWidget(addFriendButton);
    layout->addWidget(friendRequestsButton);
    //layout->addWidget(goBackButton);

    // Connexions des boutons pour qu'ils realisent action demandée qd on click sur eux
    connect(addFriendButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(addFriendScreen);
    });
    
    connect(friendRequestsButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(requestsScreen);
        
        handleFriendsRequests();
    });

    /** 
    connect(goBackButton, &QPushButton::clicked, [this]() {
        // Trouve le MainWindow parent en remontant
        QWidget* parent = this;
        while (parent && !qobject_cast<MainWindow*>(parent)) {
            parent = parent->parentWidget();
        }
        
        if (parent) {
            QMetaObject::invokeMethod(parent, "showMainMenuScreen");
        }
    });
    */
    friendsListScreen->setLayout(layout);
}



void FriendsHandler::setupAddFriendScreen() {
    addFriendScreen = new QWidget();   // nouveau widget
    QVBoxLayout* layout = new QVBoxLayout(addFriendScreen);

    QLabel* title = new QLabel("Add New Friend");  //label pour afficher sous titre
    QFont titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    layout->addWidget(title);

    usernameInput = new QLineEdit();   // créé champs pour entrer username
    usernameInput->setPlaceholderText("Enter player username");
    layout->addWidget(usernameInput);

    //label ppur affiche informations (messages)
    infoLabel = new QLabel();
    infoLabel->setStyleSheet("color: green; font-weight: bold;");
    layout->addWidget(infoLabel);

    //boutons
    QPushButton* searchButton = new QPushButton("Search");
    QPushButton* backButton = new QPushButton("Back");
    
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


void FriendsHandler::showFriendsRequests(const std::vector<string>& requestsReceivedList, const std::vector<string>& requestSentList) {

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(requestsScreen->layout());

    if(!layout){
        layout = new QVBoxLayout(requestsScreen);
        requestsScreen->setLayout(layout);
    }

    // Supprimer les widgets sauf les 2 premiers (titre et bouton back)
    while (layout->count() > 1) {
        QLayoutItem* item = layout->takeAt(1); // Prendre widget à partir de l'index 2
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item; //supprime elem de layout
    }

    // Créé la table pour pouvoir afficher demandes d amiis
    QTableWidget* requestsTable = new QTableWidget(requestsReceivedList.size() + requestSentList.size(), 3);
    requestsTable->horizontalHeader()->setVisible(false);
    requestsTable->horizontalHeader()->setStretchLastSection(true);

    int row = 0;  // ligne courante , incrementée apres

    // Requêtes reçues
    for (const auto& request_received : requestsReceivedList) {
        QString requestReceived_name = QString::fromStdString(request_received);    // conversion nom (username) en qtstring pour pouvoir l'utiliser
        requestsTable->setItem(row, 0, new QTableWidgetItem(requestReceived_name));
        requestsTable->setItem(row, 1, new QTableWidgetItem("Received"));         // indique que c'est une requete qu on a recue

        // Widget avec boutons Accept/Refuse
        QWidget* actionWidget = new QWidget;
        QPushButton* acceptButton = new QPushButton("Accept");
        QPushButton* refuseButton = new QPushButton("Refuse");

        // pour placer boutons accept/refuse de maniere horizontale
        QHBoxLayout* hLayout = new QHBoxLayout(actionWidget);
        hLayout->addWidget(acceptButton);
        hLayout->addWidget(refuseButton);
        actionWidget->setLayout(hLayout);

        requestsTable->setCellWidget(row, 2, actionWidget);

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
        QPushButton* cancelButton = new QPushButton("Cancel");
        cancelButton->setProperty("username", requestSentTo);
        cancelButton->setProperty("type", "sent");

        connect(cancelButton, &QPushButton::clicked, this, &FriendsHandler::refuseFriendRequest);
        requestsTable->setCellWidget(row, 2, cancelButton);

        row++;
    }

    layout->insertWidget(1, requestsTable);

    // va redimensionner colonnes
    requestsTable->setColumnWidth(0, 150); // Colonne nom
    requestsTable->setColumnWidth(1, 80);  // Colonne (received / sent)

    bool hasButtonBack = false;
    for (int i = 0; i < layout->count(); ++i) {
        if (qobject_cast<QPushButton*>(layout->itemAt(i)->widget()) && 
            layout->itemAt(i)->widget()->objectName() == "backButton") {
            hasButtonBack = true;
            break;
        }
    }

    if(!hasButtonBack){
        QPushButton* backButton = new QPushButton("Back");
        backButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
        layout->addWidget(backButton);

        connect(backButton, &QPushButton::clicked, [this]() {
            stackedWidget->setCurrentWidget(friendsListScreen);
        });
    }

   
   
}




void FriendsHandler::setupRequestsScreen() {
    requestsScreen = new QWidget();  // pour l ecran des demandes d'amis
    QVBoxLayout* layout = new QVBoxLayout(requestsScreen);

    QLabel* title = new QLabel("Friend Requests");
    QFont titleFont = title->font();
    titleFont.setBold(true);     // active le gras
    titleFont.setPointSize(14);
    title->setFont(titleFont);
    layout->addWidget(title);

    /** 
    QPushButton* backButton = new QPushButton("Back");   // pour revenir en arriere
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(friendsListScreen);    // revenir à l'ecran de la liste d amis
    });
    */
    requestsScreen->setLayout(layout);
}



void FriendsHandler::acceptFriendRequest() {

    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;  // Si il n est pas valide, on stop

    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    db.addFriend(currentUser.toStdString(), requesterPlayer.toStdString());           // ajout dans la db
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());  // retire la requete qui a été traitée

    QMessageBox::information(this, "Request Accepted", "you have accepted the friend request !");

    handleFriendsRequests();
}


void FriendsHandler::refuseFriendRequest() {
    
    // Récupère le bouton ayant émis le signal
    auto* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;  // Si il n est pas valide, on stop
    
    QString requesterPlayer = button->property("username").toString();   // recupere le nom de la personne qui a envoyé requete
    
    db.deleteRequestFriend(currentUser.toStdString(), requesterPlayer.toStdString());                // retire la requete qui a été traitée
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















