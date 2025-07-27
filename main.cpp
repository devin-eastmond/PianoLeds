#include "key_states.h"
#include <thread>
#include "midi_receiver.h"
#include "led_controller.h"
#include "modes.h"

int main() {
  KeyStates keyStates;
  ModeManager modeManager;
  std::thread midiThread(midiReceiver, std::ref(keyStates), std::ref(modeManager));
  std::thread ledThread(ledController, std::ref(keyStates), std::ref(modeManager));
  midiThread.join();
  ledThread.join();
  return 0;
}