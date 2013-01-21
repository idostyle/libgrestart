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
