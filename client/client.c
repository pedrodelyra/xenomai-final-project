#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <include/client.h>
#include <include/geometry.h>

int main(void) {
	start_client();
	print_header();
	show_menu();
	recv_drawing_progress();
	close(client_socket);
	return 0;
}

void start_client(void) {
	struct sockaddr_in server_address;
	
	if((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error while creating client's socket.\n");
	}

	memset(&server_address, 0, sizeof server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_address.sin_port = htons(SERVER_PORT);

	if(connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		printf("Error while establishing connection with server.\n");
		exit(EXIT_FAILURE);
	}
}

extern char* geometric_figure_name[4];

void show_menu(void) {
	/* Geometric figure selection */
	int geometric_figure_choice;
	printf("Please select the geometric figure you want to draw:\n1. Rectangle\t2. Triangle\t3. Circle\n");
	scanf("%d", &geometric_figure_choice);
	send_var(&geometric_figure_choice, sizeof(int));

	/* Figure dimensions specification */
	double height, length, radius;
	switch(geometric_figure_choice) {
		case 1:
		case 2:
			printf("Please enter the %s's height (mm): ", geometric_figure_name[geometric_figure_choice]);
			scanf("%lf", &height);
			send_var(&height, sizeof(double));
			printf("Please enter the %s's length (mm): ", geometric_figure_name[geometric_figure_choice]);
			scanf("%lf", &length);
			send_var(&length, sizeof(double));
			break;
		case 3:
			printf("Please enter the %s's radius (mm): ", geometric_figure_name[geometric_figure_choice]);
			scanf("%lf", &radius);
			send_var(&radius, sizeof(double));
			break;
	}

	/* Read geometric figure's center position */
	struct coordinate center;
	printf("Please enter the %s's center position coordinates (mm): ", geometric_figure_name[geometric_figure_choice]);	
	scanf("%lf %lf", &center.x, &center.y);
	send_var(&center.x, sizeof(double));
	send_var(&center.y, sizeof(double));

	/* Read drawing's velocity */
	double draw_velocity;
	printf("Please enter the drawing's velocity (mm/s): ");
	scanf("%lf", &draw_velocity);
	send_var(&draw_velocity, sizeof(double));
	printf("\n\n");
}

void recv_drawing_progress(void) {
	char response_message[127];
	while(recv(client_socket, response_message, sizeof(response_message), 0), *response_message) {
		printf("%s\n", response_message);
	}

	printf("Bye!\n");
}

void print_header() {
	printf("--------------------------------------------------------------------------------\n"
	       "- University of Brasília - FGA                                                 -\n"
	       "- Alumnus: Pedro de Lyra Pereira - 11/0135725                                  -\n"
 	       "- Professor: Renato Coral Sampaio                                              -\n"
	       "- Subject: Embedded Systems (Electronic Engineering)                           -\n"
	       "- Final Project - Remote control of an xy table driven by stepper motors       -\n"
	       "--------------------------------------------------------------------------------\n\n");
}

void send_var(void* var, size_t var_sz) {
	char request_message[255];
	memcpy(request_message, var, var_sz);
	send(client_socket, request_message, var_sz, 0);
}
