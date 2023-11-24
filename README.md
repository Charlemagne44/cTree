# cTree

cTree is a CLI written in C that generates random ascii art trees within the users terminal. Users can generate single trees for copyable ascii trees, or infinitely loop the generation to serve as a live animation.

## Installation

cTree must be built and installed from source via [gcc](https://gcc.gnu.org/) and [Makefile](https://www.gnu.org/software/make/manual/make.html). [Ncurses](https://ftp.gnu.org/gnu/ncurses/) is it's sole external dependency and can be installed on most popular linux package managers.

#### Debian example

```bash
sudo apt install libncursesw5-dev
```

### Build and Install

```bash
git clone https://github.com/Charlemagne44/cTree.git
cd cTree

sudo make install
```

### Uninstall

```bash
cd cTree

make clean
sudo make uninstall
```

## Usage

cTree [options] ...

```
Options:
    -h      Print help menu
    -d      Print debug seed information alongside tree
    -s      Supply a seed to the randomizer (commonly paired with -d)
    -l      Print the tree live, with -l [arg] millseconds of sleep time between printing each character
    -i      Infinitely generate trees (commonly paired with -l for infinite slowed animation)
    -w      Wait time in millseconds after a tree is finished generating, before generating a new tree (Only necessary with -i supplied)
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## Example

[![asciicast](https://asciinema.org/a/NYuJLB1YXlRfNP5FjRL4jghK3.svg)](https://asciinema.org/a/NYuJLB1YXlRfNP5FjRL4jghK3)

## License

[MIT](https://choosealicense.com/licenses/mit/)
