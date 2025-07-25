# Include directory for the library
core_inc = include_directories('include')

# Library sources
core_sources = files(
    'src/core.c',
    'src/logger.c',
    # Add more source files here
)

# Add platform-specific sources for MacOS.
if host_machine.system() == 'darwin'
    if add_languages('objc', native: false, required: true)
        core_sources += files('src/platform/platform_darwin.m')
    else
        warning('Objective-C compiler not found, skipping .m files.')
    endif
endif

# Build the library
core_lib = static_library('core',
                         core_sources,
                         include_directories : core_inc,
                         install : true,
                         install_dir : get_option('libdir')
)

# Create a dependency object for other targets to use
core_dep = declare_dependency(
    link_with : core_lib,
    include_directories : core_inc
)

# Make the dependency available to parent scope
# This allows other subdirectories to use it
set_variable('core_dep', core_dep)
