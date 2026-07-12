/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef SOCKET_H
#define SOCKET_H

#include "libnsocket.h"

#include <libnthread.h>

#ifdef LIBNSOCKET_OS_WINDOWS
    typedef SSIZE_T ssize_t;
#endif

struct NSocket
{
    NSOCKET_NATIVEDESCRIPTOR desc;

    NSocketAddressFamily af;
    NSocketType type;
    NSocketProtocol protocol;
    
    bool nonblocking;
    NThreadMutex *mutex_nonblocking;
};

#endif
