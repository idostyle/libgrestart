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
        int x = 42;
        size_t x_len = sizeof(int);
        int fd = gr_recv(r, &x, &x_len);
        printf("gr_recv: %d %lu %d\n", fd, x_len, x);
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
        size_t xi_len = 16lu;
        fd = gr_recv(r, xi, &xi_len);
        printf("gr_recv: %d %lu %s\n", fd, xi_len, xi);
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
