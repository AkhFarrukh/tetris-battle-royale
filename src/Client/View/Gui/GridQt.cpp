#include "GridQt.hpp"
#include <QPainter>
#include <QKeyEvent>
#include "Input.hpp"

GridQt::GridQt(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(800, 600);
    setFocusPolicy(Qt::StrongFocus); // Enable keyboard focus
}

void GridQt::renderGames(const std::vector<NetPlayerData>& players) {
    if (players.empty()) {
        return;    
    } else {
        currentPlayers = players;
    }
    update();
}

void GridQt::keyPressEvent(QKeyEvent* event) {
    KeyboardAction action = convertKeyToAction(event->key());
    if (action != KeyboardAction::NO_ACTION) {
        // Send the action to the server
        int type = MessageType::T_Input;
        int size = sizeof(InputMessage);
        InputMessage inputMessage;
        inputMessage.input = action;
        if (!sendAll(threadData->socket, &type, sizeof(int))) { flag = true; return; }
        if (!sendAll(threadData->socket, &size, sizeof(int))) { flag = true; return; }
        if (!sendAll(threadData->socket, &inputMessage, sizeof(InputMessage))) { flag = true; return; }
    }
    QWidget::keyPressEvent(event);
}

KeyboardAction GridQt::convertKeyToAction(int key) {
    switch (key) {
        case Qt::Key_Left:
        case Qt::Key_Q:
            return KeyboardAction::LEFT_DIRECTION_KEY;
        case Qt::Key_Right:
        case Qt::Key_D:
            return KeyboardAction::RIGHT_DIRECTION_KEY;
        case Qt::Key_Up:
        case Qt::Key_Z:
            return KeyboardAction::LEFT_ROTATION_KEY;
        case Qt::Key_R:
            return KeyboardAction::RIGHT_ROTATION_KEY;
        case Qt::Key_Down:
        case Qt::Key_S:
            return KeyboardAction::SOFT_DROP_KEY;
        case Qt::Key_Space:
            return KeyboardAction::HARD_DROP_KEY;
        case Qt::Key_Escape:
            return KeyboardAction::EXIT_KEY;
        default:
            return KeyboardAction::NO_ACTION;
    }
}

void GridQt::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    const int blockSize = 30; // Increased block size for better visibility
    const int gridWidth = 10;
    const int gridHeight = 20;
    const int spacing = 2; // Space between blocks
    QPoint offset(20, 20);

    // Set up the painter
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 1));
    bool isRoyal = false;

    for (const auto& player : currentPlayers) {
        if (player.playerId == -1 && threadData->gameMode == Modes::Royal_) {
            isRoyal = true;
            break;
        }
    }

    for (size_t i = 0; i < currentPlayers.size(); ++i) {
        const auto& player = currentPlayers[i];
        QPoint playerOffset = offset + QPoint(static_cast<int>(i * (gridWidth + 2) * (blockSize + spacing)), 0);

        // Draw player score
        painter.setPen(Qt::white);
        QFont scoreFont("Arial", 12, QFont::Bold);
        painter.setFont(scoreFont);
        painter.drawText(playerOffset + QPoint(0, -10), QString("Score: %1").arg(player.score));
        

        // Draw grid border
        QRect gridRect(playerOffset, QSize(gridWidth * (blockSize + spacing), gridHeight * (blockSize + spacing)));
        painter.drawRect(gridRect);

        // Draw grid lines
        painter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
        for (int x = 1; x < gridWidth; ++x) {
            painter.drawLine(
                playerOffset.x() + x * (blockSize + spacing),
                playerOffset.y(),
                playerOffset.x() + x * (blockSize + spacing),
                playerOffset.y() + gridHeight * (blockSize + spacing)
            );
        }
        for (int y = 1; y < gridHeight; ++y) {
            painter.drawLine(
                playerOffset.x(),
                playerOffset.y() + y * (blockSize + spacing),
                playerOffset.x() + gridWidth * (blockSize + spacing),
                playerOffset.y() + y * (blockSize + spacing)
            );
        }

        // Draw blocks
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                int index = y * gridWidth + x;
                if (index >= static_cast<int>(player.grid.size())) continue;

                Color color = player.grid[index];
                if (color == Color::nullcolor) continue;

                QRect blockRect(
                    playerOffset.x() + x * (blockSize + spacing),
                    playerOffset.y() + y * (blockSize + spacing),
                    blockSize,
                    blockSize
                );

                // Draw block with shadow effect
                painter.setPen(Qt::black);
                painter.setBrush(qtColorFromEnum(color));
                painter.drawRect(blockRect);

                // Add a subtle highlight to make blocks more 3D
                painter.setPen(QPen(QColor(255, 255, 255, 50), 1));
                painter.drawLine(blockRect.topLeft(), blockRect.topRight());
                painter.drawLine(blockRect.topLeft(), blockRect.bottomLeft());
            }
        }

        if (isRoyal) {
            int menuX = width() - 250;
            int menuY = 40;
            QFont energyFont("Arial", 12, QFont::Bold);
            painter.setFont(energyFont);
            painter.setPen(Qt::yellow);
    
            for (const auto& player : currentPlayers) {
                if (player.playerId == -1) {
                    painter.drawText(menuX, menuY, QString("Energy: %1").arg(player.nrg));
                    break;
                }
            }
        }
        
    
        if (player.endGame || player.isGameOver) {
            QFont gameOverFont("Arial", 16, QFont::Bold);
            painter.setFont(gameOverFont);
            painter.setPen(Qt::red);
            QString message = player.isGameOver ? "GAME OVER" : "YOU WIN!";
            painter.drawText(
                gridRect.center() - QPoint(50, 0),
                message
            );
        }
        if (player.playerId != -1){
            painter.drawText(playerOffset + QPoint(0, 10), QString("Player id: %1").arg(player.playerId));
        }else{
            painter.drawText(playerOffset + QPoint(0, 10), QString("YOU"));
        }
    }
}

QColor GridQt::qtColorFromEnum(Color color) {
    switch (color) {
        case Color::red: return QColor(255, 0, 0);
        case Color::green: return QColor(0, 255, 0);
        case Color::yellow: return QColor(255, 255, 0);
        case Color::blue: return QColor(0, 0, 255);
        case Color::orange: return QColor(255, 165, 0);
        case Color::cyan: return QColor(0, 255, 255);
        case Color::purple: return QColor(128, 0, 128);
        case Color::grey: return QColor(128, 128, 128);
        case Color::white: return QColor(255, 255, 255);
        default: return QColor(0, 0, 0);
    }
}
