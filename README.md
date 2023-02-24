# Perfmode

Perfmode is a performance control utility for ASUS TUF Gaming series of laptops. It allows both fan and thermal policy control along with backlight control of the laptop.

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

> You can use either of Fan or Thermal policy options and it will take desired effect. Read program help for more information.

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

### GUI -- Warning! Deprecated in release 3.1.1

GUI has been removed from the current release in favor of the upcoming GUI!

Please use an old release i.e before `3.1.1` or an older commit and compile from source!

![gui.png](assets/gui.png)
```bash
$ perfmode
```

Without any aguments to perfmode, A GUI will be launched. Run with sudo for fan control otherwise
 only LED Control is available without elevated permissions.

The GUI is still in beta and there exists an issue. Please check the issues section for more information if you're interested.

## Dependencies

- C Compiler
- make (build system)
- git (for cloning)

## Installation

### Regular Linux Distributions

```bash
$ git clone https://github.com/rdseed/perfmode.git && cd perfmode
$ make pkg
$ sudo make install
```

Clone. Make. Install. Simple as that!

### Arch (or Arch-based) Distributions

Arch Linux based distributions can directly install from the AUR. You can either use an AUR Helper or follow the given steps to install manually.

- Package [link](https://aur.archlinux.org/packages/perfmode)

```bash
$ git clone https://aur.archlinux.org/perfmode.git
$ cd perfmode
$ makepkg -si
```


### Compiling

Compile using the following commands:

- Using Make

```bash
$ make
# optional - installs to /usr/local/bin
$ sudo make install
```

### Troubleshooting

The following is a list of errors reported by faustus and how to fix them.

- `Perfmode: Kernel modules not available`

If you see this error while running, then make sure you have not disabled `asus_nb_wmi` or `faustus` modules. If you have no idea what this means then there is a very high possibility that you are running the default `asus_nb_wmi` module. You can load it by running the following command:

```bash
$ sudo modprobe asus_nb_wmi
```

Similarly, for faustus (if you have it installed) you can simple replace `asus_nb_wmi` with `faustus`

- `Perfmode: Module files not found`

This error reports that policy files for managing your hardware do not exist.

Either you're on unsupported hardware or you're probably missing the `linux-firmware` package.

Please install it for your respective distribution.

- Perfmode: Insufficient Permissions

This error reports that perfmode does not have enough permissions to open the policy file.

Run perfmode with `sudo` to fix this.

## Notice for users

This tool is still maintained and will receive updates *only* when they are
needed. As such there will not be any frequent commits or releases on this
repository. If you have any questions regarding this, then those should only
be asked in the [issues](https://github.com/rdseed/perfmode/issues) tab.