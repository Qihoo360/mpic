
# Compile on windows

## Compile libgflags for windows

    $ cd 3rdparty/gflags-2.2.0
        modify CMakeList.txt  Add 'set (BUILD_SHARED_LIBS 1)' to build shared library.
    $ cmake -G "Visual Studio 14"
    $ start gflags.sln
    compile release and debug mode

And then copy gflags.dll and gflags.lib to mpic

    $ cp bin/Debug/gflags.* lib/Debug/gflags.* ../../msvc/bin/Debug/
    $ cp bin/Release/gflags.* lib/Release/gflags.* ../../msvc/bin/Release/

## Add evpp

    $ cd ../../
    $ pwd
    /the/root/path/of/mpic
    $ cp ../evpp/msvc/bin/Debug/*.* msvc/bin/Debug
    $ cp ../evpp/msvc/bin/Release/*.* msvc/bin/Release
