#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static FILE* log_file = NULL;

__attribute__((constructor))
void xeno_init()
{
    const char* path = "/sdcard/Android/data/dev.eden.eden_emulator/files/gpu_drivers/xclipse_log.txt";

    log_file = fopen(path, "w");
    if (log_file)
    {
        fprintf(log_file, "[XCLIPSE WRAPPER] Init OK\n");
        fflush(log_file);
    }
}

__attribute__((destructor))
void xeno_shutdown()
{
    if (!log_file) return;

    fprintf(log_file, "[XCLIPSE WRAPPER] Shutdown\n");
    fflush(log_file);
    fclose(log_file);
    log_file = NULL;
}

void xeno_log(const char* text)
{
    if (!log_file) return;
    fprintf(log_file, "%s\n", text);
    fflush(log_file);
}
