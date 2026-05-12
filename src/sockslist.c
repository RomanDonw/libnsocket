#include "sockslist.h"

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

sockslisterr_t sockslist_add(Socket *socket)
{
    if (sockslist_has(socket)) return SOCKSLISTERR_ITEMALREXIST;

    {
        Socket **new_sockets = (Socket **)libsocket_realloc(sockets, sizeof(Socket *) * (sockets_count + 1));
        if (!new_sockets) return SOCKSLISTERR_NOMEM;
        sockets = new_sockets;
    }

    sockets[sockets_count++] = socket;
    return SOCKSLISTERR_SUCCESS;
}

sockslisterr_t sockslist_remove(Socket *socket)
{
    if (!sockslist_has(socket)) return SOCKSLISTERR_ITEMNOTEXIST;

    Socket **new_sockets = NULL;
    size_t new_sockets_count = 0;

    for (size_t i = 0; i < sockets_count; i++)
    {
        if (sockets[i] == socket) continue;

        {
            Socket **new_new_sockets = (Socket **)libsocket_realloc(new_sockets, sizeof(Socket *) * (new_sockets_count + 1));
            if (!new_new_sockets) { if (new_sockets) libsocket_free(new_sockets); return SOCKSLISTERR_NOMEM; }
            new_sockets = new_new_sockets;
        }

        new_sockets[new_sockets_count++] = sockets[i];
    }

    sockets = new_sockets;
    sockets_count = new_sockets_count;
    return SOCKSLISTERR_SUCCESS;
}

void sockslist_removeall(void)
{
    libsocket_free(sockets);
    sockets = NULL;
    sockets_count = 0;
}