project(
  'vara-engine',
  'c',
  version: '0.1.0',
  license: 'MIT',
  meson_version: '>=1.8.0',
  default_options: [
    'c_std=c99',
    'warning_level=3',
    'werror=true',
  ],
)

cc = meson.get_compiler('c')
buildtype = get_option('buildtype')
message('Build type: ' + buildtype)

compiler_args = []
linker_args = []

host_os = host_machine.system().to_lower()
message('Host os: ' + host_os)

platform_linux = (host_os.contains('linux'))

platform_windows = (
  host_os.contains('mingw')
  or host_os.contains('cygwin')
  or host_os.contains('windows')
)

platform_osx = (
  host_os.contains('machten')
  or host_os.contains('rhapsody')
  or host_os.contains('darwin')
)

osx_ldflags = []
if platform_osx
  add_languages('objc', native: false)
  osx_ldflags += [
    '-Wl,-framework,Foundation',
    '-Wl,-framework,AppKit',
    '-Wl,-framework,IOKit',
    '-Wl,-framework,Security',
    '-ObjC',
  ]
  add_project_link_arguments(osx_ldflags, language: ['objc', 'c'])
endif

# Add additional arguments based on buildtype (debug, release, dist, etc.)
if buildtype == 'debug'
  add_global_arguments('-DVARA_DEBUG', language: ['c', 'objc'])
  message('>> Debug mode enabled - VARA_DEBUG=1 defined')
elif buildtype == 'release'
  add_global_arguments('-DVARA_RELEASE', language: ['c', 'objc'])
  message('>> Release mode enabled - VARA_RELEASE=1 defined')
endif

# Add global arguments regardless of buildtype
add_global_arguments('-DVARA_VERSION="' + meson.project_version() + '"', language: ['c', 'objc'])
# Ignore gnu extension error, only way to allow optional macro argument.
# Needs tested to see if this will work on other platforms.
add_global_arguments('-Wno-gnu-zero-variadic-macro-arguments', language: ['c', 'objc'])
# Allow anonymous structs and unions.
add_global_arguments('-Wno-pedantic', language: ['c', 'objc'])
# Ignore unused variables (causes problems with debug logging)
add_global_arguments('-Wno-unused-variable', language: ['c', 'objc'])

# Dependencies

# ---
# Add subdirectories
subdir('core')
subdir('renderer')

subdir('sandbox')
subdir('editor')
