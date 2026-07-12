/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "base/base.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static NError err;

const char *testname = "nsocket_getaddrinfo & nsocket_getnameinfo test";

static void *ipaddrbuff = NULL;

static char *ipaddrstrbuff = NULL;
static size_t ipaddrstrsize = 0;

void printdnsinfo(const char *nodename, const char *servicename, const NSocketDNSRequest *req)
{
    NSocketDNSResponse *resps;
    if ((err = nsocket_getaddrinfo(nodename, servicename, req, &resps)) != NError_Success) handlesockerror(err, "nsocket_getaddrinfo");

    printf("======================================\n   node: %s, service: %s\n======================================\n", nodename, servicename);

    size_t recordn = 0;
    for (NSocketDNSResponse *currresp = resps; currresp; currresp = currresp->next)
    {
        printf("Record index #%zu:\n -  Flags: 0x%x (0%Xh).\n -  Address family: ", recordn, currresp->flags, currresp->flags);

        switch (currresp->af)
        {
            case NSocketAddressFamily_Unspecified:
                puts("(unspecified).");
                break;

            case NSocketAddressFamily_IPv4:
                puts("IPv4.");
                break;

            case NSocketAddressFamily_IPv6:
                puts("IPv6.");
                break;

            default:
                puts("(unknown/unsupported).");
        }

        printf(" -  NSocket type: ");
        switch (currresp->type)
        {
            case NSocketType_Unspecified:
                puts("(any).");
                break;

            case NSocketType_Stream:
                puts("stream.");
                break;

            case NSocketType_Datagram:
                puts("datagram.");
                break;

            default:
                puts("(unknown/unsupported).");
        }

        printf(" -  Protocol: ");
        switch (currresp->protocol)
        {
            case NSocketProtocol_Unspecified:
                puts("(any).");
                break;

            case NSocketProtocol_TCP:
                puts("TCP.");
                break;
            
            case NSocketProtocol_UDP:
                puts("UDP.");
                break;

            default:
                puts("(unknown/unsupported).");
        }

        if (currresp->canonname) printf(" -  Canonical name: \"%s\".\n", currresp->canonname);
        else puts(" -  Canonical name: (unspecified).");

        if (currresp->sockaddr && currresp->sockaddrlen)
        {
            static unsigned short port;
            if ((err = nsocket_unpacksockipaddr(currresp->sockaddr, currresp->af, ipaddrbuff, &port)) != NError_Success) handlesockerror(err, "nsocket_unpacksockaddr");

            if ((err = nsocket_ipaddrtostr(ipaddrbuff, currresp->af, ipaddrstrbuff, ipaddrstrsize)) != NError_Success) handlesockerror(err, "nsocket_addrtostr");

            printf(" -  Address: [%s]:%u\n", ipaddrstrbuff, port);
        }
        else puts(" -  Address: (unspecified).");

        if (currresp->next) putchar('\n');
        recordn++;
    }

    nsocket_freeaddrinfo(resps);
    puts("======================================\n======================================\n======================================\n");
}

void test(void)
{ 
    ipaddrbuff = malloc_s(MAX(sizeof(IPv4Address), sizeof(IPv6Address)));

    ipaddrstrsize = MAX(IPV4ADDRSTRSIZE, IPV6ADDRSTRSIZE);
    ipaddrstrbuff = malloc_s(ipaddrstrsize);

    printdnsinfo("google.com", NULL, NULL);
    printdnsinfo("wikipedia.org", "80", NULL);
    printdnsinfo("kernel.org", "http", NULL);

    NSocketDNSRequest req =
    {
        .flags = NSOCKET_AI_FLAG_CANONNAME,
        .af = NSocketAddressFamily_IPv4,
        .type = NSocketType_Stream,
        .protocol = NSocketProtocol_TCP
    };

    printdnsinfo("github.com", "http", &req);

    free(ipaddrstrbuff); ipaddrstrbuff = NULL; ipaddrstrsize = 0;
    free(ipaddrbuff); ipaddrbuff = NULL;

    // ===========================================================================================================================================

    NSocketIPv4Address saddr;
    IPv4Address addr4 = IPV4ADDR_INIT(IPV4ADDR_PACK(127, 0, 0, 1));
    if ((err = nsocket_packsockipaddr(&saddr, NSocketAddressFamily_IPv4, &addr4, 9418)) != NError_Success) handlesockerror(err, "nsocket_packsockaddr");

    size_t hostnamesz = 0, servicesz = 0;
    if ((err = nsocket_getnameinfo(&saddr, sizeof(saddr), NULL, &hostnamesz, NULL, &servicesz, NSOCKET_NI_NOFLAGS)) != NError_Success) handlesockerror(err, "nsocket_getnameinfo");

    char *hostname = malloc_s(hostnamesz);
    char *servicename = malloc_s(servicesz);
    if ((err = nsocket_getnameinfo(&saddr, sizeof(saddr), hostname, &hostnamesz, servicename, &servicesz, NSOCKET_NI_NOFLAGS)) != NError_Success) handlesockerror(err, "nsocket_getnameinfo");

    printf("127.0.0.1:9418 resolved to (host | service): %s | %s.\n", hostname, servicename);

    free(servicename);
    free(hostname);
}
