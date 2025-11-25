Package: jsoncpp:x86-windows@1.9.6

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.44.35215.0
-    vcpkg-tool version: 2025-04-01-9c604254140797833b6f76908435c9fcbf09920e
    vcpkg-readonly: true
    vcpkg-scripts version: 4f8fe05871555c1798dbcb1957d0d595e94f7b57

**To Reproduce**

`vcpkg install `

**Failure logs**

```
-- Using cached open-source-parsers-jsoncpp-1.9.6.tar.gz
-- Cleaning sources at E:/GitHub/CrestEngine/CrestEngine/vcpkg_installed/x86-windows/vcpkg/blds/jsoncpp/src/1.9.6-29ceffc35f.clean. Use --editable to skip cleaning for the packages you specify.
-- Extracting source C:/Users/Dator/AppData/Local/vcpkg/downloads/open-source-parsers-jsoncpp-1.9.6.tar.gz
-- Using source at E:/GitHub/CrestEngine/CrestEngine/vcpkg_installed/x86-windows/vcpkg/blds/jsoncpp/src/1.9.6-29ceffc35f.clean
-- Found external ninja('1.12.1').
CMake Error at scripts/cmake/vcpkg_host_path_list.cmake:60 (message):
  Operation REMOVE_DUPLICATES not recognized.
Call Stack (most recent call first):
  E:/GitHub/CrestEngine/CrestEngine/vcpkg_installed/x86-windows/x64-windows/share/vcpkg-cmake/vcpkg_cmake_configure.cmake:246 (vcpkg_host_path_list)
  C:/Users/Dator/AppData/Local/vcpkg/registries/git-trees/dde49ad9d554fd5f062c0c1deea6e54b6fdb35b2/portfile.cmake:12 (vcpkg_cmake_configure)
  scripts/ports.cmake:203 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "crestengine",
  "version-string": "0.1.0",
  "dependencies": [
    "openxr-loader"
  ],
  "builtin-baseline": "cf1490f51190945a70666eaeeb6f63482da080b5"
}

```
</details>
