#ifndef CLIENT_H
#define CLIENT_H
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

char* geometric_figure_name[4] = {NULL, "rectangle", "triangle", "circle"};

int client_socket;

void print_header(void);

void start_client(void);

void show_menu(void);

void recv_drawing_progress(void);

void send_var(void* var, size_t var_sz);

#endif
