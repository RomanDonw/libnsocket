/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "base/base.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static NError err;

int getsocksendbuffsize(const NSocket *s)
{
    int sendbuffsize;
    size_t sendbuffsize_len = sizeof(sendbuffsize);
    if ((err = nsocket_getopt(s, NSocketOptionLevel_Socket, NSocketOptionName_Socket_SendBufferSize, &sendbuffsize, &sendbuffsize_len)) != NError_Success) handlesockerror(err, "nsocket_getopt");
    if (sendbuffsize_len != sizeof(sendbuffsize)) testabort_c("sendbuffsize_len != sizeof(sendbuffsize). Abort.");
    return sendbuffsize;
}

void setsocksendbuffsize(const NSocket *s, int size)
{
    err = nsocket_setopt(s, NSocketOptionLevel_Socket, NSocketOptionName_Socket_RecvBufferSize, &size, sizeof(size));
    if (err != NError_Success) handlesockerror(err, "nsocket_setopt");
}

const char *testname = "HTTP 1.0 GET request to localhost:8000";

void test(void)
{
    NSocket *s;
    if ((err = nsocket_open(&s, NSocketAddressFamily_IPv4, NSocketType_Stream, NSocketProtocol_TCP)) != NError_Success) handlesockerror(err, "nsocket_open");

    printf("Old send buffer size: %i\n", getsocksendbuffsize(s));
    setsocksendbuffsize(s, 4096);
    printf("New send buffer size: %i\n", getsocksendbuffsize(s));

    IPv4Address localhost = IPV4ADDR_LOOPBACK;
    NSocketIPv4Address saddr;
    if ((err = nsocket_packsockipaddr(&saddr, NSocketAddressFamily_IPv4, &localhost, 8000)) != NError_Success) handlesockerror(err, "nsocket_packsockaddr");
    if ((err = nsocket_connect(s, &saddr, sizeof(saddr))) != NError_Success) handlesockerror(err, "nsocket_connect");

    const char *request = "GET / HTTP/1.0\r\n\r\n";
    if ((err = nsocket_send(s, request, strlen(request), NULL, NSOCKET_SEND_NOFLAGS)) != NError_Success) handlesockerror(err, "nsocket_send");

    waitms(100);

    size_t avail;
    if ((err = nsocket_getreadablebytes(s, &avail)) != NError_Success) handlesockerror(err, "nsocket_getreadablebytes");
    printf("Available bytes: %zu\n\n", avail);
    
    #define BUFFER_SIZE 512
    char buffer[BUFFER_SIZE];
    size_t readbytes;
    while (true)
    {
        if ((err = nsocket_recv(s, buffer, BUFFER_SIZE, &readbytes, NSOCKET_RECV_NOFLAGS)) != NError_Success || readbytes <= 0) break;
        for (size_t i = 0; i < readbytes; i++) putchar(buffer[i]);
    }

    if ((err = nsocket_close(s)) != NError_Success) handlesockerror(err, "nsocket_close");
}
