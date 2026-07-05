# ASCII Spinning Cube

> A tiny terminal-based 3D renderer written in C++.

A small C++ console project that renders a rotating 3D cube directly in the Windows terminal using ASCII characters.

## Preview

The program draws a rotating cube in the console using different symbols for different faces:

```text
      @@@@@@@
   @@@+++++@@@
  @@+++$$$+++@@
  @@++$$$$$++@@
   @@@+++++@@@
      @@@@@@@
```

## Features

- 3D rotation around X, Y and Z axes
- Perspective projection
- Z-buffering
- Dynamic console resizing
- Smooth terminal rendering with `WriteConsoleA`
- No external libraries

## Requirements

- Windows
- Visual Studio with **Desktop development with C++**
- C++17 or newer

## Build in Visual Studio

1. Create a new **Console App** project.
2. Choose **C++** and **Windows**.
3. Replace the generated `main.cpp` with this project's `main.cpp`.
4. Open project properties:
   - `Configuration Properties → C/C++ → Language`
   - set **C++ Language Standard** to **ISO C++17 Standard (/std:c++17)**
5. Build and run with `Ctrl + F5`.

## Build with MinGW

```bash
g++ main.cpp -std=c++17 -O2 -o cube.exe
cube.exe
```

## How it works

The program stores the terminal screen as a simple one-dimensional character buffer.

Each cube face is sampled into many 3D points. Every point is rotated, projected from 3D space onto the 2D terminal screen, and written into the screen buffer.

A z-buffer is used to decide which point should be visible when multiple points are projected onto the same terminal cell.

## Controls

There are no controls yet. Close the terminal window or stop the program to exit.

## Possible improvements

- Add keyboard controls
- Add color support
- Add pause/resume
- Add multiple cubes
- Add FPS counter
- Make the project cross-platform

## License

This project is licensed under the MIT License.
