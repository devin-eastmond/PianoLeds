#include "key_states.h"
#include <thread>
#include "midi_receiver.h"
#include "led_controller.h"

int main() {
  KeyStates keyStates;
  std::thread midiThread(midiReceiver, std::ref(keyStates));
  std::thread ledThread(ledController, std::ref(keyStates));
  midiThread.join();
  ledThread.join();
  return 0;
}