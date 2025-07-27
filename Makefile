# Makefile for compiling led_test.cpp using rpi_ws281x

TARGET = piano_leds
SRC = main.cpp midi_receiver.cpp key_states.cpp led_controller.cpp modes.cpp
LIB_PATH = rpi_ws281x
LIBS = $(LIB_PATH)/libws2811.a -lm -lasound
INCLUDES = -I$(LIB_PATH)

CXX = g++
CXXFLAGS = -Wall -O2

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)


PI_USER = devineastmond
PI_HOST = pi-led-piano.local
REMOTE_DIR = /home/$(PI_USER)/Documents/PianoLeds
LOCAL_DIR = ./

upload:
	rsync -avz --delete $(LOCAL_DIR) $(PI_USER)@$(PI_HOST):$(REMOTE_DIR)
