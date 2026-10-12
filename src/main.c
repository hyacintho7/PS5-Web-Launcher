/* SPDX-License-Identifier: GPL-3.0-or-later
 * Launcher registration adapted from owendswang/ps5-web-file-manager,
 * src/app_installer.c (3659cbbc40022e32229f9919f045018a988f2ef5).
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ps5/kernel.h>
#include "assets.h"

#define TITLE_ID "BDUL00001"
#define BASE "/user/app/" TITLE_ID
#define SYS BASE "/sce_sys"
int sceAppInstUtilInitialize(void);
int sceAppInstUtilAppInstallAll(void *);
typedef struct { char unused[45]; char message[3075]; } notification_t;
int sceKernelSendNotificationRequest(int, notification_t *, size_t, int);

static void notify(const char *message) {
    notification_t req = {0};
    snprintf(req.message, sizeof(req.message), "%s", message);
    puts(message);
    sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
}

static int directory(const char *path) {
    struct stat st;
    if (mkdir(path, 0755) == 0) return 0;
    if (errno == EEXIST && stat(path, &st) == 0 && S_ISDIR(st.st_mode)) return 0;
    perror(path);
    return -1;
}

/* Never overwrite an existing title's assets. Retry registration on each run. */
static int write_missing(const char *path, const unsigned char *data, size_t size) {
    struct stat st;
    if (lstat(path, &st) == 0) {
        if (!S_ISREG(st.st_mode) || st.st_size != (off_t)size) {
            fprintf(stderr, "Existing asset differs: %s\n", path);
            return -1;
        }
        FILE *f = fopen(path, "rb");
        if (!f) return -1;
        int same = 1;
        for (size_t i = 0; i < size; ++i) {
            if (fgetc(f) != data[i]) { same = 0; break; }
        }
        fclose(f);
        return same ? 0 : -1;
    }
    if (errno != ENOENT) return -1;
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    FILE *f = fopen(tmp, "wb");
    if (!f) return -1;
    int failed = fwrite(data, 1, size, f) != size;
    if (fclose(f) != 0) failed = 1;
    if (failed) { unlink(tmp); return -1; }
    if (rename(tmp, path) != 0) { unlink(tmp); return -1; }
    return 0;
}

int main(void) {
    int err = sceAppInstUtilInitialize();
    if (err) {
        fprintf(stderr, "sceAppInstUtilInitialize: 0x%08x\n", (unsigned)err);
        notify("Baidu shortcut: installer initialization failed. Check payload logs.");
        return 1;
    }
    if (directory(BASE) || directory(SYS) ||
        write_missing(SYS "/param.json", param_json, sizeof(param_json)) ||
        write_missing(SYS "/icon0.png", icon_png, sizeof(icon_png))) {
        notify("Baidu shortcut: asset write failed or title ID conflict. Check payload logs.");
        return 1;
    }
    int (*install_title)(const char *, const char *, void *) = 0;
    uint32_t handle;
    if (!kernel_dynlib_handle(-1, "libSceAppInstUtil.sprx", &handle))
        install_title = (void *)kernel_dynlib_resolve(-1, handle, "Wudg3Xe3heE");
    err = install_title ? install_title(TITLE_ID, "/user/app/", 0)
                        : sceAppInstUtilAppInstallAll(0);
    if (err) {
        fprintf(stderr, "Launcher registration: 0x%08x\n", (unsigned)err);
        notify("Baidu shortcut: registration failed. Check payload logs; rerun to retry.");
        return 1;
    }
    notify("Baidu shortcut installed. Open Baidu in Media. No local server required.");
    return 0;
}
