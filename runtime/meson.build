# Include directory for the library
runtime_inc = include_directories('include')

# Library sources
runtime_sources = files(
  'src/application.c',
  # Add more source files here
)

link_args = []

# Build the library
runtime_lib = static_library(
  'runtime',
  runtime_sources,
  dependencies: core_dep,
  include_directories: runtime_inc,
  install: true,
  install_dir: get_option('libdir'),
)

# Create a dependency object for other targets to use
runtime_dep = declare_dependency(link_with: runtime_lib, include_directories: runtime_inc, link_args: link_args)

# Make the dependency available to parent scope
# This allows other subdirectories to use it
set_variable('runtime_dep', runtime_dep)
set_variable('runtime_inc_path', runtime_inc)
