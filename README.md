>
> Copyright Raffaele Rossi 2023 - 2024. 
>
> Distributed under the Boost Software License, Version 1.0.  
> (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)  
>

# DepC

Like C and C++ but with Dependent Types.

For more documentation, see [docs/](docs/README.md).

## Dep0

It is the bootstrapping compiler written in C++.

# Getting in Touch

If you need help or have any question feel free to reach out to me directly on Discord @raffopazzo6911

# How to build using devbox

First you need to build the devbox docker image via:

(NB if you're behind a proxy you might need to edit the Dockerfile)

```
$ ./tools/devbox/build-image.sh
```

Then you can run conan and cmake as normal within a devbox environment:

```
$ ./tools/devbox/devbox.sh
...$ conan profile new --detect default
...$ conan profile update settings.compiler.libcxx=libstdc++11 default
...$ conan install . --install-folder build --build=antlr4-cppruntime --build=boost --build=llvm-core
...$ cmake . -Bbuild -DCMAKE_MODULE_PATH=$PWD/build
...$ cmake --build build -j
...$ ctest --test-dir build --output-on-failure --timeout 30
...$ exit
```

Alternatively:

```
$ ./tools/devbox/devbox.sh conan profile new --detect default
$ ./tools/devbox/devbox.sh conan profile update settings.compiler.libcxx=libstdc++11 default
$ ./tools/devbox/devbox.sh conan install . --install-folder build --build=antlr4-cppruntime --build=boost --build=llvm-core
$ ./tools/devbox/devbox.sh cmake . -Bbuild -DCMAKE_MODULE_PATH=$PWD/build
$ ./tools/devbox/devbox.sh cmake --build build -j
$ ./tools/devbox/devbox.sh ctest --test-dir build --output-on-failure --timeout 30
```

# How to without devbox

You will need:
 - conan 1.55 (conan 2 is currently not compatible with llvm-core/13)
 - cmake 3.20 or greater
 - gcc11 or greater (other compilers have not be tested yet)

You shoud then be able to follow the same `conan/cmake/ctest` steps documented above.
