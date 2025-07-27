#include "key_states.h"
#include <iostream>

const int NUM_KEYS = 88;

KeyStates::KeyStates() : keys(NUM_KEYS, OFF) {}

void KeyStates::setKey(int index, State state) {
    std::lock_guard<std::mutex> lock(mtx);
    if (index >= 0 && index < NUM_KEYS) {
        keys[index] = state;
    }
}

State KeyStates::getKey(int index) {
    std::lock_guard<std::mutex> lock(mtx);
    return (index >= 0 && static_cast<size_t>(index) < keys.size()) ? keys[index] : OFF;
}

std::vector<State> KeyStates::getAllKeys() {
    std::lock_guard<std::mutex> lock(mtx);
    return keys;
}
