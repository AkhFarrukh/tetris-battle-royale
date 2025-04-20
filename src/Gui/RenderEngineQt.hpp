#pragma once
#include <QWidget>
#include <vector>
#include "playerData.hpp"
#include "keyboardAction.hpp"
#include "playerFunctions.hpp"

class RenderEngineQt : public QWidget {
    Q_OBJECT

public:
    explicit RenderEngineQt(QWidget* parent = nullptr);
    void renderGames(const std::vector<NetPlayerData>& players);
    void setThreadData(std::shared_ptr<Player::ThreadData> data) { threadData = data; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    std::vector<NetPlayerData> currentPlayers;
    QColor qtColorFromEnum(Color color);
    KeyboardAction convertKeyToAction(int key);
    std::shared_ptr<Player::ThreadData> threadData;
    bool flag = false;
};
