
# Compile on windows

## Compile glog on Windows

If you use Visual Studio 2015 or higher, you need to modify the code `glog-0.3.4/src/windows/port.cc` as bellow to avoid compiling error that `snprintf is difined`:

```diff
$ cd /the/path/to/mpic
$ cd 3rdparty/glog-0.3.4

weizili@weizili-L2 MINGW64 /d/git/mpic/3rdparty/glog-0.3.4 ((v0.3.4))
$ git diff src/windows/port.cc
diff --git a/src/windows/port.cc b/src/windows/port.cc
index bfa6e70..5b07c8a 100755
--- a/src/windows/port.cc
+++ b/src/windows/port.cc
@@ -55,6 +55,8 @@ int safe_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
   return _vsnprintf(str, size-1, format, ap);
 }

+// Fix VS2015 compile error
+#if _MSC_VER < 1900
 int snprintf(char *str, size_t size, const char *format, ...) {
   va_list ap;
   va_start(ap, format);
@@ -62,3 +64,4 @@ int snprintf(char *str, size_t size, const char *format, ...) {
   va_end(ap);
   return r;
 }
+#endif
```

Open the Visual Studio solution `glog-0.3.4/google-glog.sln` and compile the project `libglog_static` with DEBUG and RELEASER modes.

And then copy libglog_static.lib to [mpic]'s build system.

	$ cd /the/path/to/mpic
	$ mkdir -p vsprojects/bin/Debug vsprojects/bin/Release
	$ mkdir -p build/lib/Debug build/lib/Release
    $ cp 3rdparty/glog-0.3.4/Debug/libglog_static.lib   vsprojects/bin/Debug/glog.lib
    $ cp 3rdparty/glog-0.3.4/Release/libglog_static.lib vsprojects/bin/Release/glog.lib
    $ cp 3rdparty/glog-0.3.4/Debug/libglog_static.lib   build/lib/Debug/glog.lib
    $ cp 3rdparty/glog-0.3.4/Release/libglog_static.lib build/lib/Release/glog.lib

## Add evpp

    $ cd /the/path/to/mpic 
    $ cd 3rdparty/evpp

Following this guide [https://github.com/Qihoo360/evpp/blob/master/docs/quick_start_win32_vs2015.md](https://github.com/Qihoo360/evpp/blob/master/docs/quick_start_win32_vs2015.md) to compile [evpp] on windows platform.

    $ cp vsprojects/bin/Debug/*.*   ../../vsprojects/bin/Debug
    $ cp vsprojects/bin/Release/*.* ../../vsprojects/bin/Release
    $ cp vsprojects/bin/Debug/*.*   ../../build/lib/Debug/
    $ cp vsprojects/bin/Release/*.* ../../build/lib/Release/






- [evpp](https://github.com/nsqio/nsq)
- [glog](https://github.com/google/glog)
