#include "MainWindow.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);

    helloLabel = new QLabel("first menu should be here", this);
    helloLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(helloLabel);
    setCentralWidget(centralWidget);

    setWindowTitle("Tetris Qt GUI");
    resize(400, 200);
}

MainWindow::~MainWindow() {}