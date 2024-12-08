//
// Created by farrukh on 8/12/24.
//
#include <memory>
#include <cmath>

#include "game_data.hpp"

#define LINES_FOR_LEVEL_UP 10
#define BASE_TIME_INTERVAL 800
#define LEVEL_DECREMENT_TIME 7


class GameDataModifier {
private:
    std::shared_ptr<GameData> gameData_;
    int timeInterval = BASE_TIME_INTERVAL; //milliseconds
    int level = 1;

    void levelUp();
    void addScoreFromLines(int lineNumber);
    void updateTimeInterval();


public:
    GameDataModifier(const GameData& gameData);
    void registerLines(int lineNumber);
    int getTimeInterval();
};




GameDataModifier::GameDataModifier(std::shared_ptr<GameData> gameData)
  : gameData_{gameData} {
}

int GameDataModifier::getTimeInterval() {
  return timeInterval;
}


void GameDataModifier::updateTimeInterval() {
  //Time = (0.8-((Level-1)*0.007))^(Level-1)
  //https://tetris.wiki/Marathon
  timeInterval = static_cast<int>(
          std::pow(
                  (BASE_TIME_INTERVAL - ((level - 1) * LEVEL_DECREMENT_TIME)) / 1000.0,
                  level - 1
                  )
          );
}


void GameDataModifier::addScoreFromLines(int lineNumber){
  switch (lineNumber) {
    // using nintendo scoring system https://tetris.wiki/Scoring
    case 1:
      gameData_->set_score(40 * (level +1));
      break;
    case 2:
      gameData_->set_score(100 * (level +1));
      break;
    case 3:
      gameData_->set_score(300 * (level +1));
      break;
    case 4:
      gameData_->set_score(1200 * (level +1));
      break;
  }
}

void GameDataModifier::levelUp() {
  if((gameData_->get_numberBrokenLines() % LINES_FOR_LEVEL_UP)== 0){
    level++;
    updateTimeInterval();
  }
}



void GameDataModifier::registerLines(int lineNumber) {
  addScoreFromLines(lineNumber);
  gameData_->set_numberBrokenLines(
          gameData_->get_numberBrokenLines() + lineNumber);
  levelUp();
}