# Build configurations
BUILDS = debug release
BIN_DIR = bin

# Default target
all: debug

# Setup targets - these check if directory exists before setting up
setup-debug:
	@mkdir -p $(BIN_DIR)
	@if [ ! -f "$(BIN_DIR)/build-debug/build.ninja" ]; then \
		echo "Setting up debug build..."; \
		meson setup $(BIN_DIR)/build-debug --buildtype=debug; \
	fi

setup-release:
	@mkdir -p $(BIN_DIR)
	@if [ ! -f "$(BIN_DIR)/build-release/build.ninja" ]; then \
		echo "Setting up release build..."; \
		meson setup $(BIN_DIR)/build-release --buildtype=release; \
	fi

setup-all: setup-debug setup-release

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

# Install targets
install-debug: debug
	meson install -C $(BIN_DIR)/build-debug

install-release: release
	meson install -C $(BIN_DIR)/build-release

# Clean targets
clean:
	rm -rf $(BIN_DIR)

clean-debug:
	rm -rf $(BIN_DIR)/build-debug

clean-release:
	rm -rf $(BIN_DIR)/build-release

# Reconfigure targets
reconfig-debug:
	meson setup --reconfigure $(BIN_DIR)/build-debug --buildtype=debug

reconfig-release:
	meson setup --reconfigure $(BIN_DIR)/build-release --buildtype=release

# Wipe and rebuild targets (complete clean rebuild)
rebuild-debug:
	meson setup --wipe $(BIN_DIR)/build-debug --buildtype=debug

rebuild-release:
	meson setup --wipe $(BIN_DIR)/build-release --buildtype=release

# Debug info target
debug-info:
	@echo "=== Meson Version ==="
	@meson --version
	@echo ""
	@echo "=== Build Directory Status ==="
	@for build in $(BUILDS); do \
		echo "--- $(BIN_DIR)/build-$$build ---"; \
		if [ -d "$(BIN_DIR)/build-$$build" ]; then \
			echo "Directory exists"; \
			ls -la $(BIN_DIR)/build-$$build/ | head -10; \
			if [ -f "$(BIN_DIR)/build-$$build/build.ninja" ]; then \
				echo "✓ build.ninja exists"; \
			else \
				echo "✗ build.ninja missing"; \
			fi; \
			if [ -d "$(BIN_DIR)/build-$$build/sandbox" ]; then \
				echo "Contents of sandbox directory:"; \
				ls -la $(BIN_DIR)/build-$$build/sandbox/; \
			fi; \
		else \
			echo "Directory does not exist"; \
		fi; \
		echo; \
	done

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
	@echo "  reconfig-debug, reconfig-release - Reconfigure specific build"
	@echo "  rebuild-debug, rebuild-release - Completely rebuild from scratch"
	@echo "  debug-info               - Show build directory information"

.PHONY: all setup-debug setup-release setup-all debug release \
        archimedes-debug archimedes-release \
        sandbox-debug sandbox-release \
        run-debug run-release \
        test-debug test-release \
        install-debug install-release \
        clean clean-debug clean-release \
        reconfig-debug reconfig-release \
        rebuild-debug rebuild-release debug-info help
