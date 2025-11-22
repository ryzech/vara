# Vara
> Custom game engine written in (mostly) pure C99.

>[!WARNING]  
>Do not use this. Nowhere near production ready!

## Inspirations
Definitely an engine inspired by Travis Vroman and the [Kohi Engine](https://kohiengine.com), I originally followed his series but went on a different path early on.

Also inspired by the [Hazel Engine](https://hazelengine.com) and some of the series was referenced for renderer architecture.

## Building
Prerequisites
- [CMake >3.20](https://cmake.org)
- (Optional) [Ninja >1.13.0](https://ninja-build.org/)

Scripts are provided under [build](./build), for example macOS has a `build/macos` folder, while Linux has a `build/linux` folder.

In the below example the scripts used are for macOS but all platforms are provided.
Windows uses a `.bat` file though.

Or... just use an IDE (CLion, Visual Studio, all should have CMake support.)

```shell
# Build Debug (macOS)
./build/macos/build-debug.sh

# Build Release
./build/macos/build-release.sh

# Run Sandbox Executable (replace Debug with build type.
./build/debug/sandbox/vara-sandbox

# Run Editor Executable (replace Debug with build type.
./build/debug/editor/vara-editor

# Clean Debug (works for Release too)
./build/macos/build-debug.sh clean
```

The output will be under the `build/` directory, either `build/debug` or `build/release` depending on which command you run.

## License
This project is licensed under the [MIT License](https://mit-license.org/) as specified in [LICENSE](./LICENSE).
