#ifdef scroll
#undef scroll
#endif

#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>
#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    auto centralWidget = new QWidget(this);
    auto layout = new QVBoxLayout(centralWidget);

    // Créer un bouton
    QPushButton *button = new QPushButton("Clique-moi", this);

    // Connecter le bouton à une action
    connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

    layout->addWidget(button);
    setCentralWidget(centralWidget);
}

// Fonction déclenchée au clic
void MainWindow::onButtonClicked() {
    qDebug() << "Bouton cliqué !";
}