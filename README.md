# Perfmode

Perfmode is a performance control utility for ASUS TUF Gaming series of laptops.

Performance modes available on these series of laptop in the Asus Armoury Crate Utility are:

- Turbo
- Balanced
- Silent

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

### Troubleshooting

The following is a list of errors reported by faustus and how to fix them.

- Perfmode: asus_nb_wmi or faustus module not loaded!

If you see this error while running, then make sure you have not disabled asus_nb_wmi or faustus modules. If you have no idea what this means then there is a very high possibility that you are running the default `asus_nb_wmi` module. You can load it by running the following command:

```bash
$ sudo modprobe asus_nb_wmi
```

Similarly, for faustus (if you have it installed) you can simple replace `asus_nb_wmi` with `faustus`

- Perfmode: module files not found

This error reports that policy files for managing your hardware do not exist. Its a very grave error and you should probably reinstall the `linux` kernel and the `linux-firmware` package.

- Perfmode: Could not open Policy file

This error reports that perfmode does not have enough permissions to open the policy file.

Run perfmode with `sudo` to fix this.

- Perfmode: Could not write to Policy file

This error reports that perfmode does not have enough permissions to open the policy file.

Run perfmode with `sudo` to fix this.

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
