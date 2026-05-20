#include "libsocket.h"
#include "init.h"

#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "sockslist.h"

void *(*libsocket_malloc)(size_t) = malloc;
void *(*libsocket_realloc)(void *, size_t) = realloc;
void (*libsocket_free)(void *) = free;

static bool inited = false;

bool socket_initialized(void) { return inited; }

SocketError socket_startup(const SocketStartupOptions *options)
{
    if (inited) return SocketError_AlreadyInitialized;

    #ifdef LIBSOCKET_OS_WINDOWS
        static const SocketStartupOptions defaultopts =
        {
            .winsock_version = MAKEWORD(LIBSOCKET_WINSOCK_DEFAULT_VERSION_LOW, LIBSOCKET_WINSOCK_DEFAULT_VERSION_HIGH)
        };

        if (!options) options = &defaultopts;

        WSADATA data;
        int err = WSAStartup(options->winsock_version, &data);
        if (err) return translateerror(err);
        //if (data.wVersion != version) { if (WSACleanup()) RETURNWITHSYSERR(false); RETURNWITHERROR(SocketError_WSAVersionNotSupported, false); }
        if (data.wVersion != options->winsock_version) { WSACleanup(); return SocketError_WSAVersionsNotMatch; }
    #endif

    inited = true;
    return SocketError_Success;
}

SocketError socket_cleanup(void)
{
    if (!inited) return SocketError_NotInitialized;

    #ifdef LIBSOCKET_OS_WINDOWS
        if (WSACleanup()) return translateerror(GETLASTERROR());
    #endif
    
    sockslist_removeall();

    inited = false;
    return SocketError_Success;
}