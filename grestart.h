#ifndef _LIB_GRESTART_
#define _LIB_GRESTART_

/**
 * Graceful Restart Library
 */

#include <stddef.h>

typedef enum codes_
{
    GR_NO_IDENTIFIER = -1,
    GR_IDENTIFIER_TOO_LONG = -2,
    GR_SOCKET_CREATION_FAILED = -3,
    GR_CONNECT_FAILED = -4,
    GR_SETSOCKOPT_FAILED = -5,
    GR_BIND_FAILED = -6,
    GR_LISTEN_FAILED = -7,
    GR_NOT_A_GR_INSTANCE = -8,
    GR_RECVMSG_FAILED = -9,
    GR_SENDMSG_FAILED = -10,
    GR_INVALID_MSG_RECVD = -11,
    GR_DOESNT_LOOK_LIKE_A_FD = -12,
    GR_POLL_FAILED = -13,
} codes_t;

/**
 * Connect to an running instance
 */
int gr_init(const char * identifier, const size_t identifier_len);

/**
 * Setup a waiting instance
 */
int gr_setup(const char * identifier, const size_t identifier_len);

/**
 * Recieve an fd from a running instance
 */
int gr_recv(int gr, void * fd_identifier, const size_t fd_identifier_len);

/**
 * Send an fd to the new instance
 */
int gr_send(int gr, int fd, void * fd_identifier, const size_t fd_identifier_len);

/**
 *
 */
#ifdef GR_WANT_POLL
int gr_poll(int fd);
#endif


#ifndef SOCK_NONBLOCK
enum  __socket_type_nb
{
    SOCK_NONBLOCK = 00004000,
};
#define SOCK_NONBLOCK SOCK_NONBLOCK
#endif

#ifndef SOCK_CLOEXEC
enum  __socket_type_ce
{
    SOCK_CLOEXEC = 02000000,
};
#define SOCK_CLOEXEC SOCK_CLOEXEC
#endif

#ifndef POLLMSG
#define POLLMSG	0x400
#endif

#endif
