# chippy

A simple CHIP-8 interpreter written in C++20.

Runs basic test ROMs (like the IBM logo) and draws the 64x32 display directly to the terminal using ASCII characters.

## Supported Opcodes
- Jumps (`1NNN`)
- Skip checks (`3XNN`, `4XNN`)
- Register arithmetic & bitwise ops (`6XNN`, `7XNN`, `8XY0`, `8XY1`)
- Index pointer (`ANNN`)
- Sprite drawing with XOR collision detection (`DXYN`)

## Output
Running `IBM Logo.ch8`:

```text
   #####  ######   #     #
     #    #     #  ##   ##
     #    #     #  # # # #
     #    ######   #  #  #
     #    #     #  #     #
     #    #     #  #     #
   #####  ######   #     #
```

## Building
Open `CHIP.slnx` in Visual Studio 2022 and build `Release | x64`.
