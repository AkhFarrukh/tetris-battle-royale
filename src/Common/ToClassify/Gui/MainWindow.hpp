#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        void showLoginScreen(); // Affiche la page de connexion
    void showAuthScreen();  // Affiche la page Username/Password

private:
    void setupUI();
    void setupLoginScreen();
    void setupAuthScreen();

    QStackedWidget *stackedWidget;

    // Widgets de la page Login/Register
    QWidget *loginScreen;

    // Widgets de la page Username/Password
    QWidget *authScreen;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
};

#endif // MAINWINDOW_HPP
