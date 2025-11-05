# Include directory for the library
opengl_inc = include_directories('include')

# Library sources
opengl_sources = files(
  'src/gl.c',
  # Add more source files here
)

link_args = []

# Build the library
opengl_lib = static_library(
  'opengl-renderer',
  opengl_sources,
  include_directories: opengl_inc,
  install: true,
  install_dir: get_option('libdir'),
)

# Create a dependency object for other targets to use
opengl_dep = declare_dependency(link_with: opengl_lib, include_directories: opengl_inc, link_args: link_args)

# Make the dependency available to parent scope
# This allows other subdirectories to use it
set_variable('opengl_dep', opengl_dep)
