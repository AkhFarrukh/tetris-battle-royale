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

    // Liste d'amis statique pour test
    QStringList friends = {"Alice", "Bob", "Charlie"};
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
    QWidget *chatScreen = new QWidget(this);
    auto layout = new QVBoxLayout(chatScreen);

    QLabel *chatTitle = new QLabel("Chat with " + friendName, this);
    chatTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(chatTitle);

    QTextEdit *chatHistory = new QTextEdit(this);
    chatHistory->setReadOnly(true);
    layout->addWidget(chatHistory);

    QLineEdit *messageInput = new QLineEdit(this);
    layout->addWidget(messageInput);

    QPushButton *sendButton = new QPushButton("Send", this);
    layout->addWidget(sendButton);
    connect(sendButton, &QPushButton::clicked, this, [=]() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        chatHistory->append("<b>You:</b> " + message);
        messageInput->clear();
    }
    });

    connect(messageInput, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

    QPushButton *backButton = new QPushButton("Back to friends list", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(friendsListScreen);
    });

    // Tu peux connecter sendButton à une future fonction pour gérer l'envoi

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