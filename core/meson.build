# Include directory for the library
core_inc = include_directories('include')

# Library sources
core_sources = files(
  'src/logger.c',
  'src/util/uuid.c',
  # Add more source files here
)

link_args = []

# Add platform-specific sources for MacOS.
if platform_osx
  link_args += osx_ldflags
  core_sources += files('src/platform/platform_darwin.m')
  core_sources += files('src/platform/platform_window_glfw.c')
endif

if platform_windows
  core_sources += files('src/platform/platform_win32.c')
  core_sources += files('src/platform/platform_window_glfw.c')
endif

if platform_linux
  core_sources += files('src/platform/platform_linux.c')
  core_sources += files('src/platform/platform_window_glfw.c')
endif

# Build the library
core_lib = static_library(
  'vara-core',
  core_sources,
  dependencies: glfw_dep,
  include_directories: core_inc,
  install: true,
  install_dir: get_option('libdir'),
)

# Create a dependency object for other targets to use
core_dep = declare_dependency(link_with: core_lib, include_directories: core_inc, link_args: link_args)

# Make the dependency available to parent scope
# This allows other subdirectories to use it
set_variable('core_dep', core_dep)
