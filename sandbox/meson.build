# Sandbox executable sources
sandbox_sources = files(
    'src/main.c',
    # Add more source files here
)

# Build the executable
sandbox_exe = executable('sandbox',
                         sandbox_sources,
                         dependencies : archimedes_dep,
                         install : true
)

# Optional: Add an alias target
alias_target('sandbox', sandbox_exe)
