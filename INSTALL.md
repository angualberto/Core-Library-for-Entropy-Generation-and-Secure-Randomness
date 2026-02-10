## Building AGLE

### CMake (Recommended)

```bash
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

### Makefile

```bash
make all
sudo make install PREFIX=/usr/local
```

### Manual

```bash
gcc -Iinclude -O3 -Wall -Wextra -c src/agle.c -o agle.o
ar rcs libagle.a agle.o
gcc -fPIC -shared -o libagle.so agle.o -lssl -lcrypto
```

## Installation Paths

After installation, headers and libraries are available at:

- Headers: `/usr/local/include/agle.h`
- Static library: `/usr/local/lib/libagle.a`
- Shared library: `/usr/local/lib/libagle.so`
- pkg-config: `/usr/local/lib/pkgconfig/agle.pc`

## Using AGLE in Your Project

### With CMake

```cmake
find_package(AGLE REQUIRED)
target_link_libraries(myapp PRIVATE AGLE::agle)
```

### With pkg-config

```bash
gcc myapp.c -o myapp $(pkg-config --cflags --libs agle)
```

### Manual linking

```bash
gcc myapp.c -o myapp -I/usr/local/include -L/usr/local/lib -lagle -lssl -lcrypto
```

## Uninstalling

### CMake

```bash
cmake --install build --prefix /usr/local --strip --component Uninstall
```

Or manually:

```bash
rm /usr/local/lib/libagle.a
rm /usr/local/lib/libagle.so*
rm /usr/local/include/agle.h
rm -rf /usr/local/lib/cmake/agle
rm /usr/local/lib/pkgconfig/agle.pc
```

### Makefile

```bash
sudo make uninstall PREFIX=/usr/local
```

## Platform-Specific Notes

### Linux

- Primary target, fully supported
- OpenSSL 3.0+ required
- `/dev/urandom` recommended

### macOS

- Requires OpenSSL via Homebrew:
  ```bash
  brew install openssl
  export LDFLAGS="-L/usr/local/opt/openssl/lib"
  export CPPFLAGS="-I/usr/local/opt/openssl/include"
  ```

### Windows

- Not directly supported
- WSL or MSYS2 recommended
- Requires Cygwin or similar for /dev/urandom

## Verification

After installation, verify with:

```bash
pkg-config --modversion agle
```

Or test compilation:

```bash
gcc -c test.c $(pkg-config --cflags --libs agle)
```
