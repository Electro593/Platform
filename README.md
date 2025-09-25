# Electro_593's Platform

A build system, set of libraries, and multi-platform libc replacement that I use for my 'handmade' projects. See [Voxarc](https://github.com/Electro593/Voxarc) for an example.

## Rationale

I believe that the best way to learn is to go as low-level as practically possible and recreate everything from the ground up. That means no libraries, no third-party code, and where remotely practical, no libc. It takes an inordinate amount of effort and time for even the most menial of functions (see `util/string.c:FString`), but the knowledge gained is worth the challenge.

### Constraints:
1. No third-party code. Taking inspiration and following publicly-available algorithms is permissible, but all code must by written manually. Furthermore, I must try to understand any code I follow to the best of my ability.
1. No third-party libraries. This includes all software compiled outside of this build system, with a few important exceptions: Platform interfaces (Win32, Syscalls) and graphics APIs (OpenGL, Vulkan, Wayland Server). `platform/linux` is also currently using `dlopen`/`dlload` because, as much as I'd like to be completely free of libc, my ELF loader currently doesn't handle transitive dependencies and OpenGL depends on libc. I'll get around to fixing this issue at some point.
1. No third-party processes except those used in the build system, such as `gcc` and `objdump`. This is an extension of constraint #2.
1. No system headers. All header definitions for allowed third-party libraries must be manually written and maintained. This is mostly to maintain the "purity" of the codebase, as well as to make sure all allowed third-party code is explicity declared and locatable.

## Build System

The build system treats directories within root `src` and `Platform/src` folders as modules, automatically detecting them and compiling based on the `main.c` present in each. Each module must be a unity build, e.g., all source and header files must be included into `main.c`. Below is a list of the provided modules:

* `platform`: This is the base module which (hot)loads and runs modules. It contains code for each supported platform, such as `linux` and `win32`, each of which interfacing with their respective kernel-provided functions. See [Platform's README](./docs/platform.md) for more details.
* `template`: This is not a true module, but rather acts as a convenient way to jumpstart a new one. Simply copy the template and substitute `$module$` with the module's name, keeping casing consistent. Then you should be ready to go!
* `util`: Contains a variety of utilities, libraries, and libc replacements that are generally useful for any project. See [Util's README](./docs/util.md) for more details on what it provides.
Some modules are ignored, such as `Platform/src/template`, since it's used to quickstart a new module by copying it and substituting `$MODULE$` with the module's name (respecting casing).
* `wayland`: Windowing functionality via wayland. Will probably be moved into `platform/linux` eventually, but for now it's its own module.

At startup, `platform` loads and initializes itself and `util`. It additionally expects a `base` module to be present; this is where your main logic and interfacing code should go.

### Module Design

Because causing eyes to bleed seems to be a profitable profession, in addition to modules being unity builds, they also contain no headers. Instead, most files separate the declaration and definition blocks into `#ifdef INCLUDE_HEADER` and `#ifdef INCLUDE_SOURCE` blocks. `main.c` then recursively includes itself with those flags set, effectively treating each file as both a header and source while maintaining the correct include order. Why was this decision made? I have no idea, ask me when I added it 5 years ago.

Modules can export five functions which are used for hotloading and interacting with other modules. If any of these functions aren't exported, they're treated as a no-op.

- `external void Load(platform_module *Platform, platform_module *Module)`

  Called when the module is loaded or reloaded. Its job is to initialize resources that must be available throughout the module's lifetime, as well as to load persistent module state into `Module->Data` and exported procedures into `Module->Funcs`. It's also used to load resources from other modules into the global namespace, such as exported `platform` and `util` functions. This is the expected time to load custom modules as well.

- `external void Init(platform_module *Platform)`

  Called once after the initial load step and never again after that, even during a hotreload. Note that modules are not initialized if they're loaded after the initial load step-- this will be fixed in the future, but is something to be aware of.

- `external void Update(platform_module *Platform)`

  Called repeatedly throughout the program's lifetime until termination. This is most useful for programs such as games, but can also be used as a simulation step.

- `external void Deinit(platform_module *Platform)`

  Called once during program termination, right before the final unload. Use it to clean up any resources that are persisted across reloads.

- `external void Unload(platform_module *Platform)`

  Called when a module is unloaded, which is caused by program termination or a hotreload. The module should clean up any resources tied to the module's existence and reopen them upon `Load`.

You might notice that `platform` doesn't export any of these procedures, and that's because it's the module that calls them. Instead, `platform` solely exports `external void Platform_Entry(void)` which the OS calls to start the process.

### Using It

You really shouldn't, this is designed for my personal projects and is liable to break if you don't have the same assumptions as me. However, if you're truly determined, you need to do the following:

1. Make sure you have `gcc`, `objdump`, and `gdb`. If you're on windows, download MSYS2 or something.
1. In your project directory, run `git submodule add https://github.com/Electro593/Platform`.
1. If you use VSCode, run `ln -s ./Platform/.vscode ./.vscode`. Otherwise, make the relevant config files for the IDE of your choice.

To see build options, run `./Platform/scripts/build.sh --help`. Currently only windows and linux are supported, only for X86_64. More architectures and platforms are planned, but I only have so much time to spend on this project. You can also just run the `Build Program (SH)`/`Build Program (MS)` tasks in VSCode, for linux and windows respectively.

Build results will be output into `./build/`:
- `assets/`: If your project directory contains an `assets` folder, it's copied into the build folder.
- `$module$.dump.asm`: Dumped assembly with comments for the given module.
- `$module$.dump.dat`: Dumped ELF metadata, headers, symbols, and relocations for the given module.
- `$module$.i`: Preprocessed code for the given module.
- `$module$[.so|.dll|.exe]`: The module's executable. The suffix depends on the target operating system and module: Non-`platform` modules are shared libraries, whereas `platform` is compiled as an executable.

To run the program, simply run `./platform [-RunTests]` in the `build/` directory. You can add additional arguments as needed, they can be retrieved in `Platform->Args`. You can also just run the `Run Program (SH)`/`BuilRun Program (MS)` tasks in VSCode, for linux and windows respectively.

To debug the program, make sure you have `gdb` and just hit `F5` in VSCode, making sure you have the right launch configuration selected. See [launch.json](./.vscode/launch.json) if you want to run it outside of vscode.

## License

There is no license! Hooray! You can go check [LICENSE](./LICENSE) if you don't believe me. Specifically, this code is UNLICENSED, which means I have voluntarily released it into the public domain with no restrictions or strings attached. Use anything in it however you like! At your own risk, of course.

If you do use any of this code, I don't expect any attribution, but it'd be nice if you credit me.

## Contributing

Since this is a personal project of mine, I won't accept any contributions, issues, or pull requests. You can't learn if someone else writes the code for you, after all!
