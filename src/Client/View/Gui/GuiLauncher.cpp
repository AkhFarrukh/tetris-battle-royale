#ifdef border
#undef border
#endif

#ifdef scroll
#undef scroll
#endif

#include <QApplication>
#include "GuiLauncher.hpp"

int runGuiApp(int argc, char** argv, int sock, std::shared_ptr<Player::ThreadData> threadData) {
    QApplication app(argc, argv);
    MainWindow w(nullptr, sock, threadData);
    threadData->qtParent = &w;
    w.show();
    return app.exec();
}