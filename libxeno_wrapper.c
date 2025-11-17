#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

typedef void* (*PFN_vkGetInstanceProcAddr)(void*, const char*);

static const char *BASE_LOG_DIR =
    "/storage/emulated/0/Android/data/dev.eden.eden_emulator/files/xclipse_logs";

static FILE *g_log = NULL;
static PFN_vkGetInstanceProcAddr real_vkGetInstanceProcAddr = NULL;

static int mkdir_recursive(const char *path, mode_t mode) {
    if (!path || !*path) return -1;
    char tmp[1024];
    strncpy(tmp, path, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    size_t len = strlen(tmp);
    if (tmp[len-1] == '/') tmp[len-1] = '\0';
    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) && errno != EEXIST) return -1;
            *p = '/';
        }
    }
    if (mkdir(tmp, mode) && errno != EEXIST) return -1;
    return 0;
}

static void open_log_file() {
    if (g_log) return;
    mkdir_recursive(BASE_LOG_DIR, 0755);
    char path[1024];
    snprintf(path, sizeof(path), "%s/xclipse_log.txt", BASE_LOG_DIR);
    g_log = fopen(path, "a");
}

static void log_printf(const char *fmt, ...) {
    open_log_file();
    if (!g_log) return;
    time_t t = time(NULL);
    struct tm tm;
    gmtime_r(&t, &tm);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm);

    fprintf(g_log, "[%s] ", ts);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(g_log, fmt, ap);
    va_end(ap);
    fprintf(g_log, "\n");
    fflush(g_log);
}

static void resolve_real_vkGetInstanceProcAddr() {
    if (real_vkGetInstanceProcAddr) return;
    real_vkGetInstanceProcAddr =
        (PFN_vkGetInstanceProcAddr)dlsym(RTLD_NEXT, "vkGetInstanceProcAddr");

    if (!real_vkGetInstanceProcAddr) {
        void *h = dlopen("libvulkan.so", RTLD_LAZY | RTLD_LOCAL);
        if (h) {
            real_vkGetInstanceProcAddr =
                (PFN_vkGetInstanceProcAddr)dlsym(h, "vkGetInstanceProcAddr");
        }
    }

    if (real_vkGetInstanceProcAddr)
        log_printf("[XCLIPSE] Real vkGetInstanceProcAddr resolved");
    else
        log_printf("[XCLIPSE] WARNING: Real vkGetInstanceProcAddr not found");
}

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default")))
void xeno_init(void) {
    log_printf("[XCLIPSE] xeno_init called");
    resolve_real_vkGetInstanceProcAddr();
}

__attribute__((visibility("default")))
void* vkGetInstanceProcAddr(void* instance, const char* pName) {
    if (pName)
        log_printf("[XCLIPSE] vkGetInstanceProcAddr -> %s", pName);
    else
        log_printf("[XCLIPSE] vkGetInstanceProcAddr -> NULL");

    if (!real_vkGetInstanceProcAddr) {
        resolve_real_vkGetInstanceProcAddr();
        if (!real_vkGetInstanceProcAddr) return NULL;
    }

    return real_vkGetInstanceProcAddr(instance, pName);
}

#ifdef __cplusplus
}
#endif
