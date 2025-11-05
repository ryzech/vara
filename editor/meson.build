# Editor executable sources
editor_sources = files(
  'src/editor_main.c',
  # Add more source files here
)

# Build the executable
editor_exe = executable(
  'vara-editor',
  editor_sources,
  dependencies: [core_dep, runtime_dep, opengl_dep],
  install: true,
)

# Optional: Add an alias target
alias_target('vara-editor', editor_exe)
