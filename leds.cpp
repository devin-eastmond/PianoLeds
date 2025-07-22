#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>         // <-- Needed for fprintf
#include "ws2811.h"

// Define constants
#define LED_COUNT 16
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

int main()
{
    if (ws2811_init(&ledstring) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed\n");
        return 1;
    }

    // Set the first LED to red
    ledstring.channel[0].leds[0] = 0x00660066;

    ws2811_render(&ledstring);

    sleep(5);  // Keep it on for 5 seconds

    ws2811_fini(&ledstring);
    return 0;
}
