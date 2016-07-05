#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <include/drawing.h>
#include <include/geometry.h>
#include <include/server.h>
#include <include/step_motor.h>

#include <arpa/inet.h>
#include <sys/socket.h>

int main(void) {
	int option;
	start_server();
	wait_connection();
	process_request();
	close(server_socket);
	return 0;
}

void start_server(void) {
	struct sockaddr_in server_address;
	
	if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error in server's socket creation.\n");
		exit(EXIT_FAILURE);
	}

	memset(&server_address, 0, sizeof server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(SERVER_PORT);

	if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		printf("Error in assigning port number to server socket.\n");
		exit(EXIT_FAILURE);
	}

	if(listen(server_socket, 10) == -1) {
		printf("Error in creating request queue to server socket.\n");
		exit(EXIT_FAILURE);
	}	

	printf("Server successfully started.\n\n");
}

void wait_connection(void) {
	int client_length = sizeof(client_socket);
	struct sockaddr_in client_address;
	printf("Waiting for clients connections. . .\n");
	if((client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_length)) == -1) {
		printf("Failure on client's socket connection acception.\n");
		exit(EXIT_FAILURE);
	}

	printf("Connection established between client %s and server\n", inet_ntoa(client_address.sin_addr));
}

void recv_var(void* var, size_t var_sz) {
	char request_data[32];
	recv(client_socket, request_data, var_sz, 0);	
	memcpy(var, request_data, var_sz);
}

void process_request(void) {
	int geometric_figure_choice;
	recv_var(&geometric_figure_choice, sizeof(int));

	struct rectangle rectangle;
	struct triangle triangle;
	struct circle circle;
	double height, length, radius;
	switch(geometric_figure_choice) {
		case 1:
		case 2:
			recv_var(&height, sizeof(double));
			recv_var(&length, sizeof(double));
			break;
		case 3:
			recv_var(&radius, sizeof(double));
			break;
	}

	struct coordinate center;
	recv_var(&center.x, sizeof(double));
	recv_var(&center.y, sizeof(double));

	double draw_velocity;
	recv_var(&draw_velocity, sizeof(double));

	switch(geometric_figure_choice) {
		case 1:
			rectangle = create_rectangle(height, length, center);
			draw_rectangle(rectangle, draw_velocity);
			break;
		case 2:
			triangle = create_triangle(height, length, center);
			draw_triangle(triangle, draw_velocity);
			break;
		case 3:
			circle = create_circle(radius, center);
			draw_circle(circle, draw_velocity);
			break;
	}
}
