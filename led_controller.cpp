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

#include <iostream>
#include <vector>

std::vector<std::vector<int>> ledMappings = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}, {14, 15},
    {16, 17}, {18, 19}, {20, 21}, {22, 23}, {24, 25}, {26, 27}, {28, 29},
    {30, 31}, {32, 33}, {34, 35}, {36, 37}, {38, 39}, {40, 41}, {42, 43},
    {44, 45}, {46, 47}, {48, 49}, {50, 51}, {52, 53}, {54, 55}, {56, 57},
    {58, 59}, {60, 61}, {62, 63}, {64, 65}, {66, 67}, {68, 69}, {70, 71},
    {72, 73}, {74, 75}, {76, 77}, {78, 79}, {80, 81}, {82, 83}, {84, 85},
    {86, 87}, {88, 89}, {90, 91}, {92, 93}, {94}, {95, 96}, {97, 98},
    {99, 100}, {101, 102}, {103, 104}, {105, 106}, {107, 108}, {109, 110},
    {111, 112}, {113, 114}, {115, 116}, {117, 118}, {119, 120}, {121, 122},
    {123, 124}, {125, 126}, {127, 128}, {129, 130}, {131, 132}, {133, 134},
    {135, 136}, {137, 138}, {139, 140}, {141, 142}, {143, 144}, {145, 146},
    {147, 148}, {149, 150}, {151, 152}, {153, 154}, {155, 156}, {157, 158},
    {159, 160}, {161, 162}, {163, 164}, {165, 166}, {167, 168}, {169, 170},
    {171, 172}, {173, 174, 175}
};

ws2811_t ledstring = {
    .freq = WS2811_TARGET_FREQ,
    .dmanum = DMA,
    .channel = {
        [0] = {
            .gpionum = GPIO_PIN,
            .invert = 0,
            .count = LED_COUNT,
            .strip_type = STRIP_TYPE,
            .brightness = 100,
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
    for (int led : ledMappings[keyIndex]) {
        ledstring.channel[0].leds[led] = color;
    }
}

void ledController(KeyStates& keyStates) {
    std::vector<State> prevState = keyStates.getAllKeys();
    std::vector<FadeState> fadeStates(LED_COUNT);

    if (ws2811_init(&ledstring) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed\n");
        return;
    }

    for (int i = 0; i < LED_COUNT; i++) {
        ledstring.channel[0].leds[i] = RELEASED_COLOR;
    }
    ws2811_render(&ledstring);

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
