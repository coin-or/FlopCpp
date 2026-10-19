# FlopCpp

[![A COIN-OR Project](https://coin-or.github.io/coin-or-badge.png)](https://www.coin-or.org)

Projects such as this one are maintained by a small group of volunteers under
the auspices of the non-profit [COIN-OR Foundation](https://www.coin-or.org)
and we need your help! Please consider [sponsoring our
activities](https://github.com/sponsors/coin-or) or [volunteering](mailto:volunteer@coin-or.org) to help!

[![Latest Release](https://img.shields.io/github/v/release/coin-or/FlopCpp?sort=semver)](https://github.com/coin-or/FlopCpp/releases)

_This file is auto-generated from [config.yml](.coin-or/config.yml) using the 
[generate_readme](.coin-or/generate_readme) script.
To make changes, please edit [config.yml](.coin-or/config.yml) or the generation scripts
[here](.coin-or/generate_readme) and [here](https://github.com/coin-or/coinbrew/blob/master/scripts/generate_readme)._

FLOPC++ (Formulation of Linear Optimization Problems in C++) is an
open-source algebraic modeling language implemented as a C++ class library.
Using FLOPC++, linear and mixed-integer optimization models can be specified
in a declarative style, similar to algebraic modeling languages such as GAMS
and AMPL, within a C++ program. This combines the traditional strengths of
algebraic modeling languages, such as the ability to declare a model in a
concise and readable way, with the speed, power, and flexibility of an
object-oriented language. FLOPC++ uses the [Open Solver
Interface](https://github.com/coin-or/Osi) (Osi) to connect to LP and MIP
solvers, such as [Clp](https://github.com/coin-or/Clp) and
[Cbc](https://github.com/coin-or/Cbc).

FlopCpp is written in C++ and is released as open source under the [Eclipse Public License 2.0](http://www.opensource.org/licenses/EPL-2.0).

It is distributed under the auspices of the [COIN-OR Foundation](https://www.coin-or.org).

The FlopCpp development site is https://github.com/coin-or/FlopCpp.

## CURRENT BUILD STATUS

[![Windows Builds](https://github.com/coin-or/FlopCpp/actions/workflows/windows-ci.yml/badge.svg?branch=master)](https://github.com/coin-or/FlopCpp/actions/workflows/windows-ci.yml?query=branch%3Amaster)

[![Linux and MacOS Builds](https://github.com/coin-or/FlopCpp/actions/workflows/linux-ci.yml/badge.svg?branch=master)](https://github.com/coin-or/FlopCpp/actions/workflows/linux-ci.yml?query=branch%3Amaster)

## DOWNLOAD

### Docker image

There is a Docker image that provides FlopCpp, as well as other projects
in the [COIN-OR Optimization
Suite](https://github.com/coin-or/COIN-OR-OptimizationSuite) [here](https://hub.docker.com/r/coinor/coin-or-optimization-suite).

### Binaries

For newer releases, binaries will be made available as assets attached to
releases in Github
[here](https://github.com/coin-or/FlopCpp/releases). Older binaries
are archived as part of FlopC++
[here](https://www.coin-or.org/download/binary/FlopC++).

Due to license incompatibilities, pre-compiled binaries may lack some
functionality. If binaries are not available for your platform for the latest
version and you would like to request them to be built and posted, feel free
to open a discussion on Github.

### Source

Source code can be obtained either by

 * Downloading a snapshot of the source code for the latest release version of FlopCpp from the
 [releases](https://github.com/coin-or/FlopCpp/releases) page.
 * Cloning this repository from [Github](https://github.com/coin-or/FlopCpp) or 
 * Using the [coinbrew](https://github.com/coin-or/coinbrew) script to get the project and all dependencies (recommended, see below).   

Below is a quick start guide for building on common platforms. More detailed
build instructions are
[here](https://coin-or.github.io/user_introduction.html).

### Dependencies

FlopCpp has a number of dependencies, which are detailed in
[config.yml](.coin-or/config.yml). Dependencies on other COIN-OR projects are
automatically downloaded when obtaining the source with `coinbrew`. For some
of the remaining third-party dependencies, automatic download scripts and
build wrappers are provided (and will also be automatically run for required
and recommended dependencies), while other libraries that are easy to obtain
must be installed using an appropriate package manager (or may come with your
OS by default). 

## BUILDING from source

The quick start assumes you are in a bash shell. 

### Using `coinbrew`

To download and build FlopCpp from source, execute the 
following on the command line. 
```
wget https://raw.githubusercontent.com/coin-or/coinbrew/master/coinbrew
chmod u+x coinbrew
./coinbrew fetch FlopCpp@master
./coinbrew build FlopCpp
```
For more detailed instructions on coinbrew, see https://coin-or.github.io/coinbrew.
The `coinbrew` script will fetch the additional projects specified in the Dependencies section of [config.yml](.coin-or/config.yml).

### Without `coinbrew` (Expert users)

 * Download the source code, e.g., by cloning the git repo https://github.com/coin-or/FlopCpp
 * Download and install the source code for the dependencies listed in [config.yml](.coin-or/config.yml)
 * Build the code as follows (make sure to set `PKG_CONFIG_PATH` to the `lib/pkgconfig` directory of the installation of the dependencies).

```
./configure -C
make
make test
make install
```

### Examples

The `examples` directory has a number of models written with FLOPC++ (e.g.,
`transport`, `aircraft`, `sudoku`). After building and installing FLOPC++,
an example can be built and run in the `examples` subdirectory of the build
directory with
```
cd build/FlopCpp/master/examples
make EXNAME=transport
./transport
```
The script `runall` there builds and runs all of them.

## Doxygen Documentation

If you have `Doxygen` available, you can build a HTML documentation by typing

`make doxygen-docs` 

in the build directory. If FlopCpp was built via `coinbrew`, then the build
directory will be `./build/FlopCpp/master` by default. The doxygen documentation main file
is found at `<build-dir>/doxydoc/html/index.html`.


## Project Links

 * [Code of Conduct](https://www.coin-or.org/code-of-conduct/)
 * [COIN-OR Web Site](http://www.coin-or.org/)
 * [COIN-OR general discussion forum](https://github.com/orgs/coin-or/discussions)
 * [FlopCpp Discussion forum](https://github.com/coin-or/FlopCpp/discussions)
 * [Report a bug](https://github.com/coin-or/FlopCpp/issues/new)

