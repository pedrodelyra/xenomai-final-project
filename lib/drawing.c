#include <stdio.h>

#include <sys/socket.h>

#include <include/drawing.h>
#include <include/step_motor.h>

double drawing_velocity;

static struct coordinate origin = {.x = 0.0, .y = 0.0};

extern int client_socket;

void draw_rectangle(struct rectangle rectangle, double velocity) {
	struct coordinates_pair trajectories[5];

	drawing_velocity = velocity;

	trajectories[0].first = origin;
	trajectories[0].second = rectangle.vertices[0];

	int i;
	for(i = 1; i <= 4; ++i) {
		trajectories[i].first = rectangle.vertices[i - 1];
		trajectories[i].second = rectangle.vertices[i % 4];
	}

	precompute_drawing_total_steps(trajectories, 5);

	double total_drawing_time = 0.0;
	printf("Drawing rectangle. . .\n");
	for(i = 0; i < 5; ++i) {
		if((fabs(trajectories[i].first.x - trajectories[i].second.x) < 1e-6) &&
		   (fabs(trajectories[i].first.y - trajectories[i].second.y) < 1e-6)) continue;

		printf("Source: (%.2lf, %.2lf) -> Destiny (%.2lf, %.2lf)\n", trajectories[i].first.x, trajectories[i].first.y,
									     trajectories[i].second.x, trajectories[i].second.y);
		track_drawing_progress();
		total_drawing_time += control_motors(&trajectories[i]);
		printf("--------------------------------------------------------------------------------\n\n");
	}

	printf("Rectangle vertices:");
	for(i = 0; i < 4; ++i)
		printf(" %c(%.2lf, %.2lf)", 'A' + i, rectangle.vertices[i].x, rectangle.vertices[i].y);

	printf("\nTime spent to draw the rectangle: %lfs\n", total_drawing_time);

	// Finish connection
	char finish_connection_flag = 0;
	send(client_socket, &finish_connection_flag, sizeof(finish_connection_flag), 0);
}

void draw_triangle(struct triangle triangle, double velocity) {
	struct coordinates_pair trajectories[4];

	drawing_velocity = velocity;

	trajectories[0].first = origin;
	trajectories[0].second = triangle.vertices[0];

	int i;
	for(i = 1; i <= 3; ++i) {
		trajectories[i].first = triangle.vertices[i - 1];
		trajectories[i].second = triangle.vertices[i % 3];
	}

	precompute_drawing_total_steps(trajectories, 4);

	double total_drawing_time = 0.0;
	printf("Drawing triangle. . .\n");
	for(i = 0; i < 4; ++i) {
		if((fabs(trajectories[i].first.x - trajectories[i].second.x) < 1e-6) &&
		   (fabs(trajectories[i].first.y - trajectories[i].second.y) < 1e-6)) continue;

		printf("Source: (%.2lf, %.2lf) -> Destiny (%.2lf, %.2lf)\n", trajectories[i].first.x, trajectories[i].first.y,
									     trajectories[i].second.x, trajectories[i].second.y);
		track_drawing_progress();
		total_drawing_time += control_motors(&trajectories[i]);
		printf("--------------------------------------------------------------------------------\n\n");
	}

	printf("Triangle vertices:");
	for(i = 0; i < 3; ++i)
		printf(" %c(%.2lf, %.2lf)", 'A' + i, triangle.vertices[i].x, triangle.vertices[i].y);

	printf("\nTime spent to draw the triangle: %lfs\n", total_drawing_time);

	// Finish connection
	char finish_connection_flag = 0;
	send(client_socket, &finish_connection_flag, sizeof(finish_connection_flag), 0);
}

void draw_circle(struct circle circle, double velocity) {
	struct coordinates_pair trajectories[infinity + 1];

	drawing_velocity = velocity;

	trajectories[0].first = origin;
	trajectories[0].second = circle.vertices[0];

	int i;
	for(i = 1; i <= infinity; ++i) {
		trajectories[i].first = circle.vertices[i - 1];
		trajectories[i].second = circle.vertices[i % infinity];
	}

	precompute_drawing_total_steps(trajectories, infinity + 1);

	double total_drawing_time = 0.0;
	printf("Drawing circle. . .\n");
	for(i = 0; i <= infinity; ++i) {
		if((fabs(trajectories[i].first.x - trajectories[i].second.x) < 1e-6) &&
		   (fabs(trajectories[i].first.y - trajectories[i].second.y) < 1e-6)) continue;

		printf("Source: (%.2lf, %.2lf) -> Destiny (%.2lf, %.2lf)\n", trajectories[i].first.x, trajectories[i].first.y,
									     trajectories[i].second.x, trajectories[i].second.y);
		track_drawing_progress();
		total_drawing_time += control_motors(&trajectories[i]);
		printf("--------------------------------------------------------------------------------\n\n");
	}

	printf("Circle vertices:");
	for(i = 0; i < infinity; ++i) {
		if((i % 4) == 0) printf("\n");
		printf("%c(%6.2lf, %6.2lf) ", 'A' + i, circle.vertices[i].x, circle.vertices[i].y);
	}

	printf("\nTime spent to draw the circle: %lfs\n", total_drawing_time);

	// Finish connection
	char finish_connection_flag = 0;
	send(client_socket, &finish_connection_flag, sizeof(finish_connection_flag), 0);
}
