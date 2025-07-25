# Include directory for the library
archimedes_inc = include_directories('include')

# Library sources
archimedes_sources = files(
    'src/archimedes.c',
    'src/logger.c',
    # Add more source files here
)

# Build the library
archimedes_lib = static_library('archimedes',
                         archimedes_sources,
                         include_directories : archimedes_inc,
                         install : true,
                         install_dir : get_option('libdir')
)

# Install headers
install_headers('include/archimedes/archimedes.h',
                subdir : 'archimedes'
)

# Create a dependency object for other targets to use
archimedes_dep = declare_dependency(
    link_with : archimedes_lib,
    include_directories : archimedes_inc
)

# Make the dependency available to parent scope
# This allows other subdirectories to use it
set_variable('archimedes_dep', archimedes_dep)
