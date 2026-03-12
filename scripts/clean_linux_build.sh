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
    rm -rf "${path}"
    removed_any=1
  fi
done

if [[ "${removed_any}" -eq 0 ]]; then
  echo "No build directories found."
fi
