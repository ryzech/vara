project('archimedes-project', 'c',
        version : '0.1.0',
        license : 'MIT',
        meson_version : '>=1.8.0',
        default_options : [
            'c_std=c99',
            'warning_level=3',
            'werror=true',
        ])

# Add additional arguments based on buildtype (debug, release, dist, etc.)
if get_option('buildtype') == 'debug'
    add_global_arguments('-D_DEBUG', language : 'c')
endif

if get_option('buildtype') == 'release'
    add_global_arguments('-D_RELEASE', language : 'c')
endif

# Add global arguments regardless of buildtype
add_global_arguments('-D_VERSION="' + meson.project_version() + '"', language: 'c')


# Add subdirectories
subdir('archimedes')
subdir('sandbox')
