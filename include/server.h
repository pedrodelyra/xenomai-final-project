#ifndef SERVER_H
#define SERVER_H

#define SERVER_PORT 8080

int server_socket, client_socket;

void start_server(void);

void wait_connection(void);

void process_request(void);

#endif
