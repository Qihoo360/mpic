
# Compile on windows

## Compile libgflags for windows

    $ cd 3rdparty/gflags-2.2.0
        modify CMakeList.txt  Add 'set (BUILD_SHARED_LIBS 1)' to build shared library.
    $ cmake -G "Visual Studio 14"
    $ start gflags.sln
    compile release and debug mode

And then copy gflags.dll and gflags.lib to mpic

    $ cp bin/Debug/gflags.* lib/Debug/gflags.* ../../vsprojects/bin/Debug/
    $ cp bin/Release/gflags.* lib/Release/gflags.* ../../vsprojects/bin/Release/

## Add evpp

    $ cd ../../
    $ pwd
    /the/root/path/of/mpic
    $ cp ../evpp/vsprojects/bin/Debug/*.* vsprojects/bin/Debug
    $ cp ../evpp/vsprojects/bin/Release/*.* vsprojects/bin/Release
