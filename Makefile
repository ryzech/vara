# Build configurations
BUILDS = debug release
BIN_DIR = bin

# Detect platform
UNAME_S := $(shell uname -s)

# Default target
all: build-debug

# Setup targets - these check if directory exists before setting up
setup-debug:
	@mkdir -p $(BIN_DIR)
	@if [ ! -f "$(BIN_DIR)/build-debug/build.ninja" ]; then \
		echo "Setting up debug build..."; \
		meson setup $(BIN_DIR)/build-debug --buildtype=debug; \
	fi
ifeq ($(UNAME_S),Darwin)
	@mkdir -p $(BIN_DIR)
	@if [ ! -d "$(BIN_DIR)/xcode" ]; then \
		echo "Setting up Xcode project..."; \
		meson setup $(BIN_DIR)/xcode --backend=xcode; \
	fi
endif

setup-release:
	@mkdir -p $(BIN_DIR)
	@if [ ! -f "$(BIN_DIR)/build-release/build.ninja" ]; then \
		echo "Setting up release build..."; \
		meson setup $(BIN_DIR)/build-release --buildtype=release; \
	fi
ifeq ($(UNAME_S),Darwin)
	@mkdir -p $(BIN_DIR)
	@if [ ! -f "$(BIN_DIR)/xcode/$(shell basename $(PWD)).xcodeproj/project.pbxproj" ]; then \
		echo "Setting up Xcode project..."; \
		meson setup $(BIN_DIR)/xcode --backend=xcode; \
	fi
endif

setup-all: setup-debug setup-release

# Build targets
build-debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug

build-release: setup-release
	meson compile -C $(BIN_DIR)/build-release

# Build specific targets
build-archimedes-debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug archimedes

build-archimedes-release: setup-release
	meson compile -C $(BIN_DIR)/build-release archimedes

build-sandbox-debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug sandbox

build-sandbox-release: setup-release
	meson compile -C $(BIN_DIR)/build-release sandbox

# Run targets - Fixed paths
run-debug: build-debug
	@if [ -f "$(BIN_DIR)/build-debug/sandbox/sandbox" ]; then \
		./$(BIN_DIR)/build-debug/sandbox/sandbox; \
	else \
		echo "Executable not found at $(BIN_DIR)/build-debug/sandbox/sandbox"; \
		echo "Contents of $(BIN_DIR)/build-debug/:"; \
		ls -la $(BIN_DIR)/build-debug/; \
		if [ -d "$(BIN_DIR)/build-debug/sandbox" ]; then \
			echo "Contents of $(BIN_DIR)/build-debug/sandbox/:"; \
			ls -la $(BIN_DIR)/build-debug/sandbox/; \
		fi; \
		exit 1; \
	fi

run-release: build-release
	@if [ -f "$(BIN_DIR)/build-release/sandbox/sandbox" ]; then \
		./$(BIN_DIR)/build-release/sandbox/sandbox; \
	else \
		echo "Executable not found at $(BIN_DIR)/build-release/sandbox/sandbox"; \
		echo "Contents of $(BIN_DIR)/build-release/:"; \
		ls -la $(BIN_DIR)/build-release/; \
		if [ -d "$(BIN_DIR)/build-release/sandbox" ]; then \
			echo "Contents of $(BIN_DIR)/build-release/sandbox/:"; \
			ls -la $(BIN_DIR)/build-release/sandbox/; \
		fi; \
		exit 1; \
	fi

# Test targets
test-debug: build-debug
	meson test -C $(BIN_DIR)/build-debug

test-release: build-release
	meson test -C $(BIN_DIR)/build-release

# Clean targets
clean:
	rm -rf $(BIN_DIR)

# Show help
help:
	@echo "Available targets:"
	@echo "  build-debug, build-release           - Build the respective version"
	@echo "  run-debug, run-release   - Build and run"
	@echo "  test-debug, test-release - Build and test"
	@echo "  build-archimedes-debug, build-archimedes-release - Build only the library"
	@echo "  build-sandbox-debug, build-sandbox-release - Build only the executable"
	@echo "  clean                    - Clean all build directories"
	@echo "  setup-all                - Setup all build directories"

.PHONY: all setup-debug setup-release setup-all \
 	build-debug build-release \
        build-archimedes-debug build-archimedes-release \
        build-sandbox-debug build-sandbox-release \
        run-debug run-release \
        test-debug test-release \
        clean help
