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
        char x = '\0';
        int fd = gr_recv(r, &x, 1lu);
        printf("gr_recv: %d %c\n", fd, x);
        if (fd >= 0)
        {
            write(fd, "Test\n", 5lu);
            close(fd);
        }
        else
        {
            printf("gr_recv errno: %s\n", strerror(errno));
        }

        fd = gr_recv(r, &x, 1lu);
        printf("gr_recv: %d %c\n", fd, x);
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
            int s = gr_send(client, 1, "x", 1lu);
            printf("gr_send: %d\n", s);
            if (s >= 0)
            {
                // party <|:)-/-<
            }
            else
            {
                printf("gr_send errno: %s\n", strerror(errno));
            }

            s = gr_send(client, 2, "x", 1lu);
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

    sleep(5);

    return 0;
}
