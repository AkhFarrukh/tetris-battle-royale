#ifdef scroll
#undef scroll
#endif

#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include "MainWindow.hpp"

void MainWindow::setupFriendsListScreen() {
    friendsListScreen = new QWidget(this);
    auto layout = new QVBoxLayout(friendsListScreen);

    QLabel *titleLabel = new QLabel("Friends list", this);
    layout->addWidget(titleLabel);

    QListWidget *friendsList = new QListWidget(this);

    std::vector<std::string> friendsVec = udm.friendsList(username);

    QStringList friends;
    for (const auto& name : friendsVec) {
        friends << QString::fromStdString(name);
    }
    friendsList->addItems(friends);
    if (friends.isEmpty()) {
        QLabel *noFriendsLabel = new QLabel("You don't have any friends yet.", this);
        layout->addWidget(noFriendsLabel);
    }
    friendsList->setStyleSheet("font-size: 16px;");
    layout->addWidget(friendsList);

    connect(friendsList, &QListWidget::itemActivated, this, [this](QListWidgetItem *item) {
        QString friendName = item->text();
        openChatWith(friendName);
    });

    QPushButton *backButton = new QPushButton("Return", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, this, [this]() {
    if (callerWindow) {
        stackedWidget->setCurrentWidget(callerWindow);
    } else {
        QApplication::quit();
    }
});

    friendsListScreen->setLayout(layout);
}

void MainWindow::openChatWith(const QString& friendName) {
    int friendId_ = udm.getUserId(friendName.toStdString().c_str());

    udm.conversationOpen(userId_, friendId_);
    vector<std::string> messagesNoRead = udm.getMessages(friendId_, userId_);
    udm.deleteMessages(friendId_, userId_);
    QWidget *chatScreen = new QWidget(this);
    auto layout = new QVBoxLayout(chatScreen);

    QLabel *chatTitle = new QLabel("Chat with " + friendName, this);
    chatTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(chatTitle);

    chatHistory = new QTextEdit(this);
    chatHistory->setReadOnly(true);
    layout->addWidget(chatHistory);

    for (const auto& msg : messagesNoRead) {
        chatHistory->append("<b>" + friendName + ": </b>" + QString::fromStdString(msg));
    }

    QLineEdit *messageInput = new QLineEdit(this);
    layout->addWidget(messageInput);

    QPushButton *sendButton = new QPushButton("Send", this);
    layout->addWidget(sendButton);
    connect(sendButton, &QPushButton::clicked, this, [=]() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        chatHistory->append("<b>You:</b> " + message);
        if(udm.getInConversation(userId_, friendId_) == 1){
            int messageSent = MessageType::T_Message;
            int size_ = sizeof(struct Message);
            Message messageToSent;
            std::strncpy(messageToSent.expediteur, username.c_str(), sizeof(messageToSent.expediteur) - 1);
            messageToSent.expediteur[sizeof(messageToSent.expediteur) - 1] =  '\0';
            std::strncpy(messageToSent.message, message.toStdString().c_str(), sizeof(messageToSent.message) - 1);
            messageToSent.message[sizeof(messageToSent.message) - 1] =  '\0';
            std::strncpy(messageToSent.destinataire, friendName.toStdString().c_str(), sizeof(messageToSent.destinataire) - 1);
            messageToSent.destinataire[sizeof(messageToSent.destinataire) - 1] =  '\0';
            if(!sendAll(sock, &messageSent, sizeof(int)))return;
            if(!sendAll(sock, &size_, sizeof(int)))return;
            if(!sendAll(sock, &messageToSent, sizeof(Message)))return;
        }
        else udm.addMessage(userId_, friendId_, message.toStdString());
        messageInput->clear();
    }
    });

    connect(messageInput, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

    QPushButton *backButton = new QPushButton("Back to friends list", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, this, [this, friendId_]() {
        udm.conversationClose(userId_, friendId_);
        stackedWidget->setCurrentWidget(friendsListScreen);
    });

    chatScreen->setLayout(layout);
    stackedWidget->addWidget(chatScreen);
    stackedWidget->setCurrentWidget(chatScreen);
}

void MainWindow::showfriendsListScreen() {
    stackedWidget->setCurrentWidget(friendsListScreen);
}

void MainWindow::showChatScreen() {
    stackedWidget->setCurrentWidget(chatScreen);
}