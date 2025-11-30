# FlopCpp Installation Guide

This guide provides detailed instructions for building and installing FlopCpp and all its dependencies using the automated installation script.

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Installation Details](#installation-details)
- [Configuration Options](#configuration-options)
- [What Gets Installed](#what-gets-installed)
- [Troubleshooting](#troubleshooting)
- [Manual Installation](#manual-installation)
- [Uninstallation](#uninstallation)

## Overview

The `install.sh` script automates the complete build and installation process for FlopCpp, including:

1. **Dependency Management**: Clones and builds all required COIN-OR libraries
2. **Build Tools Setup**: Configures BuildTools for autotools generation
3. **FlopCpp Compilation**: Builds and installs FlopCpp itself
4. **Environment Configuration**: Automatically sets up environment variables
5. **Testing**: Runs test suites to verify the installation

## Prerequisites

### Required Build Tools

The script checks for and requires the following tools:

- **gcc** - GNU C compiler
- **g++** - GNU C++ compiler
- **gfortran** - GNU Fortran compiler
- **make** - Build automation tool
- **git** - Version control system
- **autoconf** - Generate configuration scripts
- **automake** - Generate Makefile.in files
- **libtoolize** - Library support script
- **pkg-config** - Package configuration helper

### Installing Prerequisites

**On Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential git autoconf automake libtool pkg-config gfortran
```

**On RHEL/CentOS/Fedora:**
```bash
sudo yum groupinstall 'Development Tools'
sudo yum install -y git autoconf automake libtool pkgconfig gcc-gfortran
```

## Quick Start

The simplest way to install FlopCpp:

```bash
cd /path/to/FlopCpp
./install.sh
```

This will:
- Install everything to `$HOME/.local`
- Build all dependencies in `./build_deps`
- Configure your shell environment automatically

## Installation Details

### Step-by-Step Process

1. **Build Tool Verification**
   - Checks for all required tools
   - Displays version information for each tool
   - Exits with helpful error messages if tools are missing

2. **Permission Checks**
   - Verifies write access to installation directory
   - Creates directory if it doesn't exist
   - Provides alternative options if permissions are insufficient

3. **Build Directory Setup**
   - Creates `build_deps` directory for compilation
   - Prompts to remove existing build directory if found
   - Creates `build_deps/logs` for detailed build logs

4. **Dependency Installation** (in order)
   - **BuildTools**: COIN-OR build system utilities
   - **CoinUtils**: COIN-OR utility library
   - **Osi**: Open Solver Interface
   - **Clp**: COIN-OR Linear Programming solver
   - **Cgl**: Cut Generation Library
   - **Cbc**: COIN-OR Branch and Cut solver

5. **FlopCpp Compilation**
   - Auto-detects FlopCpp project directory
   - Runs `./configure` with appropriate prefix
   - Builds with parallel compilation (falls back to single-threaded if needed)
   - Runs test suite
   - Installs to specified prefix

6. **Environment Configuration**
   - Exports environment variables for current session
   - Adds permanent configuration to shell profile (~/.bashrc or ~/.zshrc)
   - Updates `PKG_CONFIG_PATH`, `LD_LIBRARY_PATH`, and `PATH`

## Configuration Options

### Custom Installation Prefix

By default, everything installs to `$HOME/.local`. To change this:

```bash
export INSTALL_PREFIX=/custom/path
./install.sh
```

**Examples:**

```bash
# Install to /usr/local (requires sudo)
export INSTALL_PREFIX=/usr/local
sudo ./install.sh

# Install to a custom directory
export INSTALL_PREFIX=$HOME/flopcpp
./install.sh

# Install to /opt
export INSTALL_PREFIX=/opt/flopcpp
sudo ./install.sh
```

### Custom Build Directory

Change where temporary build files are stored:

```bash
export BUILD_DIR=/tmp/flopcpp_build
./install.sh
```

### Non-Interactive Mode

For automated/CI environments, skip the interactive prompt:

```bash
# Remove existing build directory automatically
rm -rf build_deps
./install.sh
```

## What Gets Installed

### Directory Structure

After installation with default prefix `$HOME/.local`:

```
$HOME/.local/
├── bin/              # Executables
├── include/          # Header files
│   └── coin/        # COIN-OR headers
├── lib/              # Libraries
│   ├── libCbc.so
│   ├── libCgl.so
│   ├── libClp.so
│   ├── libCoinUtils.so
│   ├── libOsi.so
│   └── pkgconfig/   # pkg-config files
└── share/            # Documentation and examples
```

### Build Artifacts

The script creates the following during installation:

```
./build_deps/
├── BuildTools/       # COIN-OR BuildTools (cloned)
├── CoinUtils/        # CoinUtils source and build
├── Osi/              # Osi source and build
├── Clp/              # Clp source and build
├── Cgl/              # Cgl source and build
├── Cbc/              # Cbc source and build
└── logs/             # Build logs for debugging
    ├── BuildTools.log
    ├── CoinUtils.log
    ├── Osi.log
    ├── Clp.log
    ├── Cgl.log
    ├── Cbc.log
    └── FlopCpp.log
```

### Environment Variables

The script configures the following environment variables:

- **PKG_CONFIG_PATH**: Allows pkg-config to find installed libraries
- **LD_LIBRARY_PATH**: Enables dynamic linker to find shared libraries
- **PATH**: Adds installed executables to your command path

These are automatically added to your shell profile (`~/.bashrc` or `~/.zshrc`).

## Troubleshooting

### Build Failures

If a build fails, check the corresponding log file:

```bash
# View the log for a failed component
cat build_deps/logs/CoinUtils.log

# Check FlopCpp build log
cat build_deps/logs/FlopCpp.log
```

### Common Issues

**Issue: "Missing required build tools"**
```
Solution: Install the missing tools using your package manager
```

**Issue: "No write permission for /usr/local"**
```
Solution: Either use sudo or set INSTALL_PREFIX to a writable location:
export INSTALL_PREFIX=$HOME/.local
./install.sh
```

**Issue: "configure script not found"**
```
Solution: BuildTools may have failed to generate configure scripts.
Check build_deps/logs/BuildTools.log for details.
```

**Issue: "Parallel build failed"**
```
Solution: The script automatically retries with single-threaded build.
This is normal and not an error.
```

**Issue: Test failures**
```
Solution: Test failures are marked as non-critical and don't stop installation.
However, you may want to investigate if many tests fail.
```

### Cleaning Up

To start fresh:

```bash
# Remove build directory
rm -rf build_deps

# Remove installation (if using default prefix)
rm -rf $HOME/.local/lib/libCbc* \
       $HOME/.local/lib/libCgl* \
       $HOME/.local/lib/libClp* \
       $HOME/.local/lib/libCoin* \
       $HOME/.local/lib/libOsi* \
       $HOME/.local/include/coin

# Re-run installation
./install.sh
```

## Manual Installation

If you prefer to install components manually:

### Dependencies

```bash
# Clone and build each dependency
for pkg in CoinUtils Osi Clp Cgl Cbc; do
    git clone https://github.com/coin-or/$pkg
    cd $pkg
    ./configure --prefix=$HOME/.local
    make -j$(nproc)
    make test
    make install
    cd ..
done
```

### FlopCpp

```bash
cd FlopCpp
./configure --prefix=$HOME/.local
make -j$(nproc)
make test
make install
```

### Environment Setup

Add to your `~/.bashrc` or `~/.zshrc`:

```bash
export PKG_CONFIG_PATH="$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$HOME/.local/lib:$LD_LIBRARY_PATH"
export PATH="$HOME/.local/bin:$PATH"
```

Then reload:
```bash
source ~/.bashrc
```

## Uninstallation

To remove FlopCpp and its dependencies:

```bash
# If installed to $HOME/.local
cd $HOME/.local

# Remove libraries
rm -rf lib/libCbc* lib/libCgl* lib/libClp* lib/libCoin* lib/libOsi*

# Remove headers
rm -rf include/coin

# Remove pkg-config files
rm -rf lib/pkgconfig/cbc.pc lib/pkgconfig/cgl.pc lib/pkgconfig/clp.pc \
       lib/pkgconfig/coinutils.pc lib/pkgconfig/osi*.pc

# Remove executables (if any)
rm -f bin/cbc bin/clp

# Remove environment variables from shell profile
# Edit ~/.bashrc or ~/.zshrc and remove the lines added by install.sh
```

## Advanced Usage

### Development Installation

For development work, you might want to keep the build directories:

```bash
export BUILD_DIR=$HOME/flopcpp_dev
./install.sh
```

This allows you to:
- Modify source code in `$BUILD_DIR/`
- Rebuild individual components
- Test changes without re-cloning

### Parallel Installations

Install multiple versions by using different prefixes:

```bash
# Stable version
export INSTALL_PREFIX=$HOME/flopcpp-stable
./install.sh

# Development version
export INSTALL_PREFIX=$HOME/flopcpp-dev
./install.sh
```

Switch between them by modifying your PATH:
```bash
export PATH=$HOME/flopcpp-stable/bin:$PATH  # Use stable
export PATH=$HOME/flopcpp-dev/bin:$PATH     # Use dev
```

## Getting Help

If you encounter issues:

1. Check the log files in `build_deps/logs/`
2. Review the error messages in the console output
3. Verify all prerequisites are installed correctly
4. Try with a clean build directory
5. Consider using a different installation prefix

For bugs or questions about the installation script, please open an issue on the FlopCpp GitHub repository.

## Summary

The `install.sh` script provides an automated way to build and install FlopCpp with all its dependencies. It handles:

- ✅ Dependency resolution
- ✅ Parallel compilation
- ✅ Error recovery
- ✅ Environment configuration
- ✅ Comprehensive logging

For most users, simply running `./install.sh` is sufficient to get a working FlopCpp installation.
