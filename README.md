Given a set of possibly overlapping rectangles, `split_rectangles` returns a new
set of rectangles guaranteed to not overlap.

Compiling
---------

Install [catch2](https://github.com/catchorg/Catch2) and
[boost](https://www.boost.org) through
[vcpkg](https://github.com/microsoft/vcpkg), then run:

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg-dir>/scripts/buildsystems/vcpkg.cmake ..
```
