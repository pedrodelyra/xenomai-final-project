#include <include/geometry.h>

struct rectangle create_rectangle(double height, double length, struct coordinate center) {
	struct rectangle new_rectangle;
	double half_height = height / 2, half_length = length / 2;

	new_rectangle.vertices[0].x = center.x - half_length;
	new_rectangle.vertices[0].y = center.y - half_height;

	new_rectangle.vertices[1].x = center.x - half_length;
	new_rectangle.vertices[1].y = center.y + half_height;

	new_rectangle.vertices[2].x = center.x + half_length;
	new_rectangle.vertices[2].y = center.y + half_height;

	new_rectangle.vertices[3].x = center.x + half_length;
	new_rectangle.vertices[3].y = center.y - half_height;

	return new_rectangle;
}

struct triangle create_triangle(double height, double length, struct coordinate center) {
	struct triangle new_triangle;

	new_triangle.vertices[0].x = center.x - length / 2;
	new_triangle.vertices[0].y = center.y - height / 3;

	new_triangle.vertices[1].x = center.x;
	new_triangle.vertices[1].y = center.y + 2 * (height / 3);

	new_triangle.vertices[2].x = center.x + length / 2;
	new_triangle.vertices[2].y = center.y - (height / 3);

	return new_triangle;
}

struct circle create_circle(double radius, struct coordinate center) {
	struct circle new_circle;

	int i;
	for(i = 0; i < infinity; ++i) {
		new_circle.vertices[i].x = center.x - radius * cos(2 * M_PI * i / (double) infinity);
		new_circle.vertices[i].y = center.y + radius * sin(2 * M_PI * i / (double) infinity);
	}

	return new_circle;
}

double euclidean_distance(struct coordinate source, struct coordinate destiny) {
	double dx_squared = (source.x - destiny.x) * (source.x - destiny.x);
	double dy_squared = (source.y - destiny.y) * (source.y - destiny.y);
	return sqrt(dx_squared + dy_squared);
}
