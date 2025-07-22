#include "led_controller.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include "ws2811.h"

#define LED_COUNT 176
#define GPIO_PIN 18
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GRB

ws2811_t ledstring =
{
    .freq = WS2811_TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .invert = 0,
            .count = LED_COUNT,
            .strip_type = STRIP_TYPE,
            .brightness = 255,
        },
        [1] = { 0 }
    },
};

void ledController(KeyStates& keyStates) {
    std::vector<State> previousStates = keyStates.getAllKeys();

    ws2811_init(&ledstring);

    while (true) {
        std::vector<State> currentStates = keyStates.getAllKeys();
        bool changed = false;

        for (int i = 0; i < currentStates.size(); ++i) {
            if (i >= LED_COUNT) break; // avoid overflow
            if (previousStates[i] != currentStates[i]) {
                ledstring.channel[0].leds[i * 2] = (currentStates[i] == ON)
                    ? 0x00660066  // Purple-ish
                    : 0x00000000;
                ledstring.channel[0].leds[i * 2 + 1] = (currentStates[i] == ON)
                    ? 0x00660066  // Purple-ish
                    : 0x00000000;
                changed = true;
            }
        }

        if (changed) {
            ws2811_render(&ledstring);
            previousStates = currentStates;
        }

        usleep(10000); // avoid busy loop
    }

    ws2811_fini(&ledstring);
}
