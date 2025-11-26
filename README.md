# SIC/XE Toolkit (Disassembler, Loader, Linker)

A suite of tools for the Simplified Instructional Computer (SIC/XE), written in modern C++. This project currently features a functional disassembler, with a loader and linker under active development.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Usage](#usage)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Contributing](#contributing)
- [License](#license)

## Features

### Current
*   **SIC/XE Disassembler**: Translates SIC/XE object code from an object file back into human-readable assembly source code.
*   **Opcode-based Parsing**: Utilizes an external opcode definition file (`opcodes.txt`) for easy modification and extension.
*   **Cross-Platform Core**: Written in standard C++ with platform-specific code isolated.
*   **Built-in Logger**: A powerful and configurable logger for debugging and tracing program execution.

### Planned
*   **SIC/XE Loader**: A linking loader to simulate the execution of object programs.
*   **SIC/XE Linker**: A linker to process multiple object programs and resolve external references.
*   **Expanded Addressing Mode Support**: Full support for all SIC/XE addressing modes in the disassembler.
*   **Listing File Generation**: Output a `.lis` file that shows the object code alongside the generated assembly.

## Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

### Prerequisites

*   A C++ compiler that supports at least C++17. The project is built and tested with **Clang++**.
*   **Windows**: The provided `build.bat` script is designed for a Windows environment.
*   **Linux/macOS**: You can compile the project manually using the `clang++` command found in the build script.

### Building

1.  **Clone the repository:**
    ```sh
    git clone <your-repository-url>
    cd <repository-folder>
    ```

2.  **Create a data directory and opcode file:**
    The disassembler requires an opcode definition file.
    *   Create a directory named `data` in the project root.
    *   Inside `data`, create a file named `opcodes.dat`.
    *   Populate `opcodes.dat` with SIC/XE instructions. Each line should be in the format: `MNEMONIC OPCODE FORMAT`.

    **Example `data/opcodes.dat`:**
    ```
    ADD 18 3
    ADDF 58 3
    ADDR 90 2
    AND 40 3
    CLEAR B4 2
    COMP 28 3
    COMPF 88 3
    COMPR A0 2
    DIV 24 3
    DIVF 64 3
    DIVR 9C 2
    FIX C4 1
    FLOAT C0 1
    HIO F4 1
    J 3C 3
    JEQ 30 3
    JGT 34 3
    JLT 38 3
    JSUB 48 3
    LDA 00 3
    LDB 68 3
    LDCH 50 3
    LDF 70 3
    LDL 08 3
    LDS 6C 3
    LDT 74 3
    LDX 04 3
    LPS D0 3
    MUL 20 3
    MULF 60 3
    MULR 98 2
    NORM C8 1
    OR 44 3
    RD D8 3
    RMO B8 2
    RSUB 4C 3
    SHIFTL A4 2
    SHIFTR A8 2
    SIO F0 1
    SSK E0 3
    STA 0C 3
    STB 78 3
    STCH 54 3
    STF 80 3
    STI D4 3
    STL 14 3
    STS 7C 3
    STSW E8 3
    STT 84 3
    STX 10 3
    SUB 1C 3
    SUBF 5C 3
    SUBR 94 2
    SVC B0 2
    TD E0 3
    TIO F8 1
    TIX 2C 3
    TIXR B8 2
    WD DC 3
    ```

3.  **Run the build script:**
    *   **On Windows:**
        ```bat
        .\build.bat
        ```
    *   **On Linux/macOS:**
        You can adapt the command from the build script.
        ```sh
        clang++ src/**/*.cpp -std=c++17 -I./src/ -o ./bin/ysicxe
        ```

    The executable `ysicxe.exe` (or `ysicxe` on non-Windows systems) will be created in the `bin` directory.

### Usage

Run the disassembler from the command line, providing the input object file and the desired output assembly file name.

```sh
./bin/ysicxe <path/to/object_file.obj> <path/to/output_file.asm>
```

## Project Structure

```
├── bin/              # Compiled binaries
├── data/             # Data files (e.g., opcodes.dat)
├── obj/              # Intermediate object files (.o)
├── src/              # C++ source code
│   ├── core/         # Core modules (logger, defines, error handling)
│   ├── util/         # Utility helpers (opcode parser, base conversion)
│   └── main.cpp      # Main application entry point
├── .gitignore
├── build.bat         # Windows build script
└── README.md
```

## How It Works

The disassembler operates by reading a SIC/XE object file, which consists of Header (H), Text (T), and End (E) records.

1.  **Load Opcodes**: The program first loads the instruction mnemonics, opcodes, and formats from `data/opcodes.dat` into an in-memory table for quick lookups.
2.  **Parse Header Record**: It reads the `H` record to determine the program name and its starting address.
3.  **Process Text Records**: For each `T` record, it iterates through the object code byte by byte.
4.  **Instruction Lookup**: It identifies the opcode for an instruction. The two least significant bits are masked off to handle format 4 instructions correctly.
5.  **Decode and Reconstruct**: Based on the instruction's format (1, 2, 3, or 4), it decodes the operands and addressing modes. It then reconstructs the corresponding assembly language instruction.
6.  **Generate Assembly**: The reconstructed assembly lines, along with labels, directives (`START`, `END`, `RESW`, `RESB`, `BYTE`, `WORD`), are written to the specified output file.

## Contributing

Contributions are welcome! If you'd like to contribute, please follow these steps:

1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/YourFeature`).
3.  Commit your changes (`git commit -m 'Add some feature'`).
4.  Push to the branch (`git push origin feature/YourFeature`).
5.  Open a Pull Request.

## License

This project is open-source. Consider adding a license file, such as the MIT License, to clarify how others can use your code.