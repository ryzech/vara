project('archimedes-project', 'c',
        version : '0.1.0',
        license : 'MIT',
        meson_version : '>=1.8.0',
        default_options : [
            'c_std=c99',
            'warning_level=3',
            'werror=true',
        ])

cc = meson.get_compiler('c')
prefix = get_option('prefix')
buildtype = get_option('buildtype')

compiler_args = []
linker_args = []

host_os = host_machine.system().to_lower()
message('Host os: ' + host_os)

platform_linux = (
    host_os.contains('linux')
)

platform_windows = (
    host_os.contains('mingw') or
    host_os.contains('cygwin') or
    host_os.contains('windows')
)

platform_osx = (
    host_os.contains('machten') or
    host_os.contains('rhapsody') or
    host_os.contains('darwin')
)

osx_ldflags = []
if platform_osx
    add_languages('objc', native : false)
    osx_ldflags += ['-Wl,-framework,Foundation', '-Wl,-framework,AppKit', '-ObjC']
    add_project_link_arguments(osx_ldflags, language : ['objc', 'c'])
endif


# Add additional arguments based on buildtype (debug, release, dist, etc.)
if buildtype == 'debug'
    add_global_arguments('-D_DEBUG', language : 'c')
endif

if buildtype == 'release'
    add_global_arguments('-D_RELEASE', language : 'c')
endif

# Add global arguments regardless of buildtype
add_global_arguments('-D_VERSION="' + meson.project_version() + '"', language : 'c')

# Dependencies

# ---
# Add subdirectories
subdir('core')
subdir('sandbox')
