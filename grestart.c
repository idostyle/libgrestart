/*
    Graceful restart library
    Copyright (C) 2013  idostyle

    This file is part of libgrestart.

    libgrestart is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    libgrestart is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libgrestart.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _FORTIFY_SOURCE 2

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>

#include "grestart.h"

#ifndef GR_WANT_POLL
int gr_poll(const int fd, const int timeout);
#endif

#ifndef GR_WANT_IOV
int gr_recv_iov(const int gr, struct iovec * iov);
int gr_send_iov(const int gr, const int fd, struct iovec * iov);
#endif

#define GR_ARG_CHECK() \
    if (!identifier) \
        return GR_NO_IDENTIFIER; \
    if (identifier_len > /* linux/un.h: UNIX_PATH_MAX - 1*/ 107) \
        return GR_IDENTIFIER_TOO_LONG;

#define GR_CREATE_SOCKET(var_s) \
    const int var_s = socket(PF_UNIX, /*SOCK_STREAM*/ SOCK_SEQPACKET, 0); \
    if (var_s < 0) \
        return GR_SOCKET_CREATION_FAILED;

#define GR_SETUP_ADDRESS(address, identifier, identifier_len) \
    do { \
        memset(&address, 0, sizeof(struct sockaddr_un)); \
        address.sun_family = AF_UNIX; \
        memcpy(&(address.sun_path), identifier, identifier_len); \
    } while(0)

#define GR_CLOSE_AND_RETURN(var_s, err) { close(var_s); return err; }

#define GR_SUN_LEN(idlen) ((socklen_t) (((size_t)((struct sockaddr_un *) 0)->sun_path) + idlen))

#define GR_SETUP_MESSAGE_HEADER(m, c) \
    struct msghdr m; \
    memset(&m, 0, sizeof(struct msghdr)); \
    char ctrl_buf[CMSG_SPACE(sizeof(int))]; \
    memset(ctrl_buf, 0, sizeof(ctrl_buf)); \
    struct cmsghdr * c = (struct cmsghdr *) ctrl_buf; \
    m.msg_control = c; \
    m.msg_controllen = CMSG_SPACE(sizeof(int));


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

int gr_recv(const int gr, void * fd_identifier, const size_t fd_identifier_len)
{
    struct iovec iov;
    memset(&iov, '\0', sizeof(struct iovec));

    if (fd_identifier_len > 0lu)
    {
        iov.iov_len = fd_identifier_len;
        iov.iov_base = fd_identifier;
    }

    return gr_recv_iov(gr, &iov);
}

int gr_send(const int gr, const int fd, void * fd_identifier, const size_t fd_identifier_len)
{
    struct iovec iov;
    memset(&iov, '\0', sizeof(struct iovec));

    if (fd_identifier_len > 0lu)
    {
        iov.iov_len = fd_identifier_len;
        iov.iov_base = fd_identifier;
    }

    return gr_send_iov(gr, fd, &iov);
}

int gr_close(const int gr)
{
    if (gr < 0)
        return GR_NOT_A_GR_CLIENT;

    struct sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));
    socklen_t add_len = sizeof(struct sockaddr_un);

    const int rs = getsockname(gr, (struct sockaddr *) &address, &add_len);
    if (rs < 0)
    {
        return GR_GETSOCKNAME_FAILED;
    }

    if (address.sun_path[0] != '\0')
    {
        const int ru = unlink(address.sun_path);
        if (ru < 0)
        {
            return GR_UNLINK_FAILED;
        }
    }

    const int rc = close(gr);
    if (rc < 0)
    {
        return GR_CLOSE_FAILED;
    }

    return 0;
}

int gr_poll(const int fd, const int timeout)
{
    if (fd >= 0)
    {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN /* new clients ? */;
        const int r = poll(&pfd, 1lu, timeout);
        if (r < 0)
            return GR_POLL_FAILED;
        else
            return pfd.revents;
    }
    else
        return GR_DOESNT_LOOK_LIKE_A_FD;
}

int gr_recv_iov(const int gr, struct iovec * iov)
{
    if (gr < 0)
        return GR_NOT_A_GR_CLIENT;

    GR_SETUP_MESSAGE_HEADER(m, c);

    if (iov)
    {
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

int gr_send_iov(const int gr, const int fd, struct iovec * iov)
{
    if (gr < 0)
        return GR_NOT_A_GR_CLIENT;

    if (fd < 0)
        return GR_DOESNT_LOOK_LIKE_A_FD;

    GR_SETUP_MESSAGE_HEADER(m, c);

    c->cmsg_level = SOL_SOCKET;
    c->cmsg_type = SCM_RIGHTS;
    c->cmsg_len = CMSG_LEN(sizeof(int));
    *(int*)CMSG_DATA(c) = fd;

    if (iov)
    {
        m.msg_iov = iov;
        m.msg_iovlen = 1;
    }

    const ssize_t r = sendmsg(gr, &m, 0);
    if (r < 0)
        return GR_SENDMSG_FAILED;

    return (int) r;
}
