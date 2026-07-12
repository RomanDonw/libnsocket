/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "libnsocket.h"

#include <stdlib.h>
#include <string.h>

#ifndef LIBNSOCKET_OS_WINDOWS
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

#include "err.h"

NError nsocket_packsockipaddr(NSocketIPAddressInterface *sockaddr, NSocketAddressFamily af, const IPAddressInterface *addr, unsigned short port)
{
    switch (af)
    {
        case NSocketAddressFamily_IPv4:;
            NSocketIPv4Address *sa4 = sockaddr;
            memset(sa4, 0, sizeof(NSocketIPv4Address));

            sa4->sin_family = NSocketAddressFamily_IPv4;
            sa4->sin_addr = *((IPv4Address *)addr);
            sa4->sin_port = NSOCKET_HTONS(port);

            break;

        case NSocketAddressFamily_IPv6:;
            NSocketIPv6Address *sa6 = sockaddr;
            memset(sa6, 0, sizeof(NSocketIPv6Address));

            sa6->sin6_family = NSocketAddressFamily_IPv6;
            sa6->sin6_addr = *((IPv6Address *)addr);
            sa6->sin6_port = NSOCKET_HTONS(port);

            break;

        default:
            return NError_UnsupportedAddressFamily;
    }

    return NError_Success;
}

NError nsocket_unpacksockipaddr(const NSocketIPAddressInterface *sockaddr, NSocketAddressFamily af, IPAddressInterface *addr, unsigned short *port)
{
    if (NSOCKET_GETSOCKADDRAF(sockaddr) != af) return NError_IncorrectArgumentValue;

    switch (af)
    {
        case NSocketAddressFamily_IPv4:;
            const NSocketIPv4Address *sa4 = sockaddr;
            *((IPv4Address *)addr) = sa4->sin_addr;
            *port = NSOCKET_NTOHS(sa4->sin_port);
            break;

        case NSocketAddressFamily_IPv6:;
            const NSocketIPv6Address *sa6 = sockaddr;
            *((IPv6Address *)addr) = sa6->sin6_addr;
            *port = NSOCKET_NTOHS(sa6->sin6_port);
            break;

        default:
            return NError_UnsupportedAddressFamily;
    }

    return NError_Success;
}