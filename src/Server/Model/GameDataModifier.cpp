#include "GameDataModifier.hpp"

GameDataModifier::GameDataModifier(const std::shared_ptr<GameData> gameData_) : gameData{gameData_} {}

int GameDataModifier::getTimeInterval() {
  return timeInterval;
}



/**
 *@brief mise à jour de l'intervalle de temps les mouvements de pièces, en fonction du niveau.
 */
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



enum Line {
  LINE_1,
  LINE_2,
  LINE_3,
  LINE_4  // automatically equal to the number of lines
};

/**
 *@brief ajoute un score en fonction du nombre de lignes supprimées.
 *
 *@param lineNumber nombre de lignes complétées.
 */
void GameDataModifier::addScoreFromLines(int lineNumber){
  int scoreFactorLine_1 = 40;
  int scoreFactorLine_2 = 100;
  int scoreFactorLine_3 = 300;
  int scoreFactorLine_4 = 1200;

  switch (static_cast<Line>(lineNumber)) {
    // using nintendo scoring system https://tetris.wiki/Scoring
    case LINE_1:
      gameData->set_score(scoreFactorLine_1 * (level +1));
    break;
    case LINE_2:
      gameData->set_score(scoreFactorLine_2 * (level +1));
    break;
    case LINE_3:
      gameData->set_score(scoreFactorLine_3 * (level +1));
    break;
    case LINE_4:
      gameData->set_score(scoreFactorLine_4 * (level +1));
    break;
  }
}


/**
 *@brief permet de passer au niveau suppérieur
 */
void GameDataModifier::levelUp() {
  if(gameData->get_numberBrokenLines() >= LINES_FOR_LEVEL_UP){
    level++;
    updateTimeInterval();
  }
}


/**
 *@brief enregistre et met à jour le nombre de lignes complétées lors de l'action.
 *
 *@param lineNumber nombre de lignes complétées en une action
 */
void GameDataModifier::registerLines(int lineNumber) {
  addScoreFromLines(lineNumber);
  gameData->updateLines(lineNumber);
  levelUp();
}