#ifndef RESOURCES_H
#define RESOURCES_H

#include <stddef.h>

#include "libsocket.h"

enum SocketsListError
{
    SocketsListError_Success = 0,
    SocketsListError_MemoryAllocationFailed = 1,
    SocketsListError_ItemAlreadyExist = 2,
    SocketsListError_ItemNotExist = 3
} typedef SocketsListError;

bool sockslist_has(Socket *socket);
SocketsListError sockslist_add(Socket *socket);
SocketsListError sockslist_remove(Socket *socket);
void sockslist_removeall(void);

#endif