
# libgflags



$ cd 3rdparty/gflags-2.2.0
modify CMakeList.txt 
$ cmake -G "Visual Studio 14"
$ start gflags.sln
compile release and debug mode

and then copy gflags.dll and gflags.lib to mpic
$ cp bin/Debug/gflags.* lib/Debug/gflags.* ../../msvc/bin/Debug/
$ cp bin/Release/gflags.* lib/Release/gflags.* ../../msvc/bin/Release/


