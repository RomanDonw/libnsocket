/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef LIBNSOCKET_H
#define LIBNSOCKET_H

#ifdef __cplusplus
    extern "C"
    {
#endif

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
    #define LIBNSOCKET_OS_WINDOWS
#endif

#if defined(__BYTE_ORDER__)
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define LIBNSOCKET_BIG_ENDIAN
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define LIBNSOCKET_LITTLE_ENDIAN
    #endif

#elif defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)
    #define LIBNSOCKET_BIG_ENDIAN

#elif defined(LIBNSOCKET_OS_WINDOWS)
    #if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
        #define LIBNSOCKET_LITTLE_ENDIAN
    #endif
    
#endif

#if !defined(LIBNSOCKET_LITTLE_ENDIAN) && !defined(LIBNSOCKET_BIG_ENDIAN)
    #error Unsupported/unknown CPU architecture endianness.
#endif

#ifdef LIBNSOCKET_OS_WINDOWS
    // Windows environment.

    #include <winsock2.h>
    #include <ws2tcpip.h>

    #ifdef _MSC_VER
        #ifdef LIBNSOCKET_STATIC
            #define LIBNSOCKET_API
        #else
            #ifdef LIBNSOCKET_EXPORT
                #define LIBNSOCKET_API __declspec(dllexport)
            #else
                #define LIBNSOCKET_API __declspec(dllimport)
            #endif
        #endif
    #else
        #define LIBNSOCKET_API __attribute__((visibility("default")))
    #endif

    typedef SOCKET NSOCKET_NATIVEDESCRIPTOR;
    #define NSOCKET_INVALIDDESCRIPTOR (INVALID_SOCKET)

    #define LIBNSOCKET_DEFAULT_WINSOCK_VERSION (MAKEWORD(2, 2))

#else
    // POSIX environment.

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    
    #define LIBNSOCKET_API __attribute__((visibility("default")))

    typedef int NSOCKET_NATIVEDESCRIPTOR;
    #define NSOCKET_INVALIDDESCRIPTOR -1

#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <libncore.h>

#define LIBNSOCKET_ABI

#if defined(LIBNSOCKET_LITTLE_ENDIAN)
    #define NSOCKET_HTONS(x) ((uint16_t)(((uint16_t)(x) & 0xFF00) >> 8) | (((uint16_t)(x) & 0x00FF) << 8))
    #define NSOCKET_HTONL(x) ((uint32_t)((((uint32_t)(x) & 0xFF000000) >> 24) | (((uint32_t)(x) & 0x000000FF) << 24) | (((uint32_t)(x) & 0x00FF0000) >> 8) | (((uint32_t)(x) & 0x0000FF00) << 8)))
#elif defined(LIBNSOCKET_BIG_ENDIAN)
    #define NSOCKET_HTONS(x) ((uint16_t)(x))
    #define NSOCKET_HTONL(x) ((uint32_t)(x))
#endif

#define NSOCKET_NTOHS(x) NSOCKET_HTONS(x)
#define NSOCKET_NTOHL(x) NSOCKET_HTONL(x)

enum NSocketAddressFamily
{
    NSocketAddressFamily_Unspecified = AF_UNSPEC,
    NSocketAddressFamily_IPv4 = AF_INET,
    NSocketAddressFamily_IPv6 = AF_INET6
} typedef NSocketAddressFamily;

enum NSocketType
{
    NSocketType_Unspecified = 0,
    NSocketType_Stream = SOCK_STREAM,
    NSocketType_Datagram = SOCK_DGRAM,
} typedef NSocketType;

enum NSocketProtocol
{
    NSocketProtocol_Unspecified = 0,
    NSocketProtocol_TCP = IPPROTO_TCP,
    NSocketProtocol_UDP = IPPROTO_UDP
} typedef NSocketProtocol;

enum NSocketOptionLevel
{
    NSocketOptionLevel_Socket = SOL_SOCKET,
    NSocketOptionLevel_IP = IPPROTO_IP,
    NSocketOptionLevel_TCP = IPPROTO_TCP,
} typedef NSocketOptionLevel;

/*
        R/O - readonly.
        W/O - writeonly.
        R/W - readable/writeable.
*/

enum NSocketOptionName
{
    // NSocket level.
    NSocketOptionName_Socket_RecvBufferSize = SO_RCVBUF, // int, R/W.
    NSocketOptionName_Socket_SendBufferSize = SO_SNDBUF, // int, R/W.
    NSocketOptionName_Socket_KeepAliveConnection = SO_KEEPALIVE, // bool, R/W.
    NSocketOptionName_Socket_AcceptConnections = SO_ACCEPTCONN, // bool, R/O.
    NSocketOptionName_Socket_InternalError = SO_ERROR, // int, R/O.
    NSocketOptionName_Socket_Broadcast = SO_BROADCAST, // bool, R/W.
    NSocketOptionName_Socket_Linger = SO_LINGER, // struct NSocketLingerOptions, R/W.
    NSocketOptionName_Socket_RecvTimeout = SO_RCVTIMEO, // uint32_t (milliseconds), R/W.
    NSocketOptionName_Socket_SendTimeout = SO_SNDTIMEO, // uint32_t (milliseconds), R/W.
    NSocketOptionName_Socket_Type = SO_TYPE, // int (enum NSocketType), R/O.

    // IP level.
    NSocketOptionName_IP_TimeToLive = IP_TTL, // uint8_t, R/W.

    // TCP level.
    NSocketOptionName_TCP_NoDelay = TCP_NODELAY, // bool, R/W.
    NSocketOptionName_TCP_MaxKeepAliveProbes = TCP_KEEPCNT, // int, R/W.
    NSocketOptionName_TCP_KeepAliveProbesInterval = TCP_KEEPINTVL, // int (seconds), R/W.
    NSocketOptionName_TCP_ConnectionKeepIdleTime = TCP_KEEPIDLE, // int (seconds), R/W.
    NSocketOptionName_TCP_FastOpen = TCP_FASTOPEN, // bool, R/W.
} typedef NSocketOptionName;

struct NSocketLingerOptions
{
    bool enable;
    unsigned short linger; // in seconds.
} typedef NSocketLingerOptions;

#define NSOCKET_RECV_NOFLAGS 0
#define NSOCKET_RECV_FLAG_PEEK MSG_PEEK
#define NSOCKET_RECV_FLAG_WAITALL MSG_WAITALL
#define NSOCKET_RECV_FLAG_TRUNC MSG_TRUNC

#define NSOCKET_SEND_NOFLAGS 0
#define NSOCKET_SEND_FLAG_DONTROUTE MSG_DONTROUTE

// flags for NSocketDNSRequest/NSocketDNSResponse.
typedef unsigned char NSocketGetAddrInfoFlags;
#define NSOCKET_AI_NOFLAGS 0
#define NSOCKET_AI_FLAG_PASSIVE 1
#define NSOCKET_AI_FLAG_CANONNAME 2
#define NSOCKET_AI_FLAG_NUMERICHOST 4
#define NSOCKET_AI_FLAG_NUMERICSERV 8
#define NSOCKET_AI_FLAG_ADDRCONFIG 16
#define NSOCKET_AI_FLAG_IPV4MAPPED 32
#define NSOCKET_AI_FLAG_BOTHIPVERS 64

// flags for nsocket_getnameinfo.
typedef unsigned char NSocketGetNameInfoFlags;
#define NSOCKET_NI_NOFLAGS 0
#define NSOCKET_NI_FLAG_NOFQDN 1
#define NSOCKET_NI_FLAG_NUMERICHOST 2
#define NSOCKET_NI_FLAG_NUMERICSERV 4
#define NSOCKET_NI_FLAG_DGRAM 8
#define NSOCKET_NI_FLAG_NAMEREQD 16

// flags & type definitions for nsocket_shutdown.
typedef unsigned char NSocketShutdownFlags;
#define NSOCKET_SD_FLAG_RECV 1 // 01b
#define NSOCKET_SD_FLAG_SEND 2 // 10b

typedef struct in_addr IPv4Address;
typedef struct in6_addr IPv6Address;
typedef void IPAddressInterface;

#define IPV4ADDRSTRSIZE INET_ADDRSTRLEN
#define IPV6ADDRSTRSIZE INET6_ADDRSTRLEN

#define IPV4ADDR_INIT(addr) (IPv4Address){ .s_addr = NSOCKET_HTONL(addr) }
#define IPV4ADDR_PACK(a, b, c, d) ((((uint32_t)(a) & 0xFF) << 24) | (((uint32_t)(b) & 0xFF) << 16) | (((uint32_t)(c) & 0xFF) << 8) | ((uint32_t)(d) & 0xFF))

LIBNSOCKET_API extern const IPv4Address IPV4ADDR_ANY;
LIBNSOCKET_API extern const IPv4Address IPV4ADDR_LOOPBACK;
LIBNSOCKET_API extern const IPv4Address IPV4ADDR_BROADCAST;

LIBNSOCKET_API extern const IPv6Address IPV6ADDR_ANY;
LIBNSOCKET_API extern const IPv6Address IPV6ADDR_LOOPBACK;

typedef struct NSocket NSocket;

typedef void NSocketAddressInterface;
typedef struct sockaddr NSocketAddressBase;

#define NSOCKET_GETSOCKADDRAF(sockaddr_ptr) ((NSocketAddressFamily)(((const NSocketAddressBase *)(sockaddr_ptr))->sa_family))

typedef void NSocketIPAddressInterface;
typedef struct sockaddr_in NSocketIPv4Address;
typedef struct sockaddr_in6 NSocketIPv6Address;

typedef struct sockaddr_storage NSocketAddressStorage;

#define LIBNSOCKET_SOCKETDNSBASE \
    NSocketGetAddrInfoFlags flags; /* see SOCKET_AI_... flags for more info. */\
    NSocketAddressFamily af;\
    NSocketType type;\
    NSocketProtocol protocol;

struct NSocketDNSRequest
{
    LIBNSOCKET_SOCKETDNSBASE
} typedef NSocketDNSRequest;

struct NSocketDNSResponse
{
    LIBNSOCKET_SOCKETDNSBASE

    NSocketAddressInterface *sockaddr;
    size_t sockaddrlen;
    char *canonname;

    struct NSocketDNSResponse *next;
} typedef NSocketDNSResponse;

#undef LIBNSOCKET_SOCKETDNSBASE

struct LibNSocketStartupOptions
{
    const NMemoryAllocators *allocators; // can be NULL.
    NPanicHandler *panichandler; // can be NULL.
    NAlertHandler *alerthandler; // can be NULL.
    
    unsigned short winsock_version; // must be equals to 0 to use the default version of WinSock.
} typedef LibNSocketStartupOptions;

#define LIBNSOCKETSTARTUPOPTIONS_DEFAULTINIT (LibNSocketStartupOptions){0}

struct LibNSocketStartupResults
{
    unsigned short used_winsock_version; // equals to 0 on non-WinSock systems.
    unsigned short max_winsock_version; // equals to 0 on non-WinSock systems.
    unsigned short max_sockets_count; // equals to 0 if not limited or not specified by system sockets API.
    unsigned short max_datagram_size; // equals to 0 if not limited or not specified by system sockets API.
} typedef LibNSocketStartupResults;

extern const char *LIBNSOCKET_MODULENAME;

LIBNSOCKET_API bool LIBNSOCKET_ABI libnsocket_initialized(void); // can be accessed without library initialization.
LIBNSOCKET_API NError LIBNSOCKET_ABI libnsocket_startup(const LibNSocketStartupOptions *options, LibNSocketStartupResults *results); // options & results can be NULL.
LIBNSOCKET_API NError LIBNSOCKET_ABI libnsocket_cleanup(void);

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_parseipaddr(IPAddressInterface *addr, NSocketAddressFamily af, const char *straddr);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_ipaddrtostr(const IPAddressInterface *addr, NSocketAddressFamily af, char *straddr, socklen_t size);

// [nsocket_packsockipaddr]: can be accessed without library initialization.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_packsockipaddr(NSocketIPAddressInterface *sockaddr, NSocketAddressFamily af, const IPAddressInterface *addr, unsigned short port);
// [nsocket_unpacksockipaddr]: can be accessed without library initialization.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_unpacksockipaddr(const NSocketIPAddressInterface *sockaddr, NSocketAddressFamily af, IPAddressInterface *addr, unsigned short *port);

// [nsocket_getaddrinfo]: request can be NULL, and host OR service also can be NULL, but not both. see <netdb.h> getaddrinfo function documentation for more info.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getaddrinfo(const char *hostname, const char *servicename, const NSocketDNSRequest *request, NSocketDNSResponse **response);
LIBNSOCKET_API void LIBNSOCKET_ABI nsocket_freeaddrinfo(NSocketDNSResponse *response); // safe for NULL pointer and can be accessed without library initialization.
// [nsocket_getnameinfo]: hostname & servicename can be NULL, and hostnamesize OR servicenamesize can be NULL, but not both. (see <netdb.h> getnameinfo function documentation for more info.)
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getnameinfo(const NSocketIPAddressInterface *sockaddr, socklen_t sockaddrlen, char *hostname, size_t *hostnamesize, char *servicename, size_t *servicenamesize, NSocketGetNameInfoFlags flags);

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_open(NSocket **socket, NSocketAddressFamily af, NSocketType type, NSocketProtocol protocol);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_close(NSocket *socket);

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_connect(const NSocket *socket, const NSocketAddressInterface *sockaddr, socklen_t sockaddrlen);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_bind(const NSocket *socket, const NSocketAddressInterface *sockaddr, socklen_t sockaddrlen);

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_listen(const NSocket *socket, int backlog);
// [nsocket_accept]: sockaddr & sockaddrlen can be NULL. see <sys/socket.h> accept function documentation for more info.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_accept(NSocket **acceptedsocket, const NSocket *socket, NSocketAddressInterface *sockaddr, socklen_t *sockaddrlen);

// [nsocket_recv - ... - nsocket_sendto]: processedbytes can be NULL.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_recv(const NSocket *socket, void *buffer, size_t size, size_t *processedbytes, int flags);
// [nsocket_recvfrom]: sockaddr & sockaddrlen can be NULL. see <sys/socket.h> recvfrom function documentation for more info.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_recvfrom(const NSocket *socket, void *buffer, size_t size, size_t *processedbytes, int flags, NSocketAddressInterface *sockaddr, socklen_t *sockaddrlen);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_send(const NSocket *socket, const void *data, size_t size, size_t *processedbytes, int flags);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_sendto(const NSocket *socket, const void *buffer, size_t size, size_t *processedbytes, int flags, const NSocketAddressInterface *sockaddr, socklen_t sockaddrlen);

LIBNSOCKET_API bool LIBNSOCKET_ABI nsocket_isnonblocking(const NSocket *socket); // can be accessed without library initialization.
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_setnonblocking(NSocket *socket, bool enable);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getreadablebytes(const NSocket *socket, size_t *availbytes);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_shutdown(const NSocket *socket, NSocketShutdownFlags flags);

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getopt(const NSocket *socket, NSocketOptionLevel level, NSocketOptionName optname, void *optval, size_t *optsize);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_setopt(const NSocket *socket, NSocketOptionLevel level, NSocketOptionName optname, const void *optval, size_t optsize);

LIBNSOCKET_API NSocketAddressFamily LIBNSOCKET_ABI nsocket_getaf(const NSocket *socket); // can be accessed without library initialization.
LIBNSOCKET_API NSocketType LIBNSOCKET_ABI nsocket_gettype(const NSocket *socket); // can be accessed without library initialization.
LIBNSOCKET_API NSocketProtocol LIBNSOCKET_ABI nsocket_getprotocol(const NSocket *socket); // can be accessed without library initialization.

LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getpeername(const NSocket *socket, NSocketAddressInterface *sockaddr, socklen_t *size);
LIBNSOCKET_API NError LIBNSOCKET_ABI nsocket_getsockname(const NSocket *socket, NSocketAddressInterface *sockaddr, socklen_t *size);

#if defined(LIBNSOCKET_ALLOWUNSAFEACCESS) || defined(LIBNSOCKET_EXPORT)
    LIBNSOCKET_API NSOCKET_NATIVEDESCRIPTOR LIBNSOCKET_ABI nsocket_gethandle(const NSocket *socket); // can be accessed without library initialization.
#endif

#ifdef __cplusplus
    }
#endif

#endif
