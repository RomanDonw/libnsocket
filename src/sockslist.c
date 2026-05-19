#include "sockslist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static Socket **sockets = NULL;
static size_t sockets_count = 0;

bool sockslist_has(Socket *socket)
{
    bool found = false;
    for (size_t i = 0; i < sockets_count; i++) if (sockets[i] == socket) { found = true; break; }
    return found;
}

SocketsListError sockslist_add(Socket *socket)
{
    if (sockslist_has(socket)) return SocketsListError_ItemAlreadyExist;

    {
        Socket **new_sockets = (Socket **)libsocket_realloc(sockets, sizeof(Socket *) * (sockets_count + 1));
        if (!new_sockets) return SocketsListError_MemoryAllocationFailed;
        sockets = new_sockets;
    }

    sockets[sockets_count++] = socket;
    return SocketsListError_Success;
}

SocketsListError sockslist_remove(Socket *socket)
{
    bool found = false;
    size_t pos;
    for (pos = 0; pos < sockets_count; pos++) if (sockets[pos] == socket) { found = true; break; }
    if (!found) return SocketsListError_ItemNotExist;

    /*if (pos != sockets_count - 1) */sockets[pos] = sockets[sockets_count - 1];
    sockets_count--;

    if (sockets_count > 0)
    {
        Socket **new_sockets = (Socket **)libsocket_realloc(sockets, sizeof(Socket *) * sockets_count);
        if (new_sockets) sockets = new_sockets;
    }
    else
    {
        libsocket_free(sockets);
        sockets = NULL;
    }

    return SocketsListError_Success;
}

void sockslist_removeall(void)
{
    libsocket_free(sockets);
    sockets = NULL;
    sockets_count = 0;
}