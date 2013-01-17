#define _FORTIFY_SOURCE 2

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>

#include "grestart.h"

#define GR_ARG_CHECK() \
    if (!identifier) \
        return GR_NO_IDENTIFIER; \
    if (identifier_len > /* linux/un.h: UNIX_PATH_MAX */ 108) \
        return GR_IDENTIFIER_TOO_LONG;

#define GR_CREATE_SOCKET(var_s) \
    const int var_s = socket(PF_UNIX, /*SOCK_STREAM*/ SOCK_SEQPACKET | SOCK_CLOEXEC, 0); \
    if (var_s < 0) \
        return GR_SOCKET_CREATION_FAILED;

#define GR_SETUP_ADDRESS(address, identifier, identifier_len) do { \
        memset(&address, 0, sizeof(struct sockaddr_un)); \
        address.sun_family = AF_UNIX; \
        memcpy(&(address.sun_path), identifier, identifier_len); \
    } while(0)

#define GR_CLOSE_AND_RETURN(var_s, err) { close(var_s); return err; }

#define GR_SUN_LEN(idlen) ((socklen_t) (((size_t)((struct sockaddr_un *) 0)->sun_path) + idlen))


int gr_init(const char * identifier, const size_t identifier_len)
{
    GR_ARG_CHECK();

    GR_CREATE_SOCKET(s);

    struct sockaddr_un address;
    GR_SETUP_ADDRESS(address, identifier, identifier_len);

    const int r = connect(s, (struct sockaddr *) &address, GR_SUN_LEN(identifier_len));
    if (r < 0)
        GR_CLOSE_AND_RETURN(s, GR_CONNECT_FAILED);

    return s;
}

int gr_setup(const char * identifier, const size_t identifier_len)
{
    GR_ARG_CHECK();

    GR_CREATE_SOCKET(s);

    const int reuse = 1;
    const int so = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, (socklen_t) sizeof(int));
    if (so < 0)
        GR_CLOSE_AND_RETURN(s, GR_SETSOCKOPT_FAILED);

    struct sockaddr_un address;
    GR_SETUP_ADDRESS(address, identifier, identifier_len);

    const int r = bind(s, (struct sockaddr *) &address, GR_SUN_LEN(identifier_len));
    if (r < 0)
        GR_CLOSE_AND_RETURN(s, GR_BIND_FAILED);

    const int l = listen(s, 1);
    if (l < 0)
        GR_CLOSE_AND_RETURN(s, GR_LISTEN_FAILED);

    return s;
}

int gr_recv(int gr, void * fd_identifier, size_t fd_identifier_len)
{
    if (gr < 0)
        return GR_NOT_A_GR_INSTANCE;

    struct msghdr m;
    memset(&m, 0, sizeof(struct msghdr));

    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    memset(ctrl_buf, 0, sizeof(ctrl_buf));

    struct cmsghdr * c = (struct cmsghdr *) ctrl_buf;
    m.msg_control = c;
    m.msg_controllen = CMSG_SPACE(sizeof(int));

    struct iovec iov[1];
    memset(iov, 0, sizeof(struct iovec));

    if (fd_identifier)
    {
        iov[0].iov_len = fd_identifier_len;
        iov[0].iov_base = fd_identifier;

        m.msg_iov = iov;
        m.msg_iovlen = 1;
    }

    const ssize_t r = recvmsg(gr, &m, 0);
    if (r < 0)
        return GR_RECVMSG_FAILED;

    if (m.msg_flags & MSG_CTRUNC)
        return GR_INVALID_MSG_RECVD;

    if (m.msg_controllen != CMSG_SPACE(sizeof(int)))
        return GR_INVALID_MSG_RECVD;

    struct cmsghdr * cr = CMSG_FIRSTHDR(&m);

    if (cr->cmsg_level != SOL_SOCKET || cr->cmsg_type != SCM_RIGHTS)
        return GR_INVALID_MSG_RECVD;

    const int fd = *CMSG_DATA(cr);
    if (fd < 0)
        return GR_DOESNT_LOOK_LIKE_A_FD;

    return fd;
}

int gr_send(int gr, int fd, void * fd_identifier, const size_t fd_identifier_len)
{
    if (gr < 0)
        return GR_NOT_A_GR_INSTANCE;

    if (fd < 0)
        return GR_DOESNT_LOOK_LIKE_A_FD;

    struct msghdr m;
    memset(&m, 0, sizeof(struct msghdr));

    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    memset(ctrl_buf, 0, sizeof(ctrl_buf));

    struct cmsghdr * c = (struct cmsghdr *) ctrl_buf;
    c->cmsg_level = SOL_SOCKET;
    c->cmsg_type = SCM_RIGHTS;
    c->cmsg_len = CMSG_LEN(sizeof(int));
    *(int*)CMSG_DATA(c) = fd;

    m.msg_control = ctrl_buf;
    m.msg_controllen = sizeof(ctrl_buf);
    // m.msg_flags = 0;

    struct iovec iov[1];
    memset(iov, 0, sizeof(struct iovec));

    if (fd_identifier_len > 0lu)
    {
        iov[0].iov_len = fd_identifier_len;
        iov[0].iov_base = fd_identifier;
    }

    m.msg_iov = iov;
    m.msg_iovlen = 1;

    const ssize_t r = sendmsg(gr, &m, 0);
    if (r < 0)
        return GR_SENDMSG_FAILED;

    return (int) r;
}


#ifndef GR_WANT_POLL
int gr_poll(int fd);
#endif

int gr_poll(int fd)
{
    if (fd >= 0)
    {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN | POLLMSG;
        const int r = poll(&pfd, 1lu, 0);
        if (r < 0)
            return GR_POLL_FAILED;
        else
            return pfd.revents;
    }
    else
        return GR_NOT_A_GR_INSTANCE;
}
