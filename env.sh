#!/bin/bash

# Get the absolute path of the directory where env.sh is located
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Define paths relative to the project root
VCPKG_INSTALL_DIR="$ROOT_DIR/build/vcpkg_installed/x64-linux"

# Export the OpenSSL configuration path
export OPENSSL_CONF="$VCPKG_INSTALL_DIR/etc/ssl/openssl.cnf"

# Optional: Print paths for debugging
echo "Using OpenSSL config: $OPENSSL_CONF"
