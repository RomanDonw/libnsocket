/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_H
#define UTIL_H

#include "libnsocket.h"

#include <limits.h>
#include <stddef.h>
#include <libnthread.h>

#ifdef LIBNSOCKET_OS_WINDOWS
    #define CLAMPSIZET(x) ((size_t)x > INT_MAX ? (int)INT_MAX : (int)x)
    #define CLOSESOCKETDESC(descr) (closesocket(descr))

    #if defined(_MSC_VER) && (_MSC_VER) < 1300
        #define __func__ __FUNCTION__
    #endif
#else
    #define CLAMPSIZET(x) ((size_t)x)
    #define CLOSESOCKETDESC(descr) (close(descr))
#endif

extern NMemoryAllocators __libnsocket_allocators;
#define allocs (__libnsocket_allocators)

extern NUnorderedSet *__libnsocket_sockslist;
#define sockslist (__libnsocket_sockslist)

extern NThreadMutex *__libnsocket_sockslistmutex;
#define sockslistmutex (__libnsocket_sockslistmutex)

// =============================================================================

extern NPanicHandler *__libnsocket_panichandler;
#define __panichandler (__libnsocket_panichandler)

NPanicHandler __libnsocket_defaultpanichandler;
#define __defaultpanichandler (__libnsocket_defaultpanichandler)


#define PANIC_NOERRORCODE (NError_Success)

#define panic_general(errorcode, description) \
    {\
        __panichandler(LIBNSOCKET_MODULENAME, __FILE__, __LINE__, __func__, (description), (errorcode));\
        abort();\
    }

// =============================================================================

extern NAlertHandler *__libnsocket_alerthandler;
#define __alerthandler (__libnsocket_alerthandler)

NAlertHandler __libnsocket_defaultalerthandler;
#define __defaultalerthandler (__libnsocket_defaultalerthandler)

#ifdef LIBNSOCKET_DEBUG
    #define alert(format, ...) (__alerthandler(LIBNSOCKET_MODULENAME, __FILE__, __LINE__, __func__, format, __VA_ARGS__))
#else
    #define alert(format, ...)
#endif

// =============================================================================

NError __libnsocket_closesocket(NSocket *socket);
#define __closesocket (__libnsocket_closesocket)

#define SAFE_MUTEX_LOCK(mutex) \
    { NError nerr = nthread_mutex_lock(mutex); if (nerr != NError_Success) panic_general(nerr, n_panicmsg_mutexlock); }

#define SAFE_MUTEX_UNLOCK(mutex) \
    { NError nerr = nthread_mutex_unlock(mutex); if (nerr != NError_Success) panic_general(nerr, n_panicmsg_mutexunlock); }

// =============================================================================

#endif
