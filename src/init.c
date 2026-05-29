/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libsocket.h"
#include "init.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>

#include "err.h"
#include "sockslist.h"
#include "util.h"

static atomic_bool inited = ATOMIC_VAR_INIT(false);
static atomic_flag initfuncsbusyflag = ATOMIC_FLAG_INIT;

bool libsocket_initialized(void) { return atomic_load(&inited); }

SocketError libsocket_startup(const LibSocketAllocators *allocators, const SocketStartupOptions *options)
{
    if (atomic_flag_test_and_set(&initfuncsbusyflag)) return SocketError_OperationInProgress;
    if (atomic_load(&inited)) { atomic_flag_clear(&initfuncsbusyflag); return SocketError_AlreadyInitialized; }

    if (allocators) allocs = *allocators;
    else
    {
        allocs.malloc = malloc;
        allocs.realloc = realloc;
        allocs.free = free;
    }

    // =============================================================================

    if (mutex_init(&sockslist_mutex) != MUTEXERROR_SUCCESS)
    {
        memset(&allocs, 0, sizeof(allocs));

        atomic_flag_clear(&initfuncsbusyflag);
        return SocketError_InitializationError;
    }

    #ifdef LIBSOCKET_OS_WINDOWS
        static const SocketStartupOptions defaultopts = SOCKSTUPOPTS_DEFAULTINIT;

        if (!options) options = &defaultopts;

        WSADATA data;
        int err = WSAStartup(options->winsock_version, &data);
        if (err)
        {
            memset(&allocs, 0, sizeof(allocs));
            
            atomic_flag_clear(&initfuncsbusyflag); 
            return translateerror(err);
        }

        if (data.wVersion != options->winsock_version)
        {
            WSACleanup();

            memset(&allocs, 0, sizeof(allocs));
            
            atomic_flag_clear(&initfuncsbusyflag);
            return SocketError_WSAVersionsNotMatch;
        }
    #endif

    // =============================================================================

    atomic_store(&inited, true);
    atomic_flag_clear(&initfuncsbusyflag);
    return SocketError_Success;
}

SocketError libsocket_cleanup(void)
{
    if (atomic_flag_test_and_set(&initfuncsbusyflag)) return SocketError_OperationInProgress;
    if (!atomic_load(&inited)) { atomic_flag_clear(&initfuncsbusyflag); return SocketError_NotInitialized; }

    // =============================================================================

    #ifdef LIBSOCKET_OS_WINDOWS
        if (WSACleanup())
        {
            atomic_flag_clear(&initfuncsbusyflag);
            return GETLASTTRANSLATEDSYSERR();
        }

        sockslist_removeall(false);
    #else
        sockslist_removeall(true);
    #endif
    
    mutex_destroy(sockslist_mutex);

    // =============================================================================

    memset(&allocs, 0, sizeof(allocs));

    atomic_store(&inited, false);
    atomic_flag_clear(&initfuncsbusyflag);
    return SocketError_Success;
}