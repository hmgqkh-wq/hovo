// libxeno_wrapper.c
// Minimal Android-safe wrapper that EXPORTS xeno_init so Eden will call it.
// Writes a simple test file to external storage so we can verify the wrapper ran.

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Make symbol visible so manifest "entrypoint": "xeno_init" matches.
__attribute__((visibility("default")))
void xeno_init(void)
{
    FILE* f = fopen("/storage/emulated/0/xclipse_log.txt", "a");
    if (f) {
        fprintf(f, "[XCLIPSE] xeno_init called\n");
        fflush(f);
        fclose(f);
    }
}

// Optional exported shutdown (not strictly required, but useful)
__attribute__((visibility("default")))
void xeno_shutdown(void)
{
    FILE* f = fopen("/storage/emulated/0/xclipse_log.txt", "a");
    if (f) {
        fprintf(f, "[XCLIPSE] xeno_shutdown called\n");
        fflush(f);
        fclose(f);
    }
}

#ifdef __cplusplus
}
#endif
