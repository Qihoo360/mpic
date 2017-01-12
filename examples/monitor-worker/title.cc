#include "./title.h"

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char** environ;

namespace mpic {

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
bool Title::Init(int argc, char** argv) {
    return true;
}
void Title::Set(const std::string& title) {
    setproctitle("%s", title.c_str());
}

#elif defined(linux) || defined(__linux) || defined(__linux__)

namespace {
int g_argc = 0;
char** g_argv = NULL;
char* g_last = NULL;
}

bool Title::Init(int argc, char** argv) {
    assert(g_argv == NULL);
    g_argc = argc;
    g_argv = argv;

    size_t size = 0;
    for (int i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }
    char* p = (char*)malloc(size);
    if (!p) {
        return false;
    }

    g_last = g_argv[0];
    for (int i = 0; g_argv[i]; ++i) {
        if (g_last == g_argv[i]) {
            g_last = g_argv[i] + strlen(g_argv[i]) + 1;
        }
    }

    for (int i = 0; environ[i]; i++) {
        if (g_last == environ[i]) {
            size = strlen(environ[i]) + 1;
            g_last = environ[i] + size;
            strncpy(p, environ[i], size);
            environ[i] = p;
            p += size;
        }
    }

    g_last--;
    return true;
}

void Title::Set(const std::string& title) {
    g_argv[1] = NULL;
    strncpy(g_argv[0], title.c_str(), g_last - g_argv[0]);
    char* p = g_argv[0] + std::min(title.size(), (size_t)(g_last - g_argv[0]));

    if (g_last - p) {
        memset(p, 0, g_last - p);
    }
}

#else
bool Title::Init(int argc, char** argv) {
    return true;
}
void Title::Set(const std::string& title) {}
#endif

}

