#include "modes.h"
#include <algorithm>
#include <iostream>

ModeManager::ModeManager() : currentMode(ModeType::PLAY) {}

constexpr size_t MAX_COMBINATION_LENGTH = 10;

const ModeCombinationsMap playModeCombinations = {
  {ModeType::SELECT_COLORS, {{{65, 67}, {77, 79}, {70, 72}}}},
  {ModeType::EDIT_COLORS, {{{2, 3}, {4, 5}, {6, 7}}}}
};

const ModeCombinationsMap selectColorsModeCombinations = {
  {ModeType::SELECT_RELEASED_COLOR, {
    {{0}}, {{1}}, {{2}}, {{3}}, {{4}}, {{5}}, {{6}}, {{7}}, {{8}}, {{9}}, {{10}}, 
    {{11}}, {{12}}, {{13}}, {{14}}, {{15}}, {{16}}, {{17}}, {{18}}, {{19}}, {{20}}, {{21}}, 
    {{22}}, {{23}}, {{24}}, {{25}}, {{26}}, {{27}}, {{28}}, {{29}}, {{30}}, {{31}}, {{32}}, 
    {{33}}, {{34}}, {{35}}, {{36}}, {{37}}, {{38}}, {{39}}, {{40}}, {{41}}, {{42}}, {{43}}
  }},
  {ModeType::SELECT_PRESSED_COLOR, {
    {{44}}, {{45}}, {{46}}, {{47}}, {{48}}, {{49}}, {{50}}, {{51}}, {{52}}, {{53}}, {{54}}, 
    {{55}}, {{56}}, {{57}}, {{58}}, {{59}}, {{60}}, {{61}}, {{62}}, {{63}}, {{64}}, {{65}}, 
    {{66}}, {{67}}, {{68}}, {{69}}, {{70}}, {{71}}, {{72}}, {{73}}, {{74}}, {{75}}, {{76}}, 
    {{77}}, {{78}}, {{79}}, {{80}}, {{81}}, {{82}}, {{83}}, {{84}}, {{85}}, {{86}}, {{87}}
  }}
};

const ModeCombinationsMap editColorsModeCombinations = {
  {ModeType::ADD_COLOR, {{{4, 5}, {6, 7}}}},
  {ModeType::DELETE_COLOR, {{{8, 9}, {10, 11}}}},
};

ModeType ModeManager::checkModeTransition(const ModeCombinationsMap& combinationsMap) {
  for (const auto& [mode, combosList] : combinationsMap) {
    for (const auto& combo : combosList) {
      if (currentCombination.size() < combo.size()) continue;

      // Start index in currentCombination where combo must match
      size_t startIndex = currentCombination.size() - combo.size();

      bool match = true;
      for (size_t i = 0; i < combo.size(); ++i) {
        if (currentCombination[startIndex + i] != combo[i]) {
          match = false;
          break;
        }
      }

      if (match) {
        currentCombination.clear();  // Reset after transition
        return mode;
      }
    }
  }
  return currentMode; // No transition
}



void ModeManager::setMode(ModeType mode) {
  currentMode = mode;
}

ModeType ModeManager::getMode() {
  return currentMode;
}

void ModeManager::addPressedKeys(PressedKeys keys) {
  std::sort(keys.begin(), keys.end());  // Sort keys from smallest to biggest
  currentCombination.push_back(keys);

  // Keep only the most recent N entries
  if (currentCombination.size() > MAX_COMBINATION_LENGTH) {
    currentCombination.erase(currentCombination.begin());  // Remove oldest
  }

  ModeType newMode = currentMode;
  switch (currentMode) {
    case ModeType::PLAY:
      newMode = checkModeTransition(playModeCombinations);
      break;
    case ModeType::SELECT_COLORS:
      newMode = checkModeTransition(selectColorsModeCombinations);
      break;
    case ModeType::EDIT_COLORS:
      newMode = checkModeTransition(editColorsModeCombinations);
      break;
    default:
      break;
  }

  setMode(newMode);
}

void ModeManager::onKeyPressed(int key) {
  currentPressedKeys.push_back(key);
  currentKeys.push_back(key);
}

void ModeManager::onKeyReleased(int key) {
  // remove key from currentPressedKeys
  currentPressedKeys.erase(std::remove(currentPressedKeys.begin(), currentPressedKeys.end(), key), currentPressedKeys.end());
  if (currentPressedKeys.empty()) {
    // add currentKeys to currentCombination using addPressedKeys
    addPressedKeys(currentKeys);
    currentKeys.clear();
  }
}
