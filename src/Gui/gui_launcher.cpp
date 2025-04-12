#ifdef scroll
#undef scroll
#endif

#include <QApplication>
#include "MainWindow.hpp"

int runGuiApp(int argc, char** argv, int sock) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
