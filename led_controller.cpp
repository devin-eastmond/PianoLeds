#include "led_controller.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <chrono>
#include "ws2811.h"

#define LED_COUNT 176
#define GPIO_PIN 18
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GRB
#define TRANSITION_TIME 0.15f  // seconds
#define POWER 2.0f             // Easing curve exponent

const uint32_t PRESSED_COLOR  = 0x880088; // Purple
const uint32_t RELEASED_COLOR = 0x001417; // Dark Blue

uint8_t getRed(uint32_t color)   { return (color >> 16) & 0xFF; }
uint8_t getGreen(uint32_t color) { return (color >> 8) & 0xFF; }
uint8_t getBlue(uint32_t color)  { return color & 0xFF; }

ws2811_t ledstring = {
    .freq = WS2811_TARGET_FREQ,
    .dmanum = DMA,
    .channel = {
        [0] = {
            .gpionum = GPIO_PIN,
            .invert = 0,
            .count = LED_COUNT,
            .strip_type = STRIP_TYPE,
            .brightness = 255,
        },
        [1] = {0}
    }
};

// Get current time in milliseconds
uint64_t millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

struct FadeState {
    bool transitioning = false;
    uint64_t transitionStart = 0;
};

void setLeds(int keyIndex, uint32_t color) {
    int ledA = keyIndex * 2;
    int ledB = keyIndex * 2 + 1;
    ledstring.channel[0].leds[ledA] = color;
    ledstring.channel[0].leds[ledB] = color;
}

void ledController(KeyStates& keyStates) {
    std::vector<State> prevState = keyStates.getAllKeys();
    std::vector<FadeState> fadeStates(LED_COUNT);

    if (ws2811_init(&ledstring) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed\n");
        return;
    }

    while (true) {
        std::vector<State> currentState = keyStates.getAllKeys();
        uint64_t now = millis();
        bool anyChange = false;

        for (int i = 0; i < std::min((int)currentState.size(), LED_COUNT); ++i) {
            State prev = prevState[i];
            State curr = currentState[i];

            // Key pressed → light up immediately
            if (curr == ON && prev == OFF) {
                setLeds(i, PRESSED_COLOR);
                fadeStates[i].transitioning = false;
                anyChange = true;
            }

            // Key released → begin fading to RELEASED_COLOR
            else if (curr == OFF && prev == ON) {
                fadeStates[i].transitioning = true;
                fadeStates[i].transitionStart = now;
            }

            // Handle fading from PRESSED_COLOR to RELEASED_COLOR
            if (curr == OFF && fadeStates[i].transitioning) {
                float elapsed = (now - fadeStates[i].transitionStart) / 1000.0f;
                if (elapsed >= TRANSITION_TIME) {
                    setLeds(i, RELEASED_COLOR);
                    fadeStates[i].transitioning = false;
                    anyChange = true;
                } else {
                    float factor = powf(elapsed / TRANSITION_TIME, POWER);

                    uint8_t r = static_cast<uint8_t>((1.0f - factor) * getRed(PRESSED_COLOR)  + factor * getRed(RELEASED_COLOR));
                    uint8_t g = static_cast<uint8_t>((1.0f - factor) * getGreen(PRESSED_COLOR) + factor * getGreen(RELEASED_COLOR));
                    uint8_t b = static_cast<uint8_t>((1.0f - factor) * getBlue(PRESSED_COLOR)  + factor * getBlue(RELEASED_COLOR));

                    setLeds(i, (r << 16) | (g << 8) | b);
                    anyChange = true;
                }
            }
        }

        if (anyChange) {
            ws2811_render(&ledstring);
        }

        prevState = currentState;
        usleep(5000); // ~200 FPS
    }

    ws2811_fini(&ledstring);
}
