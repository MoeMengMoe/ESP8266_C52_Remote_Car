# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Remote-controlled car firmware targeting an **80C52 (8051-family) 8-bit MCU**, built with the **Keil C51** toolchain. The car is controlled over WiFi via an ESP8266 module using AT commands over UART.

## Build System

- **IDE**: Keil uVision 5 — no CLI build. Open `remote_car.uvproj` to build and flash.
- **Editing**: VS Code is used for editing only. The `.clangd` config forces C++ mode and includes `fix.h` so clangd can parse 8051-specific keywords (`sbit`, `sfr`, `interrupt`, `code`, `bit`).
- **Keil C51 include path**: `D:/Keil_v5/C51/INC` (referenced in `.clangd`; adjust per machine).

## Code Architecture

### Platform compatibility layer (`fix.h`)

The heart of the VS Code editing setup. When `__clang__` or `VSCODE` is defined (which `.clangd` does), it provides C++ stubs for 8051-specific constructs:
- `sfr` → a C++ class that accepts bitwise operations, so register definitions in `REG52.H` parse correctly
- `sbit` → `bool`
- `bit` → `bool` (via `<stdbool.h>`)
- `code` → empty macro (8051 places constants in ROM; PC has no such distinction)
- `interrupt(x)` / `using(x)` → empty macros (8051 ISR attributes; meaningless on PC)

When compiled with Keil C51 (no `__clang__` defined), these macros expand to proper C51 keywords.

### Motor control (`PWM.c/h`)

Software PWM via the Timer 0 ISR (calls `moto()` every ~1ms). An H-bridge (L298N-style) drives two DC motors through pins `P1^2` through `P1^7`. `motomode` selects behavior:

| Mode | Behavior |
|------|----------|
| 0 | Stop (all pins low) |
| 1 | Forward (both motors forward, PWM on `Speed_L`/`Speed_R`) |
| 2 | Backward (both motors reverse) |
| 3 | Left turn (left motor reverse, right motor stop) |
| 4 | Right turn (left motor stop, right motor forward) |

Speed is controlled via duty cycle: `Speed_L` and `Speed_R` (0–20) set how many ms of a 20ms PWM period the motors are active. Default is 10.

### Display (`display.c/h`)

6-digit 7-segment LED display with dynamic scanning driven by Timer 1 ISR (1ms period). Uses a 74HC595 shift register interface (SS = segment select P2^6, CS = digit select P2^7). `DispBuf[6]` holds the segment data for each digit. `Tab[]` contains the segment patterns for 0–9, A–F, H, L, P, `-`.

### UART & WiFi (`usart.c/h`)

UART configured at 9600 baud (Timer 1 mode 2, TH1=0xFD). `WIFI_Init()` sends a sequence of AT commands to the ESP8266: set STA mode, connect to AP, establish TCP connection to a server, enter transparent transmission mode.

### Main control loop (`main.c`)

The UART ISR receives 9-byte command frames from the WiFi server. Frame format: `-1PdXXXX0` where byte index 7 holds the motion command char (`'0'`–`'4'`), and bytes 4–5 control LEDs. The main loop periodically sends `"12345\n"` as a heartbeat and polls a physical button (`key1` on P3^4) to cycle through motion modes locally.

### Peripherals (`hardware.h`)

| Pin | Signal |
|-----|--------|
| P1^0 | led1 |
| P1^1 | led2 |
| P1^2–P1^7 | Motor H-bridge IN1–IN4, EN1, EN2 |
| P2^3 | buzzer |
| P2^6–P2^7 | Display SS/CS |
| P3^4 | key1 (button) |
