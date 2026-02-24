# Perfmode

Perfmode is a performance control utility for ASUS TUF Gaming series of laptops.
It allows both fan and thermal policy control along with backlight control of the laptop.

### 24 Feb 2026

As of above date, perfmode also supports the generic acpi exposed firmware files for controlling
performance modes on asus laptops. If it detects both the generic ACPI files and kernel module
files are present, it will write to both.
If a single is present, either asus, faustus or acpi files, it will only write to that.

If neither thermal throttle, fan boost, or faustus mode files are present, it will default to acpi
platform files. If none are found, it will error out.

**No matter what your system is using, simply doing `perfmode -fan or -thermal <operation>` will work regardless.**
**If you're not sure what works, always do both, `-fan` and `-thermal`.**

**`-platform` should be the safest option on newer kernels and laptops (6.18+) from testing**

#### Platform Profiles

- `performance` | `p`
- `balanced`    | `b`
- `quiet`       | `s`

#### Fan Control Modes

- `turbo`    | `t`
- `balanced` | `b`
- `silent`   | `s`

#### Thermal Policy
- `overboost` | `o`
- `default`   | `d`
- `silent`    | `s`

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
$ sudo perfmode --option arg
```

or the shorthand

```bash
$ sudo perfmode -o a
```

- Viewing Help

```bash
$ perfmode --help
```

> Please do note that this program relies on the files present in 
 `/sys/devices/platform/asus-nb-wmi` and `/sys/firmware/acpi/` for operation.

## Dependencies

- cc
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
be asked in the [issues](https://github.com/icebarf/perfmode/issues) tab.
