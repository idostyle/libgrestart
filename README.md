Graceful Restart Library

```c
gr_init(identifier, identifier_len);
gr_setup(identifier, identifier_len);

gr_recv(gr, fd_identifier, fd_identifier_len);
gr_send(gr, fd, fd_identifier, fd_identifier_len);

gr_poll(fd, timeout);
```
