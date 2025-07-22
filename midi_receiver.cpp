#include "midi_receiver.h"
#include <iostream>
#include <alsa/asoundlib.h>

void midiReceiver(KeyStates& keyStates) {
  snd_seq_t *seq_handle;
  int in_port;
  
  // Open sequencer
  if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
      std::cerr << "Error opening ALSA sequencer." << std::endl;
      return;
  }

  snd_seq_set_client_name(seq_handle, "MIDI Listener");

  // Create input port
  in_port = snd_seq_create_simple_port(seq_handle, "Input Port",
                                        SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
                                        SND_SEQ_PORT_TYPE_APPLICATION);

  if (in_port < 0) {
      std::cerr << "Error creating sequencer port." << std::endl;
      return;
  }

  std::cout << "Waiting for MIDI input..." << std::endl;

  snd_seq_event_t *ev;

  while (true) {
      snd_seq_event_input(seq_handle, &ev);

      if (ev->type == SND_SEQ_EVENT_NOTEON || ev->type == SND_SEQ_EVENT_NOTEOFF) {
          // std::cout << (ev->type == SND_SEQ_EVENT_NOTEON ? "Note On: " : "Note Off: ")
          //           << "Channel " << (int)ev->data.note.channel
          //           << ", Note " << (int)ev->data.note.note
          //           << ", Velocity " << (int)ev->data.note.velocity << std::endl;

          State state = ev->data.note.velocity > 0 ? ON : OFF;
          keyStates.setKey(ev->data.note.note - 21, state);
      }

      snd_seq_free_event(ev);
  }

  snd_seq_close(seq_handle);
  return;
}