[中文](README.md) | **English**

# SVpwm_STM32G474

Space-vector pulse-width modulation (SVPWM) on the STM32G474 microcontroller.

> **Note**: this project was an entry that was *not* selected for the 2025 National Undergraduate
> Electronic Design Contest (NUEDC). It is published for learning and reference only.

## Overview

Implements a three-phase SVPWM algorithm on the STM32G474, aimed at motor control and inverter
applications. It was developed while preparing for the 2025 NUEDC, later abandoned because of the
technical issues below, and is open-sourced here for study and discussion.

## Hardware

- **MCU**: STM32G474XX
- **Tooling**: STM32CubeMX + CMake

## Project layout

```
.
├── Core/                   # Application and peripheral code
├── Drivers/                # STM32 HAL drivers
├── build/                  # Build output
├── cmake/                  # CMake configuration
├── log/                    # Log files
├── CMakeLists.txt          # Top-level CMake build
├── CMakePresets.json       # CMake presets
├── phase3_svpwm.ioc        # STM32CubeMX project file
├── startup_stm32g474xx.s   # Startup file
└── STM32G474XX_FLASH.ld    # Linker script
```

## Building

### Prerequisites

- ARM GCC toolchain
- CMake (>= 3.22)
- Make or Ninja

### Steps

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Or via CMake presets
cmake --preset default
cmake --build --preset default
```

## Features

- ✅ Three-phase SVPWM implementation
- ✅ STM32G474 timer configuration
- ✅ CMake build system

## Tooling

Any of the following works well:

- STM32CubeIDE
- VS Code + CMake Tools
- Keil MDK (requires extra configuration)

## Project status

⚠️ **Abandoned.** This was an early exploration for the 2025 NUEDC. Development stopped because of:

- **SVPWM harmonics** — space-vector modulation produced high harmonic content, making it hard to
  meet the contest's strict requirements on output waveform quality.
- **Closed-loop control difficulty** — the closed-loop system was complex to debug, PID tuning was
  difficult, and overall stability was poor.

**Lessons learned**

- SVPWM looks attractive on paper, but practical use needs extra filtering to reduce harmonics.
- Closed-loop control demands thorough upfront simulation and parameter tuning; the time cost is high.
- Future contestants should assess feasibility and debugging difficulty carefully before committing.

## License

Released under an open-source license — fill in the specific one as appropriate.

## Contributing

Issues and pull requests are welcome.

## Contact

- Author: 66Leslie
- Repository: <https://github.com/66Leslie/SVpwm_STM32G474>
