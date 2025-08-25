# Makefile simples para Conan + CMake
# uso:
#   make              # build padrão (Release)
#   make BUILD_TYPE=Debug
#   make install
#   make clean / make distclean

# ===================== Config =====================
PROJECT_NAME := r3dp

BUILD_TYPE ?= Release
BUILD_DIR  := build/$(BUILD_TYPE)
GEN_DIR    := $(BUILD_DIR)/generators

# Binários irão para ./bin (absoluto evita problema de ln/cmake)
PREFIX_BIN := $(abspath bin)

# Flags de Release (O3)
CXX_RELFLAGS ?= -O3 -DNDEBUG

# ===================== Alvos ======================
.PHONY: all conan configure build link-compile-commands install run clean distclean

all: build

conan:
	@echo "==> Conan install ($(BUILD_TYPE))"
	@mkdir -p "$(BUILD_DIR)"
	conan install . -s build_type=$(BUILD_TYPE) -of="$(BUILD_DIR)"

configure: conan
	@echo "==> CMake configure"
	cmake -S . -B "$(BUILD_DIR)" \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_TOOLCHAIN_FILE="$(GEN_DIR)/conan_toolchain.cmake" \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$(PREFIX_BIN)" \
		-DCMAKE_CXX_FLAGS_RELEASE="$(CXX_RELFLAGS)"

build: configure
	@echo "==> CMake build"
	cmake --build "$(BUILD_DIR)" --config $(BUILD_TYPE)
	$(MAKE) link-compile-commands

link-compile-commands:
	@echo "==> Link simbólico compile_commands.json"
	@ln -sfn "$(BUILD_DIR)/compile_commands.json" compile_commands.json

install: build
	@echo "==> CMake install (prefix = projeto)"
	cmake --install "$(BUILD_DIR)" --config $(BUILD_TYPE) --prefix "$(abspath .)"

run: build
	"$(PREFIX_BIN)/$(PROJECT_NAME)"

clean:
	@echo "==> Limpando build da config $(BUILD_TYPE)"
	@rm -rf "$(BUILD_DIR)"

distclean:
	@echo "==> Limpando tudo"
	@rm -rf build compile_commands.json bin
