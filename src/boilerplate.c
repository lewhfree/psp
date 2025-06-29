#include <pspkernel.h>

int exit_callback(int arg1, int arg2, void *common) {
    (void) arg1; (void) arg2; (void) common;
    sceKernelExitGame();
    return 0;
}
int callback_thread(SceSize args, void *argp) {
    (void) args; (void) argp;
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}
int boilerplate(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}
