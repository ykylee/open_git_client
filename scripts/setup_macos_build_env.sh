#!/usr/bin/env bash

set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "This setup script is intended for macOS hosts only." >&2
    exit 1
fi

if ! command -v xcode-select >/dev/null 2>&1; then
    echo "xcode-select was not found. Install Xcode first." >&2
    exit 1
fi

if ! xcode-select -p >/dev/null 2>&1; then
    echo "Xcode command line tools are not configured. Open Xcode once or run xcode-select --install." >&2
    exit 1
fi

if [[ -x /opt/homebrew/bin/brew ]]; then
    BREW_BIN="/opt/homebrew/bin/brew"
elif [[ -x /usr/local/bin/brew ]]; then
    BREW_BIN="/usr/local/bin/brew"
else
    echo "Homebrew was not found. Install Homebrew first: https://brew.sh" >&2
    exit 1
fi

eval "$("${BREW_BIN}" shellenv)"

required_formulae=(
    cmake
    ninja
    pkg-config
    wxwidgets
)

missing_formulae=()
for formula in "${required_formulae[@]}"; do
    if ! "${BREW_BIN}" list --versions "${formula}" >/dev/null 2>&1; then
        missing_formulae+=("${formula}")
    fi
done

if (( ${#missing_formulae[@]} > 0 )); then
    echo "Installing missing Homebrew formulae: ${missing_formulae[*]}"
    "${BREW_BIN}" install "${missing_formulae[@]}"
else
    echo "All required Homebrew formulae are already installed."
fi

wx_prefix="$("${BREW_BIN}" --prefix wxwidgets)"

cat <<EOF
macOS build environment is ready.

Recommended shell setup:
  eval "\$(${BREW_BIN} shellenv)"

Configure and build:
  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="${wx_prefix}"
  cmake --build build
  ctest --test-dir build --output-on-failure
EOF
