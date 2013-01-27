/*
    Graceful restart library
    Copyright (C) 2013  idostyle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define GR_WANT_POLL
#include "grestart.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>


int main(int argc, char ** argv)
{
    (void)argc; (void)argv;

    int r = gr_init("\0/tmp/gr_test.sock", 19lu);
    printf("gr_init: %d\n", r);
    if (r >= 0)
    {
        int p = gr_poll(r, 1000);
        printf("gr_poll: %d\n", p);
        int x = 42;
        int fd = gr_recv(r, &x, sizeof(int));
        printf("gr_recv: %d %d\n", fd, x);
        if (fd >= 0)
        {
            write(fd, "Test\n", 5lu);
            close(fd);
        }
        else
        {
            printf("gr_recv errno: %s\n", strerror(errno));
        }

        char xi[16];
        fd = gr_recv(r, xi, 16lu);
        printf("gr_recv: %d %s\n", fd, xi);
        if (fd >= 0)
        {
            write(fd, "Test\n", 5lu);
            close(fd);
        }
        else
        {
            printf("gr_recv errno: %s\n", strerror(errno));
        }

        close(r);
    }
    else
        printf("gr_init errno: %s\n", strerror(errno));

    r = gr_setup("\0/tmp/gr_test.sock", 19lu);
    printf("gr_setup: %d\n", r);
    if (r >= 0)
    {
        int client = accept(r, 0, 0);
        printf("accept: %d\n", client);
        if (client >= 0)
        {
            int xi = 42;
            int s = gr_send(client, 1, &xi, sizeof(int));
            printf("gr_send: %d\n", s);
            if (s >= 0)
            {
                // party <|:)-/-<
            }
            else
            {
                printf("gr_send errno: %s\n", strerror(errno));
            }

            s = gr_send(client, 2, "MainSocket", 11lu);
            printf("gr_send: %d\n", s);
            if (s >= 0)
            {
                // party <|:)-/-<
            }
            else
            {
                printf("gr_send errno: %s\n", strerror(errno));
            }
        }
        else
        {
            printf("accept errno: %s\n", strerror(errno));
        }

        close(r);
    }
    else
    {
        printf("gr_setup errno: %s\n", strerror(errno));
    }

    sleep(1);

    return 0;
}
