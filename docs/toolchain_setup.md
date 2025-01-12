# Toolchain Setup

## Setup from a fresh Ubuntu-24.04 LTS AMD64

```sh
sudo apt-get update

sudo apt install build-essential

sudo apt install gcc-riscv*

# Alternativly sudo apt install gcc-riscv64-unknown-elf gcc-riscv64-linux-gnu

sudo apt install qemu-system

sudo apt install gdb-multiarch

# Then you need to create ~/.config/gdb/gdbinit file, and add
# .gdbinit in the xv6-labs-2024 to it.
# gdb-mulitiarch will hints how to do this.
```

## Setup From a Fresh ARM64 MACs

```sh
# Install gcc-riscv toolchain, maybe by homebrew?

# Install qemu

```

### Use lldb as the debugger

You cannot use gdb on ARM based macs. Use `lldb` instead.

The lldb is also compatible with the xv6 lab. But you need to a **`.lldbinit`** file with following content:

```txt
settings set target.x86-disassembly-flavor intel
target create --arch riscv64 kernel/kernel
gdb-remote 127.0.0.1:25501
settings set target.process.thread.step-avoid-regexp "lib.*|__.*"
```

You also need to add a config file `~/.lldbinit` with following content

```txt
settings set target.load-cwd-lldbinit true
```

### Commands in `lldb`

```sh
# Display the source code
gui
```
