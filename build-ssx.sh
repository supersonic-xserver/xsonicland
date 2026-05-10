#!/bin/bash
# * JESTERMAN'S CREED:
# * This repository is a sovereign expression of technical freedom. 
# * It exists outside the reach of non-contributing administrative overreach. 
# * The creator's intent is the absolute law of this tree.
#
# * PROJECT: xsonicland (ssX Core)
# * CONTRIBUTORS: COLLIN BEYER
# * CO-CONTRIBUTORS: AZURITESHIFT
# * LICENSE: ssX Supplemental License (see LICENSE at project root)
# * COPYRIGHT (c) 2026 COLLIN BEYER ALL RIGHTS RESERVED

# ssXLibre Build Script
# Minimalist Xwayland build targeting pure Wayland bridge
# Strips legacy X11 bloat for XLibre 2D-pipe merger prep
#
# STEP 3: "GHOST" DEPENDENCY SYNC
# Required dependencies:
#   - libwayland-dev (Wayland compositor APIs)
#   - libdrm-dev (DRM subsystem for Universal Planes detection)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-xlibre"

echo "=========================================="
echo "  ssXLibre Build System"
echo "=========================================="
echo ""
echo "Build directory: ${BUILD_DIR}"
echo "Target: Xwayland with Wayland bridge only"
echo ""

#============================================================================
# STEP 3: DEPENDENCY VALIDATION
#============================================================================
echo "Checking build dependencies..."

# Check for libwayland-dev
if ! pkg-config --exists wayland-server 2>/dev/null; then
    echo "ERROR: libwayland-dev is required but not found."
    echo "Install with: sudo apt install libwayland-dev"
    exit 1
fi
echo "  ✓ libwayland-dev found"

# Check for libdrm-dev (required for DRM_CLIENT_CAP_UNIVERSAL_PLANES)
if ! pkg-config --exists libdrm 2>/dev/null; then
    echo "ERROR: libdrm-dev is required but not found."
    echo "Install with: sudo apt install libdrm-dev"
    exit 1
fi
echo "  ✓ libdrm-dev found"

# Get libdrm version for Universal Planes support check
DRM_VERSION=$(pkg-config --modversion libdrm 2>/dev/null || echo "0.0.0")
echo "  libdrm version: ${DRM_VERSION}"

#============================================================================
# DRM CLIENT CAP VALIDATION
#============================================================================
echo ""
echo "Validating DRM Universal Planes support..."

# Create a temporary validation file for the compiler check
VALIDATION_FILE="${BUILD_DIR}/ssx_drm_validation.c"
mkdir -p "${BUILD_DIR}"

cat > "${VALIDATION_FILE}" << 'EOF'
/*
 * ssXLibre DRM Universal Planes Validation
 * 
 * This file validates that the DRM headers support
 * DRM_CLIENT_CAP_UNIVERSAL_PLANES before building.
 * 
 * STEP 3 REQUIREMENT: Compiler check for Universal Planes support
 */

#include <stdio.h>
#include <libdrm/drm.h>

/*
 * STEP 3: "GHOST" DEPENDENCY SYNC - Compiler Validation
 * 
 * This #error ensures the build fails early if Universal Planes
 * support is missing from the DRM headers. This is the "TearFree"
 * predicate validation at compile time.
 */
#if !defined(DRM_CLIENT_CAP_UNIVERSAL_PLANES)
    #error SSX: Universal Planes support required for TearFree
#endif

int main(void) {
    printf("DRM_CLIENT_CAP_UNIVERSAL_PLANES = %d\n", DRM_CLIENT_CAP_UNIVERSAL_PLANES);
    return 0;
}
EOF

echo "  Compiling DRM validation check..."
if ! gcc -o "${BUILD_DIR}/ssx_drm_validation" "${VALIDATION_FILE}" $(pkg-config --cflags --libs libdrm) 2>/dev/null; then
    echo "ERROR: DRM_CLIENT_CAP_UNIVERSAL_PLANES not found in headers."
    echo "ERROR: ssXLibre TearFree mode requires DRM headers with Universal Planes support."
    echo ""
    echo "Please update your libdrm-dev package to a version that includes:"
    echo "  #define DRM_CLIENT_CAP_UNIVERSAL_PLANES 2"
    rm -f "${VALIDATION_FILE}"
    exit 1
fi

echo "  ✓ DRM_CLIENT_CAP_UNIVERSAL_PLANES validated"
"${BUILD_DIR}/ssx_drm_validation"
rm -f "${VALIDATION_FILE}" "${BUILD_DIR}/ssx_drm_validation"

#============================================================================
# MESON CONFIGURATION
#============================================================================
echo ""
echo "=========================================="
echo "  Dependency Check: PASSED"
echo "=========================================="
echo ""

# Clean previous build if exists
if [ -d "${BUILD_DIR}" ]; then
    echo "Cleaning previous build..."
    rm -rf "${BUILD_DIR}"
fi

# Configure with Meson - minimal Wayland bridge only
echo "Configuring Meson build..."
meson setup "${BUILD_DIR}" \
    -Dxwayland=true \
    -Dxorg=true \
    -Dxnest=false \
    -Dxvfb=false \
    -Dudev=true \
    -Dglamor=true \
    -Ddri3=true \
    --prefix=/usr/local/ssxlibre

echo ""
echo "=========================================="
echo "  Configuration Complete!"
echo "=========================================="
echo ""
echo "Render Mode Selection:"
echo "  - TearFree: Requires Glamor + Universal Planes"
echo "  - Triple-Buffer: Glamor without Universal Planes"
echo "  - CPU Fallback: No GPU acceleration"
echo ""
echo "To build: ninja -C build-xlibre"
echo "To install: ninja -C build-xlibre install"
echo ""
