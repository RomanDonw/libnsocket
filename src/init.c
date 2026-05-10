#include "libsocket.h"
#include "init.h"

#include <stdio.h>
#include <stdlib.h>

#include "err.h"

void *(*libsocket_malloc)(size_t) = malloc;
void *(*libsocket_realloc)(void *, size_t) = realloc;
void (*libsocket_free)(void *) = free;

volatile bool inited = false;

bool socket_isinited(void) { return inited; }

bool socket_startup(void)
{
    if (inited) RETURNWITHERROR(AlreadyInitialized, false);

    #ifdef LIBSOCKET_OS_WINDOWS
        const WORD version = MAKEWORD(LIBSOCKET_WINSOCK_VERSION_LOW, LIBSOCKET_WINSOCK_VERSION_HIGH);

        WSADATA data;
        int err = WSAStartup(version, &data);
        if (err) RETURNWITHERROR(translateerror(err), false);
        //if (data.wVersion != version) { if (WSACleanup()) RETURNWITHSYSERR(false); RETURNWITHERROR(WSAVersionNotSupported, false); }
        if (data.wVersion != version) { WSACleanup(); RETURNWITHERROR(WSAVersionsNotMatch, false); }
    #endif

    inited = true;
    RETURNWITHSUCCESS(true);
}

bool socket_cleanup(void)
{
    if (!inited) RETURNWITHERROR(NotInitialized, false);

    #ifdef LIBSOCKET_OS_WINDOWS
        if (WSACleanup()) RETURNWITHSYSERR(false);
    #endif

    inited = false;
    RETURNWITHSUCCESS(true);
}

#ifndef LIBSOCKET_MANUAL_INIT

    #ifdef _MSC_VER
        #define LIBSOCKET_STUPATTR
        #define LIBSOCKET_CLUPATTR
    #else
        #define LIBSOCKET_STUPATTR __attribute__((constructor(101)))
        #define LIBSOCKET_CLUPATTR __attribute__((destructor))
    #endif

    static void LIBSOCKET_STUPATTR libsocket_autostartup(void)
    {
        if (inited) return;

        if (!socket_startup())
        {
            fprintf(stderr, "[libsocket]: auto startup error: %s. Application aborted.\n", socket_strerror(socket_lasterror));
            abort();
        }
    }

    static void LIBSOCKET_CLUPATTR libsocket_autocleanup(void)
    {
        if (!inited) return;
        
        if (!socket_cleanup())
        {
            fprintf(stderr, "[libsocket]: auto cleanup error: %s. Application aborted.\n", socket_strerror(socket_lasterror));
            abort();
        }
    }

    #ifdef _MSC_VER
        static void libsocket_MSVCautostartup(void)
        {
            libsocket_autostartup();
            if (atexit(libsocket_autocleanup))
            {
                fprintf(stderr, "[libsocket]: error binding library auto cleanup callback in \"atexit\" C function. Application aborted.\n");
                libsocket_autocleanup();
                abort();
            }
        }

        #pragma section(".CRT$XCU", read)
        __declspec(allocate(".CRT$XCU")) void (*libsocket_MSVCautostartup__ptr)(void) = libsocket_MSVCautostartup;
    #endif
#endif