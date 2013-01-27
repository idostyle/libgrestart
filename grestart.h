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

#ifndef _LIB_GRESTART_
#define _LIB_GRESTART_

/**
 * Graceful Restart Library
 */

#include <stddef.h>

typedef enum error_codes_
{
    GR_NO_IDENTIFIER = -1,
    GR_IDENTIFIER_TOO_LONG = -2,
    GR_SOCKET_CREATION_FAILED = -3,
    GR_CONNECT_FAILED = -4,
    GR_SETSOCKOPT_FAILED = -5,
    GR_BIND_FAILED = -6,
    GR_LISTEN_FAILED = -7,
    GR_NOT_A_GR_CLIENT = -8,
    GR_RECVMSG_FAILED = -9,
    GR_SENDMSG_FAILED = -10,
    GR_INVALID_MSG_RECVD = -11,
    GR_DOESNT_LOOK_LIKE_A_FD = -12,
    GR_GETSOCKNAME_FAILED = -13,
    GR_UNLINK_FAILED = -14,
    GR_CLOSE_FAILED = -15,
    GR_POLL_FAILED = -16,
} error_codes_t;

/**
 * Connect to an running instance if there is one.
 *
 * - identifier:
 *   - a writable path string (ex. "/tmp/example.sock")
 *   - an abstract uds string (ex. "\0e11ab38fa")
 *
 * - identifier_len: length of identifier (ex. "/tmp/example.sock" => 17)
 *
 * Returns a fd >= 0 or an error code < 0
 *   -
 */
int gr_init(const char * identifier, const size_t identifier_len);

/**
 * Setup a waiting instance.
 *
 * - identifier:
 *   - a writable path string (ex. "/tmp/example.sock")
 *   - an abstract uds string (ex. "\0e11ab38fa")
 *
 * - identifier_len: length of identifier (ex. "/tmp/example.sock" => 17)
 *
 * Returns a fd >= 0 or an error code < 0
 */
int gr_setup(const char * identifier, const size_t identifier_len);

/**
 * Recieve a fd from a running instance.
 *
 * - gr: is expected to be an accepted client socket;
 *     not the main socket from gr_init/gr_setup;
 *
 * - fd_identifier: is expected to be an allocated memory area of fd_identifier_len
 *     size or a 0-pointer; can be used to identify the fd send over;
 *     is send over unmodified; may or may not be null-terminated;
 *
 * - fd_identifier_len: size of memory area passed to fd_identifier;
 *     can be 0 if fd_identifier is a 0-pointer;
 *
 * Returns an fd >= 0 or an error code < 0
 */
int gr_recv(const int gr, void * fd_identifier, const size_t fd_identifier_len);

/**
 * Send a fd to the new instance.
 *
 * - gr: is expected to be an accepted client socket
 *
 * - fd: file descriptor to be send over
 *
 * - fd_identifier: is expected to be an allocated memory area of fd_identifier_len
 *     size or a 0-pointer; can be used to identify the fd send over;
 *
 * - fd_identifier_len: size of memory area passed to fd_identifier;
 *     can be 0 if fd_identifier is a 0-pointer;
 *
 * Returns count of byte sent >= 0 or an error code < 0
 */
int gr_send(const int gr, const int fd, void * fd_identifier, const size_t fd_identifier_len);


/**
 *
 */
int gr_close(const int gr);

/**
 * Poll if the fd is readable.
 * It may be advisable to use you own implementation.
 *
 * - fd: file descriptor to check
 *
 * - timeout: timeout in milisecs (passed over to poll)
 *
 * Returns poll events >= 0 or an error code < 0
 */
#ifdef GR_WANT_POLL
int gr_poll(const int fd, const int timeout);
#endif

/**
 * Raw interface to gr_recv/gr_send.
 */
#ifdef GR_WANT_IOV
int gr_recv_iov(const int gr, struct iovec * iov);
int gr_send_iov(const int gr, const int fd, struct iovec * iov);
#endif

#endif
