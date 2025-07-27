#ifndef MODES_H
#define MODES_H

#include <variant>
#include <iostream>
#include <vector>
#include <map>

enum class ModeType {
    PLAY,
    SELECT_COLORS,
    SELECT_PRESSED_COLOR,
    SELECT_RELEASED_COLOR,
    EDIT_COLORS,
    ADD_COLOR,
    DELETE_COLOR
};

using PressedKeys = std::vector<int>;
using PressedKeysCombination = std::vector<PressedKeys>;
using PressedKeysCombinationsList = std::vector<PressedKeysCombination>;

using ModeCombinationsMap = std::map<ModeType, PressedKeysCombinationsList>;

class ModeManager {
private:
  ModeType currentMode;
  PressedKeysCombination currentCombination;
  PressedKeys currentPressedKeys;
  PressedKeys currentKeys;
  ModeType checkModeTransition(const ModeCombinationsMap& combinationsMap);

public:
  ModeManager();
  void setMode(ModeType mode);
  ModeType getMode();
  void addPressedKeys(PressedKeys keys);
  void onKeyPressed(int key);
  void onKeyReleased(int key);
};

#endif // MODES_H
