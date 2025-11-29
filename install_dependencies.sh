#!/bin/bash

################################################################################
# FlopCpp Dependencies Installation Script
# 
# This script installs all required dependencies for FlopCpp by cloning
# repositories and building them from source.
#
# Usage: ./install_dependencies.sh
################################################################################

set -e  # Exit on error
set -u  # Exit on undefined variable

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Installation directory
INSTALL_PREFIX="${INSTALL_PREFIX:-/usr/local}"
BUILD_DIR="${BUILD_DIR:-$(pwd)/build_deps}"
LOG_DIR="${BUILD_DIR}/logs"

################################################################################
# Utility Functions
################################################################################

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

################################################################################
# Check for required build tools
################################################################################

check_build_tools() {
    log_info "Checking for required build tools..."
    
    local missing_tools=()
    local required_tools=("gcc" "g++" "make" "git" "autoconf" "automake" "libtool" "pkg-config")
    
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            missing_tools+=("$tool")
        else
            local version
            case "$tool" in
                gcc)
                    version=$(gcc --version | head -n1)
                    log_success "Found $tool: $version"
                    ;;
                g++)
                    version=$(g++ --version | head -n1)
                    log_success "Found $tool: $version"
                    ;;
                make)
                    version=$(make --version | head -n1)
                    log_success "Found $tool: $version"
                    ;;
                git)
                    version=$(git --version)
                    log_success "Found $tool: $version"
                    ;;
                *)
                    log_success "Found $tool"
                    ;;
            esac
        fi
    done
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required build tools: ${missing_tools[*]}"
        log_info "Please install missing tools. For Debian/Ubuntu, run:"
        echo "  sudo apt-get update"
        echo "  sudo apt-get install -y build-essential git autoconf automake libtool pkg-config"
        log_info "For RHEL/CentOS/Fedora, run:"
        echo "  sudo yum groupinstall 'Development Tools'"
        echo "  sudo yum install -y git autoconf automake libtool pkgconfig"
        exit 1
    fi
    
    log_success "All required build tools are available"
}



################################################################################
# Check write permissions for installation directory
################################################################################

check_permissions() {
    log_info "Checking write permissions for $INSTALL_PREFIX..."
    
    if [ ! -d "$INSTALL_PREFIX" ]; then
        log_warning "Directory $INSTALL_PREFIX does not exist"
        if ! mkdir -p "$INSTALL_PREFIX" 2>/dev/null; then
            log_error "Cannot create $INSTALL_PREFIX. You may need sudo privileges."
            log_info "Either run this script with sudo, or set INSTALL_PREFIX to a writable location:"
            echo "  export INSTALL_PREFIX=\$HOME/.local"
            echo "  ./install_dependencies.sh"
            exit 1
        fi
    fi
    
    if [ ! -w "$INSTALL_PREFIX" ]; then
        log_error "No write permission for $INSTALL_PREFIX"
        log_info "Either run this script with sudo, or set INSTALL_PREFIX to a writable location:"
        echo "  export INSTALL_PREFIX=\$HOME/.local"
        echo "  ./install_dependencies.sh"
        exit 1
    fi
    
    log_success "Write permissions verified for $INSTALL_PREFIX"
}

################################################################################
# Create build directory
################################################################################

setup_build_directory() {
    log_info "Setting up build directory: $BUILD_DIR"
    
    if [ -d "$BUILD_DIR" ]; then
        log_warning "Build directory already exists"
        read -p "Remove existing build directory and start fresh? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            log_info "Removing existing build directory..."
            rm -rf "$BUILD_DIR"
        else
            log_info "Keeping existing build directory (may cause conflicts)"
        fi
    fi
    
    mkdir -p "$BUILD_DIR"
    mkdir -p "$LOG_DIR"
    log_success "Build directory ready: $BUILD_DIR"
}

################################################################################
# Install BuildTools (special handling)
################################################################################

install_buildtools() {
    local name="BuildTools"
    local url="$1"
    local branch="$2"
    
    log_info "=========================================="
    log_info "Installing $name"
    log_info "=========================================="
    
    local repo_dir="$BUILD_DIR/$name"
    local log_file="$LOG_DIR/${name}.log"
    
    # Clone repository
    log_info "Cloning $name from $url (branch: $branch)..."
    if [ -d "$repo_dir" ]; then
        log_warning "Repository directory already exists: $repo_dir"
        log_info "Pulling latest changes..."
        cd "$repo_dir"
        if ! git pull >> "$log_file" 2>&1; then
            log_error "Failed to pull latest changes for $name"
            log_info "Check log file: $log_file"
            return 1
        fi
    else
        if ! git clone -b "$branch" "$url" "$repo_dir" >> "$log_file" 2>&1; then
            log_error "Failed to clone $name"
            log_info "Check log file: $log_file"
            return 1
        fi
        cd "$repo_dir"
    fi
    
    log_success "Repository cloned/updated successfully"
    
    # BuildTools uses run_autotools instead of configure/make/install
    log_info "BuildTools does not require installation, only cloning"
    log_info "BuildTools will be used to generate configure scripts for FlopCpp"
    
    # Detect the FlopCpp project directory
    local project_dir=""
    if [ -d "/workspaces/FlopCpp/FlopCpp" ]; then
        project_dir="/workspaces/FlopCpp/FlopCpp"
    elif [ -d "$(pwd)/../../FlopCpp" ]; then
        project_dir="$(cd $(pwd)/../../FlopCpp && pwd)"
    elif [ -d "$(pwd)/../FlopCpp" ]; then
        project_dir="$(cd $(pwd)/../FlopCpp && pwd)"
    else
        log_warning "Could not auto-detect FlopCpp project directory"
        log_info "To generate configure scripts for your project, run manually:"
        echo "  cd $repo_dir"
        echo "  ./run_autotools /path/to/your/FlopCpp/project"
        cd "$BUILD_DIR"
        return 0
    fi
    
    log_info "Detected FlopCpp project at: $project_dir"
    
    # Set COIN_AUTOTOOLS_DIR as recommended by BuildTools documentation
    export COIN_AUTOTOOLS_DIR="$INSTALL_PREFIX"
    log_info "Setting COIN_AUTOTOOLS_DIR=$COIN_AUTOTOOLS_DIR"
    log_info "Running run_autotools for FlopCpp project..."
    
    if [ -f "./run_autotools" ]; then
        if ! ./run_autotools "$project_dir" >> "$log_file" 2>&1; then
            log_error "run_autotools failed for FlopCpp project"
            log_info "Check log file: $log_file"
            log_info "You may need to run it manually:"
            echo "  cd $repo_dir"
            echo "  export COIN_AUTOTOOLS_DIR=$INSTALL_PREFIX"
            echo "  ./run_autotools $project_dir"
            return 1
        fi
        log_success "BuildTools successfully generated configure scripts for FlopCpp"
    else
        log_warning "run_autotools script not found in BuildTools"
        log_info "To generate configure scripts for your project, run manually:"
        echo "  cd $repo_dir"
        echo "  export COIN_AUTOTOOLS_DIR=$INSTALL_PREFIX"
        echo "  ./run_autotools $project_dir"
    fi
    
    cd "$BUILD_DIR"
    log_success "$name setup completed successfully"
    echo ""
    return 0
}

################################################################################
# Install a dependency from Git repository
################################################################################

install_git_dependency() {
    local name="$1"
    local url="$2"
    local branch="$3"
    
    log_info "=========================================="
    log_info "Installing $name"
    log_info "=========================================="
    
    local repo_dir="$BUILD_DIR/$name"
    local log_file="$LOG_DIR/${name}.log"
    
    # Clone repository
    log_info "Cloning $name from $url (branch: $branch)..."
    if [ -d "$repo_dir" ]; then
        log_warning "Repository directory already exists: $repo_dir"
        log_info "Pulling latest changes..."
        cd "$repo_dir"
        if ! git pull >> "$log_file" 2>&1; then
            log_error "Failed to pull latest changes for $name"
            log_info "Check log file: $log_file"
            return 1
        fi
    else
        if ! git clone -b "$branch" "$url" "$repo_dir" >> "$log_file" 2>&1; then
            log_error "Failed to clone $name"
            log_info "Check log file: $log_file"
            return 1
        fi
        cd "$repo_dir"
    fi
    
    log_success "Repository cloned/updated successfully"
    
    # Configure
    log_info "Configuring $name..."
    if [ ! -f "./configure" ]; then
        log_warning "configure script not found, checking for autogen.sh or similar..."
        if [ -f "./autogen.sh" ]; then
            log_info "Running autogen.sh..."
            if ! ./autogen.sh >> "$log_file" 2>&1; then
                log_error "autogen.sh failed for $name"
                log_info "Check log file: $log_file"
                return 1
            fi
        elif [ -f "./bootstrap" ]; then
            log_info "Running bootstrap..."
            if ! ./bootstrap >> "$log_file" 2>&1; then
                log_error "bootstrap failed for $name"
                log_info "Check log file: $log_file"
                return 1
            fi
        elif [ -f "configure.ac" ] || [ -f "configure.in" ]; then
            log_info "Running autoreconf..."
            if ! autoreconf -fi >> "$log_file" 2>&1; then
                log_error "autoreconf failed for $name"
                log_info "Check log file: $log_file"
                return 1
            fi
        else
            log_error "Cannot find configure script or way to generate it for $name"
            return 1
        fi
    fi
    
    if ! ./configure --prefix="$INSTALL_PREFIX" >> "$log_file" 2>&1; then
        log_error "Configuration failed for $name"
        log_info "Check log file: $log_file"
        return 1
    fi
    log_success "Configuration completed successfully"
    
    # Build
    log_info "Building $name (this may take a while)..."
    if ! make -j$(nproc) >> "$log_file" 2>&1; then
        log_warning "Parallel build failed, trying single-threaded build..."
        make clean >> "$log_file" 2>&1 || true
        if ! make >> "$log_file" 2>&1; then
            log_error "Build failed for $name"
            log_info "Check log file: $log_file"
            return 1
        fi
    fi
    log_success "Build completed successfully"
    
    # Install
    log_info "Installing $name..."
    if ! make install >> "$log_file" 2>&1; then
        log_error "Installation failed for $name"
        log_info "Check log file: $log_file"
        return 1
    fi
    log_success "Installation completed successfully"
    
    # Run ldconfig if installing to system directories
    if [[ "$INSTALL_PREFIX" == "/usr"* ]] || [[ "$INSTALL_PREFIX" == "/opt"* ]]; then
        log_info "Updating library cache..."
        if command -v ldconfig &> /dev/null; then
            ldconfig 2>/dev/null || log_warning "Could not run ldconfig (may need sudo)"
        fi
    fi
    
    cd "$BUILD_DIR"
    log_success "$name installed successfully"
    echo ""
    return 0
}



################################################################################
# Main installation process
################################################################################

main() {
    log_info "FlopCpp Dependencies Installation Script"
    log_info "Installation prefix: $INSTALL_PREFIX"
    log_info "Build directory: $BUILD_DIR"
    echo ""
    
    # Perform checks
    check_build_tools
    check_permissions
    
    setup_build_directory
    
    # Track success/failure
    local failed_deps=()
    local success_count=0
    local total_count=0
    
    # Install dependencies in order
    # BuildTools (special handling - uses run_autotools instead of configure/make/install)
    total_count=$((total_count + 1))
    if install_buildtools "https://github.com/coin-or-tools/BuildTools" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("BuildTools")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # CoinUtils
    total_count=$((total_count + 1))
    if install_git_dependency "CoinUtils" "https://github.com/coin-or/CoinUtils" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("CoinUtils")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # Osi
    total_count=$((total_count + 1))
    if install_git_dependency "Osi" "https://github.com/coin-or/Osi" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("Osi")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # Clp
    total_count=$((total_count + 1))
    if install_git_dependency "Clp" "https://github.com/coin-or/Clp" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("Clp")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # Cgl
    total_count=$((total_count + 1))
    if install_git_dependency "Cgl" "https://github.com/coin-or/Cgl" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("Cgl")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # Cbc
    total_count=$((total_count + 1))
    if install_git_dependency "Cbc" "https://github.com/coin-or/Cbc" "master"; then
        success_count=$((success_count + 1))
    else
        failed_deps+=("Cbc")
        log_warning "Continuing with remaining dependencies..."
    fi
    
    # Summary
    echo ""
    log_info "=========================================="
    log_info "Installation Summary"
    log_info "=========================================="
    log_info "Successfully installed: $success_count/$total_count"
    
    if [ ${#failed_deps[@]} -eq 0 ]; then
        log_success "All dependencies installed successfully!"
        log_info "Installation prefix: $INSTALL_PREFIX"
        log_info "Build logs: $LOG_DIR"
        
        # Update environment variables
        echo ""
        log_info "Updating environment variables..."
        
        export PKG_CONFIG_PATH="$INSTALL_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
        export LD_LIBRARY_PATH="$INSTALL_PREFIX/lib:$LD_LIBRARY_PATH"
        export PATH="$INSTALL_PREFIX/bin:$PATH"
        
        log_success "Environment variables updated for current session"
        
        # Add to shell profile for persistence
        local shell_profile=""
        if [ -n "$BASH_VERSION" ]; then
            if [ -f "$HOME/.bashrc" ]; then
                shell_profile="$HOME/.bashrc"
            elif [ -f "$HOME/.bash_profile" ]; then
                shell_profile="$HOME/.bash_profile"
            fi
        elif [ -n "$ZSH_VERSION" ]; then
            shell_profile="$HOME/.zshrc"
        fi
        
        if [ -n "$shell_profile" ]; then
            log_info "Adding environment variables to $shell_profile for future sessions..."
            
            # Check if already added to avoid duplicates
            if ! grep -q "# FlopCpp dependencies environment" "$shell_profile" 2>/dev/null; then
                cat >> "$shell_profile" << EOF

# FlopCpp dependencies environment (added by install_dependencies.sh)
export PKG_CONFIG_PATH="$INSTALL_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$INSTALL_PREFIX/lib:\$LD_LIBRARY_PATH"
export PATH="$INSTALL_PREFIX/bin:\$PATH"
EOF
                log_success "Environment variables added to $shell_profile"
                log_info "Run 'source $shell_profile' or restart your shell to apply changes"
            else
                log_info "Environment variables already present in $shell_profile"
            fi
        else
            log_warning "Could not detect shell profile file"
            log_info "Add these lines to your shell profile manually:"
            echo "  export PKG_CONFIG_PATH=$INSTALL_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH"
            echo "  export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:\$LD_LIBRARY_PATH"
            echo "  export PATH=$INSTALL_PREFIX/bin:\$PATH"
        fi
        
        return 0
    else
        log_error "Failed to install: ${failed_deps[*]}"
        log_info "Check the log files in: $LOG_DIR"
        return 1
    fi
}

################################################################################
# Run main function
################################################################################

main "$@"
exit $?
