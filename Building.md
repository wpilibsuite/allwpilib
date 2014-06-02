Building everything requires Maven
mvn package -DembeddedJDKHome=/home/patrick/Downloads/arm-jdk1.7.0_45/
TODO... Explain maven....
TODO.. how to import into eclipse correctly...

Building C++ only
------------------

C++ requires cmake if not run from maven, and is much faster. 
Make a new directory and then run:
```
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake
make # multicore add -j(num of cpu cores + 1), so -j3 on dual core for faster compile
make install DESTDIR=/some/dir/you/want/to/put/all/headers/and/libs #optional
``

Alternatively, if you like IDEs, you can import it directly into QtDeveloper, or a number of other IDEs such as Code::Blocks or Eclipse. See CMake documentation for details.
Eclipse demo:
```
cd ..
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../allwpilib/arm-toolchain.cmake .. -G "Eclipse CDT4 - Unix Makefiles"
```
and then import that directory into eclipse. Eclipse will detect a CDT project and standard tools will work.

GCC versions
------------
Update arm-toolchain.cmake if the triplet changes (eg using Ubuntu repo arm compiler is arm-linux-gnueabi) or in a non-standard location. Currently it assumes that the compiler is on the path.
