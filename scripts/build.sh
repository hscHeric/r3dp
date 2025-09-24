#!/bin/bash

# ===================== Config =====================
# Definir o tipo de build (Debug ou Release)
BUILD_TYPE=${1:-Release}
BUILD_DIR="build/$BUILD_TYPE"

# ===================== Funções =====================
run_conan() {
  echo "==> Conan install ($BUILD_TYPE)"
  mkdir -p "$BUILD_DIR"
  conan install . --build=missing -s build_type="$BUILD_TYPE" -of="$BUILD_DIR"
}

run_cmake_configure() {
  echo "==> CMake configure"
  # Ajusta o caminho para o arquivo de toolchain
  cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="$BUILD_DIR/conan_toolchain.cmake" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$(pwd)/bin"
}

run_cmake_build() {
  echo "==> CMake build"
  cmake --build "$BUILD_DIR" --config "$BUILD_TYPE"
}

link_compile_commands() {
  echo "==> Link simbólico compile_commands.json"
  ln -sfn "$BUILD_DIR/compile_commands.json" compile_commands.json
}

copy_binaries() {
  echo "==> Copiando binários para $(pwd)/bin"
  mkdir -p bin
  cp -u "$BUILD_DIR/bin/"* bin/ 2>/dev/null || true
}

# ===================== Execução =====================
run_conan
run_cmake_configure
run_cmake_build
link_compile_commands
copy_binaries

echo "Build concluído com sucesso."
