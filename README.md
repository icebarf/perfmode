# Perfmode

Perfmode is a performance control utility for ASUS FX505DD/DT/DU series of laptops.

Performance modes available on these series of laptop in the Asus Armoury Crate Utility are:

- Turbo
- Balanced
- Silent

Perfmode aims at bringing the same ease of use from windows to linux in maintaining the performance of your asus laptop.

## Usage

```bash
$ sudo perfmode -op | --option
```

- Viewing Help
```bash
$ perfmode -h

or

$ perfmode --help
```

### Notes

Perfmode depends on the asus_nb_wmi module which is used for maintaining the various componenets of an Asus laptop. 
Make sure it is loaded before running.
Perfmode will throw an error if it does not find `asus_nb_wmi` module in the loaded modules list. 
Please refer to the internet on how to load kernel modules if your machine does not have it loaded.

Usually, By default the module is loaded and you have **nothing** to worry about.
This extra note is only for the people who have disabled/unloaded the module for whatever reason they deemed necessary.

### Dependencies

- A C Compiler
- make

### Compiling

Compile using the following commands:

- Using Make

```bash
$ make
```

- Directly by a compiler

```bash
$ gcc -Wall -Wextra src/perfmode.c -o bin/perfmode
```
