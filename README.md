# Plamol

A 2D isometric RPG engine written in C using Raylib. An in-house map editor is also included (currently under construction while work on the UI library is under work).

The engine is heavily data-driven, configured via text files and a central `engine.ini` file.

---

## Extra Libraries

This repository uses the following third-party libraries:

- [raylib](https://github.com/raysan5/raylib): *not vendoreded see Build*
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

The project uses Cmake. You can either install raylib in a system path the compiler can find, or you can vendor you're own with the cmake flag:

`cmake -S . -B build -DRAYLIB_DIR=direction/to/raylib`

or you can have cmake compile raylib automatically in the build folder

`cmake -S . -B build -DFETCH_RAYLIB=ON`

Then build normally.

### Linux

[^1]: Installing raylib on your system is going to depend on your distro, please look into it, or manually point cmake to raylib or have it automatically compile for you [see above].

`cmake -S . -B build`
`cmake --build build`
`./app`

### macOS

#### dependencies
`xcode-select --install`
`brew install cmake raylib` 
#### cmake
`cmake -S . -B build`
`cmake --build build`
`./app`

### Windows

I'm pretty sure this won't work. I use posix functions for the logging. Eventually I'll get round to making it compilable for windows, but for now it's not happening. Even if I fixed that I'm pretty sure it'll freak out about fopen and strcpy and sscanf and strtok and sprintf, and I'm not doing windows specific EOF checking on the ini files, I'm not actually sure if that'll break, but it's definetly not ideal. It's probably a 20 minutes fix with a couple ifdefs, I'll get round to it eventually.

#### You can use MSVC from powershell with Visual Studio or the Build Tools with "Desktop Development with C++"
[^1]: You'll most likely need to use fetch raylib. You don't have to of course but it's the simplest.
`cmake -S . -B build -DFETCH_RAYLIB=ON`
`cmake --build build --config Release`
`.\Release\app.exe`

---

## Data & Architecture

1. **`engine.ini`**: The master configuration file that most systems reference. You can override the default path (which defaults to the executable directory) by passing a path argument to the executable. Entries use an X-Macro pattern defined in `settings.h`. The parser detects missing values, but omitting required settings will cause undefined behavior.
2. **Binary Formats**: Maps and save files are stored as binary blobs with custom magic numbers and endianness handling. Maps require the custom editor, while save files are handled directly in-game.

---

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**. See the [LICENSE](LICENSE) file for details.

Unaltered licenses for all third-party libraries are located in the `licenses/` directory.
