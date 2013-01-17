Graceful Restart Library

```c
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
gr_init(identifier, identifier_len);

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
gr_setup(identifier, identifier_len);


/**
 * Recieve an fd from a running instance.
 *
 * - gr: is expected to be an accepted client socket
 *
 * - fd_identifier: is expected to be allocated memory area or 0
 *
 * - fd_identifier_len:
 *
 * Returns an fd >= 0 or an error code < 0
 */
gr_recv(gr, fd_identifier, fd_identifier_len);

/**
 * Send an fd to the new instance.
 *
 * Returns count of byte sent >= 0 or an error code < 0
 */
gr_send(gr, fd, fd_identifier, fd_identifier_len);


/**
 * Poll if the fd is readable or message is to be recieved.
 *
 * Returns poll events > 0 or an error code < 0
 */
gr_poll(fd, timeout);
```
