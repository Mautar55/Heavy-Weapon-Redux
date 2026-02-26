A slow attempt to reimplement the game Heavy Weapon Deluxe / Atomic Tank in C with raylib in a way that is compatible with legacy assets.

## Build and Run Instructions

This project uses CMake and requires a C/C++ compiler. Raylib is automatically fetched using CMake's `FetchContent`.

### Prerequisites
- [CMake](https://cmake.org/download/) (version 4.1 or higher)
- A C/C++ compiler (e.g., GCC, Clang, or MSVC)

### Building from Console

1.  **Configure the project:**
    Create a build directory and configure the project.
    ```bash
    cmake -B build
    ```

2.  **Build the project:**
    Compile all targets.
    ```bash
    cmake --build build
    ```

### Running the Project

After building, you can run the executables located in the `build` directory (or `build/Debug` / `build/Release` on Windows with MSVC).

-   **Main Game:**
    ```bash
    ./build/attempt_0
    ```
    (On Windows: `.\build\Debug\attempt_0.exe`)

-   **Tests:**
    ```bash
    ./build/test
    ```
    (On Windows: `.\build\Debug\test.exe`)

-   **Asset Generator:**
    ```bash
    ./build/asset_gen
    ```
    (On Windows: `.\build\Debug\asset_gen.exe`)