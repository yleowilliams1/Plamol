# Plamol

A 2D isometric RPG engine written in C using Raylib. An in-house map editor is also included (currently under construction while work on the entity and loading is under work).

The engine is heavily data-driven, configured via lua files. 

---

## Extra Libraries

This repository uses the following third-party libraries:

- [raylib](https://github.com/raysan5/raylib): *not vendoreded see Build*
- [raygui](https://github.com/raysan5/raygui) `root:/src/raygui.h` 
- [pcg-dxsm](https://github.com/fanf2/pcg-dxsm): `root:/src/util/pcg_basic.c/h`
- [lua](https://www.lua.org/): *not vendoreded see Build*

---

## Status

**Dysfunctional / In Active Development:**
[^1]: In order

- Add windows alternatives for POSIX functions
- Implement entity scripting
- Implement item scripting
- Implement entity saving feature
- Rewrite draw.c to memory efficent and faster.
- Write the map editor.
- Implement unity tests with Unity.
- Write dialogue runner.
- Write combat system.
- Write game manager for map transitions, loading/saving, and camera placement on load(focus on player).
---

## Build

The project uses Cmake. You can either install raylib in a system path the compiler can find, or you can vendor you're own with the cmake flag:

`cmake -S . -B build -DRAYLIB_DIR=direction/to/raylib`

or you can have cmake compile raylib automatically in the build folder

`cmake -S . -B build -DFETCH_RAYLIB=ON`

Then build normally.
---
### Linux

Installing raylib on your system is going to depend on your distro, please look into it, or manually point cmake to raylib or have it automatically compile for you [see above].

`cmake -S . -B build`

`cmake --build build`

`./app`
---
### macOS

#### dependencies
`xcode-select --install`

`brew install cmake raylib` 
#### cmake
`cmake -S . -B build`

`cmake --build build`

`./app`
---
### Windows

I'm pretty sure this won't work. I use posix functions for the logging. Eventually I'll get round to making it compilable for windows, but for now it's not happening. Even if I fixed that I'm pretty sure it'll freak out about fopen and strcpy and sscanf and strtok and sprintf, and I'm not doing windows specific EOF checking on the ini files, I'm not actually sure if that'll break, but it's definetly not ideal. It's probably a 20 minutes fix with a couple ifdefs, I'll get round to it eventually.

#### You can use MSVC from powershell with Visual Studio or the Build Tools with "Desktop Development with C++

`cmake -S . -B build -DFETCH_RAYLIB=ON`

`cmake --build build --config Release`

`.\Release\app.exe`

---

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**. See the [LICENSE](LICENSE) file for details.

Unaltered licenses for all third-party libraries are located in the `licenses/` directory.
