#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>

#define infinity 20

struct coordinate {
	double x, y;
};

struct triangle {
	struct coordinate vertices[3];
};

struct rectangle {
	struct coordinate vertices[4];
};

struct circle {
	struct coordinate vertices[infinity];
};

struct coordinates_pair {
	struct coordinate first, second;
};

struct rectangle create_rectangle(double height, double length, struct coordinate center);

struct triangle create_triangle(double height, double length, struct coordinate center);

struct circle create_circle(double radius, struct coordinate center);

double euclidean_distance(struct coordinate source, struct coordinate destiny);

#endif
