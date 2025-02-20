#include "gamedatamodifier.hpp"

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


/**
 *@brief ajoute un score en fonction du nombre de lignes supprimées.

 *@param lineNumber nombre de lignes complétées.
 */
void GameDataModifier::addScoreFromLines(int lineNumber){
  switch (lineNumber) {
    // using nintendo scoring system https://tetris.wiki/Scoring
    case 1:
      gameData->set_score(40 * (level +1));
      break;
    case 2:
      gameData->set_score(100 * (level +1));
      break;
    case 3:
      gameData->set_score(300 * (level +1));
      break;
    case 4:
      gameData->set_score(1200 * (level +1));
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

 *@param lineNumber nombre de lignes complétées en une action
 */
void GameDataModifier::registerLines(int lineNumber) {
  addScoreFromLines(lineNumber);
  gameData->updateLines(lineNumber);
  levelUp();
}