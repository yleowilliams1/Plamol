# Plamol

A 2D isometric RPG engine written in C using Raylib. An in-house map editor is also included (currently under construction while work on the UI library is under work).

The engine is heavily data-driven, configured via text files and a central `engine.ini` file.

---

## Extra Libraries

This repository uses the following third-party libraries:

- [raylib](https://github.com/raysan5/raylib): `user/include`
- [pcg-dxsm](https://github.com/fanf2/pcg-dxsm): `root:/src/util/pcg_basic.c/h`

---

## Status

**Dysfunctional / In Active Development:**
[^1]: In order

- Setup Cmake.
- Implement unity tests with Unity.
- Write dialogue runner.
- Write combat system.
- Write custom UI system.
- Write in house map editor with UI system.
- Write game manager for map transitions, loading/saving, and camera placement on load(focus on player).
---

## Build

There is no automated build script right now. A `Makefile` or CMake configuration will be added eventually, but for now, there is no build setup available.

---

## Data & Architecture

1. **`engine.ini`**: The master configuration file that most systems reference. You can override the default path (which defaults to the executable directory) by passing a path argument to the executable. Entries use an X-Macro pattern defined in `settings.h`. The parser detects missing values, but omitting required settings will cause undefined behavior.
2. **Binary Formats**: Maps and save files are stored as binary blobs with custom magic numbers and endianness handling. Maps require the custom editor, while save files are handled directly in-game.

---

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**. See the [LICENSE](LICENSE) file for details.

Unaltered licenses for all third-party libraries are located in the `licenses/` directory.
