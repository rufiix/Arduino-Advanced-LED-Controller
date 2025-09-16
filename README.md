# Arduino Advanced LED Controller

## Overview

This project is an advanced LED controller for the Arduino platform, written in C++. It allows a user to cycle through multiple lighting modes and adjust LED brightness using a single push-button with long-press detection. The code is designed to be efficient and non-blocking, utilizing the `millis()` function for timing instead of `delay()`.

The controller demonstrates handling of complex user input (short vs. long press), state management for different operational modes, and real-time hardware interaction.

---

## Features

* **Single-Button Control**: Use one button to manage all functions.
    * **Short Press**: Cycles through the available lighting modes.
    * **Long Press**: Enters brightness adjustment mode.
* **Multiple Lighting Modes**: A total of 7 distinct modes are implemented:
    * **Mode 0**: All LEDs off.
    * **Modes 1-3**: Synchronized blinking with different intervals (500ms, 200ms, 100ms).
    * **Mode 4**: Alternating flash between two LEDs.
    * **Mode 5**: Realistic anti-collision strobe light effect with randomized intervals.
    * **Mode 6**: Very fast synchronized blinking (50ms interval).
* **Adjustable Brightness**: Hold the button to enter a smooth brightness adjustment mode. The brightness cycles between a defined minimum and maximum (`120` to `255`).
* **Non-Blocking Code**: The main loop is free of `delay()`, allowing for responsive button handling even during blinking sequences.
* **Software Debounce**: Implements a robust debounce mechanism for the button to prevent multiple triggers from a single press.
* **Serial Monitor Feedback**: The application prints the current mode and brightness level to the serial monitor for easy debugging and status checks.

---

## Hardware Requirements

* An Arduino-compatible microcontroller (e.g., Arduino Uno, Nano).
* 2 x LEDs.
* 1 x Push-button.
* Resistors for the LEDs and button as required by your specific components.

### Wiring

* Connect the LEDs to digital pins `3` and `5` (or any other PWM-capable pins, update `ledPins` array accordingly).
* Connect the push-button to analog pin `A3` with a pull-up resistor configuration (the internal pull-up is used in the code).

---

## How to Use

1.  **Upload the Code**: Compile and upload the `lampki.cpp` sketch to your Arduino board using the Arduino IDE or a compatible tool like PlatformIO.
2.  **Open Serial Monitor**: Open the serial monitor at a baud rate of `9600` to see status messages.
3.  **Control the LEDs**:
    * **Change Mode**: Briefly press and release the button. The program will cycle from Mode 0 to 6 and then loop back to 0.
    * **Adjust Brightness**: Press and hold the button for 3 seconds. The LEDs will enter a brightness adjustment mode, smoothly dimming and brightening. Release the button to set the current brightness level.

---

## Code Structure

The code is contained within a single `lampki.cpp` file and is structured as follows:

* **Global Constants & Variables**:
    * Pin definitions (`ledPins`, `buttonPin`).
    * Brightness thresholds (`minBrt`, `maxBrt`).
    * Mode definitions and corresponding blink intervals (`mode`, `intervals`, `modeNames`).
    * Variables for button debounce, long-press detection, brightness adjustment, and blinking states.

* **`setup()`**: Initializes the pin modes and starts serial communication.

* **`loop()`**: The main, non-blocking loop that performs the following actions:
    1.  Reads and debounces the button state.
    2.  Detects short vs. long presses to either change modes or enter brightness adjustment.
    3.  If in brightness adjustment mode, smoothly updates the `brightness` variable.
    4.  Executes the logic for the current `mode` using a `switch` statement, controlling the LEDs accordingly.
