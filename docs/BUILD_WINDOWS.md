# Build (Windows, Qt6.5+, CMake 3.26+, Ninja/MSVC)

1) Install: Qt6 (Widgets/Network/Sql), MSVC Build Tools, Ninja, (optional) vcpkg + libssh2.
2) Configure project:
```bat
cd app
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" -DGRA_ENABLE_SSH=ON
cmake --build . --config Release
```
Output: `app/build/gra(.exe)`
