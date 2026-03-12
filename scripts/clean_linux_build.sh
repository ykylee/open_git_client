#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"

shopt -s nullglob

paths=(
  "${repo_root}/build"
  "${repo_root}"/cmake-build-*
)

removed_any=0

for path in "${paths[@]}"; do
  if [[ -e "${path}" ]]; then
    echo "Removing ${path}"
    if [[ -d "${path}" ]]; then
      find "${path}" -mindepth 1 -depth -exec rm -rf {} + 2>/dev/null || true
    fi
    rm -rf "${path}" 2>/dev/null || true

    if [[ -e "${path}" ]]; then
      echo "Failed to remove ${path}" >&2
      exit 1
    fi

    removed_any=1
  fi
done

if [[ "${removed_any}" -eq 0 ]]; then
  echo "No build directories found."
fi
