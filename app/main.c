#include <stdio.h>
#include <include/geometry.h>
#include <include/step_motor.h>
#include <include/drawing.h>

void print_header(void);
void show_menu(void);

int main(void) {
	int option;
	print_header();
	show_menu();
	return 0;
}

void print_header() {
	printf("--------------------------------------------------------------------------------\n");
	printf("- University of Brasília - FGA                                                 -\n");
	printf("- Alumnus: Pedro de Lyra Pereira - 11/0135725                                  -\n");
	printf("- Professor: Renato Coral Sampaio                                              -\n");
	printf("- Subject: Embedded Systems (Electronic Engineering)                           -\n");
	printf("- Final Project - Remote control of an xy table driven by stepper motors       -\n");
	printf("--------------------------------------------------------------------------------\n\n");
}

char* geometric_figure_name[4] = {NULL, "rectangle", "triangle", "circle"};
void show_menu(void) {
	int geometric_figure_choice, continue_option;
	printf("Please select the geometric figure you want to draw:\n");
	printf("1. Rectangle\t2. Triangle\t3. Circle\n");
	scanf("%d", &geometric_figure_choice);

	struct rectangle rectangle;
	struct triangle triangle;
	struct circle circle;
	double height, length, radius;
	switch(geometric_figure_choice) {
		case 1:
		case 2:
			printf("Please enter the %s's height (mm): ", geometric_figure_name[geometric_figure_choice]);
			scanf("%lf", &height);
			printf("Please enter the %s's length (mm): ", geometric_figure_name[geometric_figure_choice]);
			scanf("%lf", &length);
			break;
		case 3:
			printf("Please enter the %s's radius (mm): ", geometric_figure_name[geometric_figure_choice]);	
			scanf("%lf", &radius);
			break;
	}

	struct coordinate center;
	printf("Please enter the %s's center position coordinates (mm): ", geometric_figure_name[geometric_figure_choice]);	
	scanf("%lf %lf", &center.x, &center.y);

	double draw_velocity;
	printf("Please enter the drawing's velocity (mm/s): ");	
	scanf("%lf", &draw_velocity);

	int i;
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
