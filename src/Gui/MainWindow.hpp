#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void onButtonClicked();

private:
    QLabel* helloLabel;
    void setupUI();
};

#endif // MAINWINDOW_HPP
