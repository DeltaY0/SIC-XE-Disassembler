# SIC/XE Toolkit (Disassembler, Linker)

A suite of tools for the Simplified Instructional Computer (SIC/XE), written in modern C++. This project currently features a functional disassembler and a linker.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
- [Usage](#usage)
  - [Commands](#commands)
    - [dasm](#dasm)
    - [link](#link)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
  - [Disassembler](#disassembler)
  - [Linker](#linker)
- [Contributing](#contributing)
- [License](#license)

## Features

*   **SIC/XE Disassembler**: Translates SIC/XE object code from an object file back into human-readable assembly source code.
*   **SIC/XE Linker**: Links multiple object files into a single loadable memory image.
*   **Opcode-based Parsing**: Utilizes an external opcode definition file (`res/opcodes.txt`) for easy modification and extension.
*   **Cross-Platform Core**: Written in standard C++ with platform-specific code isolated.
*   **Built-in Logger**: A powerful and configurable logger for debugging and tracing program execution.

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

2.  **Run the build script:**
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

## Usage

The program is run from the command line. The first argument is the command you want to execute (`dasm` or `link`), followed by the command's arguments.

```sh
./bin/ysicxe <command> [args...]
```

### Commands

#### `dasm`
Disassembles a SIC/XE object file into assembly source code.

**Usage:**
```sh
./bin/ysicxe dasm <file> [args...]
```

**Arguments:**

| Flag(s)          | Description                                                    | Required | Default   |
| ---------------- | -------------------------------------------------------------- | -------- | --------- |
| `-i`, `--input`  | Path to the input object file (`.obj`).                        | Yes      |           |
| `-o`, `--output` | Path to the output source file (`.asm`).                       | No       | `out.asm` |
| `-s`, `--symtab` | Path to an external symbol table for label resolution.         | No       |           |

**Example:**
```sh
./bin/ysicxe dasm -i test/testxy.obj -o test/testxy.asm -s test/testxy_symtab.txt
```

#### `link`
Links multiple SIC/XE object files into a single executable memory image.

**Usage:**
```sh
./bin/ysicxe link [args...]
```

**Arguments:**

| Flag(s)             | Description                                                         | Required | Default |
| ------------------- | ------------------------------------------------------------------- | -------- | ------- |
| `-i`, `--inputs`    | Comma-separated list of input object files.                         | Yes      |         |
| `-o`, `--output`    | Path to the output executable/memory-dump file.                     | No       | `a.out` |
| `-a`, `--addr`      | Starting load address in hexadecimal.                               | No       | `0`     |
| `-e`, `--export-estab`| Export the global symbol table (ESTAB) to a file.                  | No       |         |

**Example:**
```sh
./bin/ysicxe link -i test/prog1.obj,test/prog2.obj -o test/linked.exe -a 4000
```

## Project Structure

```
├── bin/              # Compiled binaries
├── res/              # Data files (e.g., opcodes.txt)
├── obj/              # Intermediate object files (.o)
├── src/              # C++ source code
│   ├── cmd/          # Command line parsing and handlers
│   ├── core/         # Core modules (logger, defines, error handling)
│   ├── dasm/         # Disassembler implementation
│   ├── linker/       # Linker implementation
│   ├── util/         # Utility helpers
│   └── main.cpp      # Main application entry point
├── test/             # Test files
├── .gitignore
├── build.bat         # Windows build script
└── README.md
```

## How It Works

### Disassembler

The disassembler operates by reading a SIC/XE object file, which consists of Header (H), Text (T), and End (E) records.

1.  **Load Opcodes**: The program first loads the instruction mnemonics, opcodes, and formats from `res/opcodes.txt` into an in-memory table for quick lookups.
2.  **Parse Header Record**: It reads the `H` record to determine the program name and its starting address.
3.  **Process Text Records**: For each `T` record, it iterates through the object code byte by byte.
4.  **Instruction Lookup**: It identifies the opcode for an instruction. The two least significant bits are masked off to handle format 4 instructions correctly.
5.  **Decode and Reconstruct**: Based on the instruction's format (1, 2, 3, or 4), it decodes the operands and addressing modes. It then reconstructs the corresponding assembly language instruction.
6.  **Symbol Resolution**: If a symbol table is provided, the disassembler will use it to resolve addresses into labels, making the output more readable.
7.  **Generate Assembly**: The reconstructed assembly lines, along with labels, directives (`START`, `END`, `RESW`, `RESB`, `BYTE`, `WORD`), are written to the specified output file.

### Linker

The linker performs a two-pass process to resolve external references between different object files and create a single, loadable program.

*   **Pass 1:**
    1.  **Process Control Sections:** The linker processes each control section from the input object files.
    2.  **Build ESTAB:** It builds an External Symbol Table (ESTAB), which stores the names and addresses of all external symbols (defined in `D` records).
    3.  **Assign Addresses:** It assigns a starting address to each control section and calculates the length of the linked program.

*   **Pass 2:**
    1.  **Generate Object Code:** The linker generates the final object code by processing the `T` records of each control section.
    2.  **Resolve External References:** It uses the ESTAB built in Pass 1 to resolve external references (found in `M` records). It modifies the object code at the specified locations to insert the correct addresses.
    3.  **Write Executable:** The final, linked object code is written to the output file, which can then be loaded into memory for execution.

## Contributing

Contributions are welcome! If you'd like to contribute, please follow these steps:

1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/YourFeature`).
3.  Commit your changes (`git commit -m 'Add some feature'`).
4.  Push to the branch (`git push origin feature/YourFeature`).
5.  Open a Pull Request.

## License

This project is open-source and under the MIT License, learn more by reading the LICENSE file.
