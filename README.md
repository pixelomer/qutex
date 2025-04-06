# qutex

Quick and dirty texture packer

## Usage

Texture packs can be created or read using the CLI. A minimal SDL3 interface is also provided for displaying individual sprites within a texture pack.

```
$ ./build/qutex
Usage:
  ./build/qutex pack <sprites/> <pack/>
  ./build/qutex view <pack/>
  ./build/qutex show <pack/>
```

Texture packs created with qutex can be loaded using `qutex::reader`.

```cpp
qutex::reader reader { "/path/to/texture-pack/" };
reader.read_all_sprites(
    [&](std::filesystem::path path, int width, int height) {
        // load full texture at path
    },
    [&](int x, int y, qutex::sprite_info const& info) {
        // load data about individual sprites within this texture
    }
);
```

See [main.cc](main.cc) for an example usage of qutex.
