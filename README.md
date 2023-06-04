# Perfmode

Perfmode is a performance control utility for ASUS TUF Gaming series of laptops.
It allows both fan and thermal policy control along with backlight control of the laptop.

#### Fan Control Modes

- `turbo` | `t`
- `balanced` | `b`
- `silent` | `s`

#### Thermal Policy
- `overboost` | `ob`
- `default` | `df`
- `silent` | `s`

Keyboard backlight :

- `off`
- `min`
- `med`
- `max`

> You can use either of Fan or Thermal policy options and it will take desired effect.
Read program help for more information.

## Usage

### Command Line mode

Generic usage:

```bash
$ sudo perfmode -option arg
```

- Viewing Help

```bash
$ perfmode --help
```

> Please do note that this program relies on the files present in 
 `/sys/devices/platform/` and assumes if they exist, then the kernel driver
 is also loaded. Earlier the program relied on lsmod output but that was not
 feasible where the module(s) were built into the kernel.

## Dependencies

- cc (should support C23's `[[noreturn]]`, use anything released in late-2022, or 2023)
- make
- git (optional)

## Installation

### Regular Linux Distributions

```bash
$ git clone https://github.com/rdseed/perfmode.git && cd perfmode
$ make
$ sudo make install # optional - installs to /usr/bin
```

Clone. Make. Install. Simple as that!

### Arch Linux or based distributions

Arch Linux based distributions can directly install from the AUR.
You can either use an AUR Helper or follow the given steps to install manually.

- Package [link](https://aur.archlinux.org/packages/perfmode)

```bash
$ git clone https://aur.archlinux.org/perfmode.git
$ cd perfmode
$ makepkg -si
```

## Notice for users

This tool is still maintained and will receive updates *only* when they are
needed. As such there will not be any frequent commits or releases on this
repository. If you have any questions regarding this, then those should only
be asked in the [issues](https://github.com/rdseed/perfmode/issues) tab.
