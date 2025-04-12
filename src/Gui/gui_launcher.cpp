#ifdef scroll
#undef scroll
#endif

#include <QApplication>
#include "MainWindow.hpp"

int runGuiApp(int argc, char** argv, int sock) {
    QApplication app(argc, argv);
    MainWindow window(nullptr, sock);
    window.show();
    return app.exec();
}
