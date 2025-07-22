#pragma once
#include <vector>
#include <mutex>

enum State { OFF, ON };

class KeyStates {
private:
    std::vector<State> keys;
    std::mutex mtx;

public:
    KeyStates();
    void setKey(int index, State state);
    State getKey(int index);
    std::vector<State> getAllKeys();
};
