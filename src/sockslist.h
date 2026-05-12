#ifndef RESOURCES_H
#define RESOURCES_H

#include <stddef.h>

#include "libsocket.h"

#define SOCKSLISTERR_SUCCESS 0
#define SOCKSLISTERR_NOMEM 1
#define SOCKSLISTERR_ITEMALREXIST 2
#define SOCKSLISTERR_ITEMNOTEXIST 3

typedef unsigned char sockslisterr_t;

bool sockslist_has(Socket *socket);
sockslisterr_t sockslist_add(Socket *socket);
sockslisterr_t sockslist_remove(Socket *socket);
void sockslist_removeall(void);

#endif