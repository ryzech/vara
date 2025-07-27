# Archimedes
> Custom game engine written in pure C99.

>[!WARNING]  
>Do not use this. Currently, not production ready.

## Building
Prerequisites
- [Meson >1.8.0](https://mesonbuild.com)
- [Ninja >1.13.0](https://ninja-build.org/)
- (Optional) [Mise](https://mise.jdx.dev/)

For convenience, a [Makefile](./Makefile) is provided.

```shell
# Build Debug
make build-debug

# Build Release
make build-release

# Run Debug
make run-debug

# Run Release
make run-release

# Clean Build Files
make clean
```

The output will be under the `bin/` directory, either `bin/build-debug/` or `bin/build-release/` depending on which command you ran.

## TODO
- [ ] Implement Native MacOS windowing (ObjC, Cocoa.)
- [ ] Separate windowing into its own struct / file.

## License
This project is licensed under the [MIT License](https://mit-license.org/) as specified in [LICENSE](./LICENSE).
