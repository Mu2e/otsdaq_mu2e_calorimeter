# What's here
The purpose of this repository is to provide a simple API to send commands to the Mu2E MZB Board via the I2C bus.

# Files
## The [API_I2C.c](./API_I2C.c) file
- `MZB_Encode_CMD_Command` allows to construnct the full sequence of bytes to be send, from the mnemonic command (defined in `[MZB_Encode_CMD_Command](./res/MZB_Encode_CMD_Command)`)
- `I2C_Write` is an example of an impelmentation of a I2C communication using a Raspberry Pi400 as a master


## The [API_I2C.h](./API_I2C.h) file
Is an header file, which also includes all the relevant headers

## The [main.c](./main.c) file
This file contains a simple example of how to turn on all the channels @ 20 V. A call to the `MZB_Encode_CMD_Command` and then to the `I2C_Write` function are present for two different commands (`HVON` and `DACSET`)


## The [res](./res) directory
This directory contains a copy of the chunked header files from the MZB firmware project


# Compilation and headers
A `[Makefile](./Makefile)` is provided for compiling with GCC. In particular
- A `GNU_COMP` macro is defined to allow the header files modified by me to compile both with KEIL (default) or with GCC
- Compiling using `make PROJ_FOLD=yes` allows to search the header files directly from the firmware directory (not provided here). Otherwise, typing simply `make` result in the compiler to search the headers in the `[res](./res)` directory