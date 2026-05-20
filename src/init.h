#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

#include "err.h"

#define ENSURE_INIT if (!socket_initialized()) return SocketError_NotInitialized

#endif