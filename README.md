Graceful Restart Library

```c
gr_init(identifier, identifier_len);
gr_setup(identifier, identifier_len);

gr_recv(gr, fd_identifier, fd_identifier_len);
gr_send(gr, fd, fd_identifier, fd_identifier_len);

gr_close(gr);

gr_poll(fd, timeout);

gr_recv_iov(gr, iov);
gr_send_iov(gr, fd, iov);
```


Quick tutorial:
```c
main()
{
    int gr = gr_init("/tmp/foo.sock", 13lu);
    if (gr >= 0)
    {
        // connected to a running instance

        // recieve fd
        int new_fd = gr_recv(gr, 0, 0lu);
        if (new_fd >= 0)
        {
            // got our new main socket ...
        }

        // close gr-socket
        gr_close(gr);
    }
    else if (gr != GR_CONNECT_FAILED)
    {
        // something bad happened, see errno
    }

    ...

    gr = gr_setup("/tmp/foo.sock", 13lu);
    if (gr >= 0)
    {
        // integrate the gr socket into your poll/epoll routine or use gr_poll

        // accept any incomming connections
        {
            gr_send(client, any_fd, 0, 0lu);
            ...
        }

        gr_close(gr);
    }
    else
    {
        // something bad happened, see errno
    }
}
```
