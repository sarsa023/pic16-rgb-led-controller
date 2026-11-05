# PIC16F877A RGB LED Controller

An embedded systems project implementing an RGB LED controller using the PIC16F877A microcontroller, a 4×4 keypad, and a multiplexed seven-segment display.

The system allows users to select and adjust the intensity of individual red, green, and blue LED channels through keypad input. Three PWM signals control the RGB LED brightness, allowing different colors to be produced by combining the three components.

The project was written in C, with the circuit designed and simulated using **Proteus 8 Professional**.

## Project Overview

RGB LEDs produce colors by combining three primary light components: red, green, and blue. Each component has an independently adjustable intensity, represented by a value between 0 and 255.

By combining different intensity values, the system can produce a wide range of colors. For example:

| Red | Green | Blue | Result |
|---:|---:|---:|---|
| 255 | 0 | 0 | Red |
| 0 | 255 | 0 | Green |
| 0 | 0 | 255 | Blue |
| 255 | 255 | 0 | Yellow |
| 0 | 255 | 255 | Cyan |
| 255 | 0 | 255 | Magenta |
| 255 | 255 | 255 | White |
| 0 | 0 | 0 | Off |

The controller receives the desired RGB intensity values through the keypad and adjusts the corresponding PWM signals to control the LED brightness.

## System Functionality

### 1. Keypad Input

A 4×4 matrix keypad provides the user interface.

- Three dedicated keys select the color component to be modified: Red, Green, or Blue.
- Numeric keys (0–9) allow the user to enter an intensity value.
- An Enter key confirms the entered value and updates the selected channel.
- A Reset key sets all three color intensities to zero.
- An Erase key removes the last entered digit.

Each RGB component is intended to accept an intensity value between 0 and 255.

### 2. Seven-Segment Display

A Four-digit multiplexed seven-segment display provides feedback during numeric input, where the most significant digit will show the current color being manipulated and the rest 3 digits will show the number being entered.

As the user enters a value, the digits appear sequentially. Newly entered digits shift the previous digits to the left, allowing the user to build a three-digit intensity value.

The display also indicates the currently selected color component.


### 3. PWM-Based RGB Control

The RGB LED has three independently controlled channels. Since the microcontroller's digital outputs provide binary logic levels, Pulse Width Modulation (PWM) is used to control the brightness of each channel.

PWM controls the proportion of time a signal remains HIGH during each period, known as its duty cycle.

The implementation uses two PIC16F877A timer peripherals to generate the three PWM signals:

- **Red channel:** Software-generated PWM using Timer0 interrupts. The interrupt routine toggles the output pin and adjusts the timer reload value according to the requested duty cycle.
- **Blue channel:** Hardware PWM using the CCP1 module and Timer2.
- **Green channel:** Hardware PWM using the CCP2 module and Timer2.

The requested intensity determines the corresponding PWM duty-cycle setting. Combining the three independently controlled channels produces the desired RGB color.

## Interrupt-Based Operation

The controller uses interrupts to handle timer events and keypad input.

- **Timer0 interrupt:** Handles the timing and output transitions of the software-generated red PWM signal.
- **PORTB interrupt-on-change:** Detects keypad input changes and initiates key scanning and processing.

The two CCP modules use Timer2 for hardware PWM generation.

Meanwhile, the seven-segment display is continuously refreshed through multiplexing in the main program loop.

This combines interrupt-driven input handling and PWM generation with continuous display refreshing.

## Hardware and Simulation

The circuit was designed and simulated using **Proteus 8 Professional**.

The system integrates the following components:

- PIC16F877A microcontroller.
- RGB LED.
- 4×4 matrix keypad or 16 buttons as implemented in this project's circuit.
- Four-digit multiplexed seven-segment display.
- Supporting electronic components.

The simulation was used to test the interaction between the microcontroller, keypad, display, and RGB LED output.

## Technologies and Concepts

**Programming language:** C

**Microcontroller:** PIC16F877A

**Simulation:** Proteus 8 Professional

**Concepts:**
- Embedded C programming.
- Microcontroller timers and interrupts.
- Hardware and software PWM generation.
- CCP modules.
- Keypad interfacing and scanning.
- Seven-segment display multiplexing.
- Digital input/output and hardware interfacing.

## Repository Structure

```text
pic16-rgb-led-controller/
├── src/
│   └── main.c     # C code
├── Proteus/
│   ├── circuit.pdsprj
|   ├── circuit_proteus.png
|   ├── main.hex   # hex file to run in proteus microcontroller
├── simple_test/   # testing images
│   └── ...
└── README.md
```

## Academic Context

Developed as part of the Embedded Systems course at the University of Jordan.
