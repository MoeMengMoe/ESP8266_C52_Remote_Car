# Repository Guidelines

## Project Structure & Module Organization

This repository contains 80C52/8051 firmware for a WiFi-controlled remote car. Source files live at the root:

- `main.c` coordinates UART commands, display feedback, heartbeat sending, and button control.
- `PWM.c/.h` implement Timer 0 driven software PWM and motor modes.
- `display.c/.h` handle six-digit 7-segment display data.
- `usart.c/.h` configure UART and ESP8266 AT-command startup.
- `hardware.h` centralizes pin aliases; `REG52.H` and `STARTUP.A51` are 8051/Keil support files.
- `fix.h` is an editor compatibility layer for clangd/VS Code parsing of Keil C51 extensions.
- `doc/` contains design and debugging notes. `Listings/` and `Objects/` are Keil build outputs.

## Build, Test, and Development Commands

There is no repository CLI build. Use Keil uVision 5:

- Open `remote_car.uvproj` in Keil uVision.
- Build from the IDE to generate `Objects/` and `Listings/`.
- Flash/debug using the configured Keil target.

For editing, VS Code with clangd uses `.clangd`; adjust the Keil include path if your C51 installation differs from `D:/Keil_v5/C51/INC`.

## Coding Style & Naming Conventions

Follow the existing C51 style and keep hardware behavior explicit. Use tabs or local indentation when editing existing functions; do not reformat unrelated code. Keep module APIs in matching `.h` files. Preserve Keil C51 keywords such as `sbit`, `bit`, `code`, and `interrupt(n)`.

Use descriptive names for hardware-facing globals (`Speed_L`, `motomode`, `DispBuf`) and keep pin aliases in `hardware.h`. Prefer short comments only where timing, register setup, or protocol details are not obvious.

## Testing Guidelines

No automated tests are configured. Verify changes by building in Keil and, for behavior changes, testing on hardware or simulator. Check UART frame parsing, motor modes, display scanning, and timer interrupts after edits to `main.c`, `PWM.c`, `display.c`, or `usart.c`.

## Commit & Pull Request Guidelines

Git history currently contains only `Initial Commit`, so no strict commit convention is established. Use concise imperative messages such as `Fix UART frame reset` or `Document motor control modes`.

Pull requests should include a summary, affected modules, Keil build result, and hardware/simulator verification notes. Link issues when available. For visible firmware behavior, include command frames or reproduction steps.

## Agent-Specific Instructions

Do not delete or hand-edit generated Keil output unless requested. Keep `CLAUDE.md` and `doc/` consistent with behavior changes that affect architecture, protocol, or debugging workflow.
