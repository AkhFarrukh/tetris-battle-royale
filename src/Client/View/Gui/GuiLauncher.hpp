#pragma once
#ifdef border
#undef border
#endif


#ifdef scroll
#undef scroll
#endif

#include "PlayerFunctions.hpp"
#include "MainWindow.hpp"

int runGuiApp(int argc, char** argv, int sock, std::shared_ptr<Player::ThreadData> threadData);