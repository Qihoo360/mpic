#pragma once

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
#include <iostream>
#include <memory>
#include <functional>
#endif // end of define __cplusplus

#ifdef _WIN32

#define usleep(us) Sleep((us)/1000)
#define snprintf  _snprintf
#define thread_local __declspec( thread )
#define umask _umask
#define mkdir _mkdir
#define access _access
#define getpid _getpid
#define sleep(sec) Sleep(sec*1000)

// 00
// Existence only
//
// 02
// Write-only
//
// 04
// Read-only
//
// 06
// Read and write
#define F_OK  0
#define W_OK  2
#define R_OK  4 /* read-only (for access()) */
#define RW_OK 6
#define X_OK  1
#define S_ISDIR(m)  (((m) & _S_IFMT) == _S_IFDIR)


#pragma warning( disable: 4005 ) // warning C4005 : 'va_copy' : macro redefinition
#pragma warning( disable: 4251 )
#pragma warning( disable: 4996 ) // warning C4996: 'strerror': This function or variable may be unsafe. Consider using strerror_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning( disable: 4244 4251 4355 4715 4800 4996 4005 4819)

#endif // end of _WIN32

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <Windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <dlfcn.h>
#endif

