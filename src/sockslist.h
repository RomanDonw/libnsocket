/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef RESOURCES_H
#define RESOURCES_H

#include "libsocket.h"

#include <libmutex.h>
#include <stddef.h>

enum SocketsListError
{
    SocketsListError_Success = 0,
    SocketsListError_MemoryAllocationFailed = 1,
    SocketsListError_ItemAlreadyExist = 2,
    SocketsListError_ItemNotExist = 3
} typedef SocketsListError;

extern mutex_t *sockslist_mutex; // recursive mutex.

bool sockslist_has(Socket *socket);
SocketsListError sockslist_add(Socket *socket);
SocketsListError sockslist_remove(Socket *socket);
void sockslist_removeall(bool closesocks);

#endif