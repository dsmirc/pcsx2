# PCSX2

![Windows Build Status](https://img.shields.io/github/actions/workflow/status/PCSX2/pcsx2/windows_build_matrix.yml?label=%F0%9F%96%A5%EF%B8%8F%20Windows%20Builds)
![Linux Build Status](https://img.shields.io/github/actions/workflow/status/PCSX2/pcsx2/linux_build_matrix.yml?label=%F0%9F%90%A7%20Linux%20Builds)
![MacOS Build Status](https://img.shields.io/github/actions/workflow/status/PCSX2/pcsx2/macos_build_matrix.yml?label=%F0%9F%8D%8E%20MacOS%20Builds)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/1f7c0d75fec74d6daa6adb084e5b4f71)](https://www.codacy.com/gh/PCSX2/pcsx2/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=PCSX2/pcsx2&amp;utm_campaign=Badge_Grade)
[![Discord Server](https://img.shields.io/discord/309643527816609793?color=%235CA8FA&label=PCSX2%20Discord&logo=discord&logoColor=white)](https://discord.com/invite/TCz3t9k)

PCSX2 is a free and open-source PlayStation 2 (PS2) emulator. Its purpose is to emulate the PS2's hardware, using a combination of MIPS CPU [Interpreters](<https://en.wikipedia.org/wiki/Interpreter_(computing)>), [Recompilers](https://en.wikipedia.org/wiki/Dynamic_recompilation) and [Virtual Machine](https://en.wikipedia.org/wiki/Virtual_machine) which manages hardware states and PS2 system memory. This allows you to play PS2 games on your PC, with many additional features and benefits.

## Project Details

The PCSX2 project has been running for more than twenty years. Past versions could only run a few public domain game demos, but newer versions can run most games at full speed, including popular titles such as Final Fantasy X and Devil May Cry 3. Visit the [PCSX2 compatibility list](https://pcsx2.net/compat/) to check the latest compatibility status of games (with more than 2500 titles tested), or ask for help in the [official forums](https://forums.pcsx2.net/).

The latest officially released stable version is version 1.6.0.

Installers and binaries for both stable and development builds are available from [our website](https://pcsx2.net/downloads/).

## System Requirements

### Minimum

| Operating System                                                                                                               | CPU                                                                                                                                                                                             | GPU                                                                                                                                                                                               | RAM  |
| ------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---- |
| - Windows 10 21H2 (1809 or later) (64-bit) <br/> - Ubuntu 20.04/Debian or newer, Arch Linux, or other distro (64-bit) <br/> - macOS 10.14 | - Supports SSE4.1 <br/> - [PassMark Single Thread Performance](https://www.cpubenchmark.net/singleThread.html) rating near or greater than 1800 <br/> - Two physical cores, with hyperthreading | - Direct3D10 support <br/> - OpenGL 3.x support <br/> - Vulkan 1.1 support <br/> - Metal support <br/> - [PassMark G3D Mark](https://www.videocardbenchmark.net/high_end_gpus.html) rating around 3000 (Geforce GTX 750 Radeon RX 560 Intel Arc A380) <br/> - 2 GB Video Memory | 4 GB |

_Note: Recommended Single Thread Performance is based on moderately complex games. Games that pushed the PS2 hardware to its limits will struggle on CPUs at this level. Some release titles and 2D games which underutilized the PS2 hardware may run on CPUs rated as low as 1200. A quick reference for CPU **intensive games**: [Wiki](https://wiki.pcsx2.net/Category:CPU_intensive_games), [Forum](https://forums.pcsx2.net/Thread-LIST-The-Most-CPU-Intensive-Games) and CPU **light** games: [Forum](https://forums.pcsx2.net/Thread-LIST-Games-that-don-t-need-a-strong-CPU-to-emulate)_

### Recommended

| Operating System                                                                                                              | CPU                                                                                                                                                                                                       | GPU                                                                                                                                                                                                   | RAM  |
| ----------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---- |
| - Windows 10 21H2 (1809 or later) (64-bit) <br/> - Ubuntu 22.04/Debian or newer, Arch Linux, or other distro (64-bit) <br/> - macOS 10.14 | - Supports AVX2 <br/> - [PassMark Single Thread Performance](https://www.cpubenchmark.net/singleThread.html) rating near or greater than 2600 <br/> - Four physical cores, with or without hyperthreading | - Direct3D12 support <br/> - OpenGL 4.6 support <br/> - Vulkan 1.3 support <br/> - Metal support <br/> - [PassMark G3D Mark](https://www.videocardbenchmark.net/high_end_gpus.html) rating around 6000 (GeForce GTX 1650 Radeon RX 570) <br/> - 4 GB Video Memory | 8 GB |

_Note: Recommended GPU is based on 3x Internal, ~1080p resolution requirements. Higher resolutions will require stronger cards; 6x Internal, ~4K resolution will require a [PassMark G3D Mark](https://www.videocardbenchmark.net/high_end_gpus.html) rating around 12000 (GeForce RTX 2060 Radeon RX 6600 Intel Arc A750). Just like CPU requirements, this is also highly game dependent. A quick reference for GPU **intensive games**: [Wiki](https://wiki.pcsx2.net/Category:GPU_intensive_games)_

### Technical Notes

-   You need the [Visual C++ 2019 x64 Redistributables](https://support.microsoft.com/en-us/help/2977003/) to run PCSX2.
-   Windows XP and Direct3D9 support was dropped after stable release 1.4.0.
-   Windows 7, Windows 8.0, and Windows 8.1 support was dropped after stable release 1.6.0.
-   32-bit and wxwidgets support was dropped after stable release 1.6.0, with the wxwidgets code being removed completely on 25th December 2022.
-   Make sure to update your operating system and drivers to ensure you have the best experience possible. Having a newer GPU is also recommended so you have the latest supported drivers.
-   Because of copyright issues, and the complexity of trying to work around it, you need a BIOS dump extracted from a legitimately-owned PS2 console to use the emulator. For more information about the BIOS and how to get it from your console, visit [this page](pcsx2/Docs/PCSX2_FAQ.md#question-13-where-do-i-get-a-ps2-bios).
-   PCSX2 uses two CPU cores for emulation by default. A third core can be used via the MTVU speed hack, which is compatible with most games. This can be a significant speedup on CPUs with 3+ cores, but it may be a slowdown on GS-limited games (or on CPUs with fewer than 2 cores). Software renderers will then additionally use however many rendering threads it is set to and will need higher core counts to run efficiently.
-   Requirements benchmarks are based on a statistic from the Passmark CPU bench marking software. When we say "STR", we are referring to Passmark's "Single Thread Rating" statistic. You can look up your CPU on [Passmark's website for CPUs](https://cpubenchmark.net) to see how it compares to PCSX2's requirements.
-   Vulkan requires an up-to-date GPU driver; old drivers may cause graphical problems.

Want more? [Check out the PCSX2 website](https://pcsx2.net/).
