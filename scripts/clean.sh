#!/bin/bash

# ===================== Config =====================
ACTION=${1:-clean}
BUILD_TYPE=${2:-Release}

BUILD_DIR="build/$BUILD_TYPE"
PROJECT_BIN_DIR="$(pwd)/bin"

# ===================== Funções =====================
clean() {
  echo "==> Limpando build da config $BUILD_TYPE e bin/"
  rm -rf "$BUILD_DIR"
  rm -rf "$PROJECT_BIN_DIR"
}

distclean() {
  echo "==> Limpando tudo (build/, bin/, compile_commands.json, CMakeUserPresets.json, .cache/)"
  rm -rf build
  rm -rf "$PROJECT_BIN_DIR"
  rm -rf .cache
  rm -f compile_commands.json
  rm -f CMakeUserPresets.json
}

# ===================== Execução =====================
case "$ACTION" in
clean)
  clean
  ;;
distclean)
  distclean
  ;;
*)
  echo "Uso: $0 {clean|distclean} [Debug|Release]"
  exit 1
  ;;
esac

echo "Limpeza concluída."
