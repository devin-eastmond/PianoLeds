# PianoLeds
A C++ program that runs on a Raspberry Pi 3 that takes in MIDI input from an electronic piano keyboard and outputs signals to a WS2812b LED strip

## What you need
This project is designed to work with a WS2812b LED light strip with 144 pixels per meter, spanning 176 pixels (I bought two 1-meter strips, soldered them together, and then cut to match the length of the keyboard). It is programmed to start on the left and go to the right

## Setup
* Connect the data wire to GPIO 18
* Clone this repository on your Raspberry Pi
* In the project folder, clone the rpi_ws281x repository: <https://github.com/jgarff/rpi_ws281x>
* Run `make` to compile the project
* Run the program with `sudo ./main`
* You may need to run `aconnect 24:0 128:0` in another terminal window to start receiving midi
