# Build configurations
BUILDS = debug release
BIN_DIR = bin

# Detect platform
UNAME_S := $(shell uname -s)

# Default target
all: debug

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

# Open Xcode project (macOS only)
open-xcode: setup-debug
ifeq ($(UNAME_S),Darwin)
	@PROJECT_FILE=$$(find $(BIN_DIR)/xcode -name "*.xcodeproj" | head -n 1); \
	if [ -n "$$PROJECT_FILE" ]; then \
		echo "Opening Xcode project: $$PROJECT_FILE"; \
		open "$$PROJECT_FILE"; \
	else \
		echo "No Xcode project found in $(BIN_DIR)/xcode/"; \
	fi
else
	@echo "Xcode is only available on macOS"
endif

# Build targets
debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug

release: setup-release
	meson compile -C $(BIN_DIR)/build-release

# Build specific targets
archimedes-debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug archimedes

archimedes-release: setup-release
	meson compile -C $(BIN_DIR)/build-release archimedes

sandbox-debug: setup-debug
	meson compile -C $(BIN_DIR)/build-debug sandbox

sandbox-release: setup-release
	meson compile -C $(BIN_DIR)/build-release sandbox

# Run targets - Fixed paths
run-debug: debug
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

run-release: release
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
test-debug: debug
	meson test -C $(BIN_DIR)/build-debug

test-release: release
	meson test -C $(BIN_DIR)/build-release

# Clean targets
clean:
	rm -rf $(BIN_DIR)

clean-debug:
	rm -rf $(BIN_DIR)/build-debug

clean-release:
	rm -rf $(BIN_DIR)/build-release

# Show help
help:
	@echo "Available targets:"
	@echo "  debug, release           - Build the respective version"
	@echo "  run-debug, run-release   - Build and run"
	@echo "  test-debug, test-release - Build and test"
	@echo "  archimedes-debug, archimedes-release - Build only the library"
	@echo "  sandbox-debug, sandbox-release - Build only the executable"
	@echo "  clean                    - Clean all build directories"
	@echo "  clean-debug, clean-release - Clean specific build"
	@echo "  setup-all                - Setup all build directories"
ifeq ($(UNAME_S),Darwin)
	@echo ""
	@echo "macOS-specific targets:"
	@echo "  open-xcode               - Open Xcode project"
endif

.PHONY: all setup-debug setup-release setup-all debug release \
        archimedes-debug archimedes-release \
        sandbox-debug sandbox-release \
        run-debug run-release \
        test-debug test-release \
        install-debug install-release \
        clean clean-debug clean-release clean-xcode \
        open-xcode help
