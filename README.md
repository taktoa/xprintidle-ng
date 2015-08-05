# README

## What is `xprintidle-ng`?

### [Web page][website]

[![Gitter][badge-gitter]][gitter]
[![Travis][badge-travis]][travis]
![Issues][badge-gh-issues]
![Forks][badge-gh-forks] 
![Stars][badge-gh-stars] 
![License][badge-license]

`xprintidle-ng` is a utility that queries the X server for the user's idle time
and prints it in milliseconds (or with a custom format string).

## Installation

Basically, type `./configure && make && sudo make install` to compile and
install the program. If you are compiling from the git repository, you must
run `./bootstrap` beforehand.

## Dependencies

### Build requirements

The following common executables must be available to build `xprintidle-ng`:
  * `cat`, `sed`, `tr`, `grep`, `mkdir`, `find`
  * `git`, `wget`, `perl`, `gzip`, `sha1sum`
  * `bash`, `gcc`, `make`

These executables are less common and are also required:
  * `autoconf`, `automake`, `autopoint`, `autoreconf`, `autom4te`, `libtoolize`
  * `pkg-config`, `m4`, `gettext` et al.
  * `help2man`, `makeinfo`

### Runtime requirements

The following libraries must be available to run `xprintidle-ng`:
  * `libxss` (also known as `libXScrnSaver` or similar)
  * `libXext`
  * `libX11`

Here is a short summary of packages required on various systems

| System     | Packages                                                        |
| ---------- | --------------------------------------------------------------- |
| Debian     | `libxss-dev`                                                    |
| Arch Linux | `libxss`                                                        |
| Gentoo     | `x11-base/xorg-server`                                          |
| NixOS      | `xlibs.libX11`, `xlibs.libXScrnSaver`, `xlibs.libXext`          |

## Contribute

### Translations

If you want to submit a translation for this project, here are the steps:
  1.  Install all relevant dependencies
  2.  Fork this repository on GitHub
  3.  `git clone http://github.com/<USERNAME>/xprintidle-ng && cd xprintidle-ng`
  4.  `./bootstrap && ./configure && make`
  5.  The template file will be in `po/xprintidle-ng.pot`
  6.  Based on the template file, write your `po` file, e.g.: `la.po` for Latin
  7.  Put `la.po` in the `po` folder in the repository
  8.  `git add po/la.po`
  9.  `git commit -a -m "Added Latin translation" && git push`
  10. Open a pull request against this repository

You can also translate `doc/xprintidle-ng.texi`; put these translations in an
appropriately named file, e.g.: `doc/xprintidle-ng.la.texi`.

Languages translated so far:
  * English




[website]:         https://taktoa.github.io/xprintidle-ng
[gitter]:          https://gitter.im/taktoa/xprintidle-ng
[travis]:          https://travis-ci.org/taktoa/xprintidle-ng

[badge-gitter]:    https://img.shields.io/badge/Gitter-Join%20Chat%20%E2%86%92-brightgreen.svg?style=flat
[badge-travis]:    https://img.shields.io/travis/taktoa/xprintidle-ng.svg?style=flat
[badge-gh-issues]: https://img.shields.io/github/issues/taktoa/xprintidle-ng.svg?style=flat
[badge-gh-forks]:  https://img.shields.io/github/forks/taktoa/xprintidle-ng.svg?style=flat
[badge-gh-stars]:  https://img.shields.io/github/stars/taktoa/xprintidle-ng.svg?style=flat
[badge-license]:   https://img.shields.io/badge/license-GPL-blue.svg?style=flat
