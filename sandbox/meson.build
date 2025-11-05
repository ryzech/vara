# Sandbox executable sources
sandbox_sources = files(
  'src/main.c',
  # Add more source files here
)

# Build the executable
sandbox_exe = executable(
  'vara-sandbox',
  sandbox_sources,
  dependencies: [core_dep, runtime_dep, opengl_dep],
  install: true,
)

# Optional: Add an alias target
alias_target('vara-sandbox', sandbox_exe)
