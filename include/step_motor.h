#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <include/geometry.h>

/* The step motor being used (NEMA 17 - Unipolar)
   has 200 steps per revolution and 1 cm diameter */

#define MOTOR_RADIUS 5 /* (in mm) */
#define STEPS_PER_REV 200
#define MM_PER_STEP 0.157079633 /* (in mm) */

#define UP 1
#define DOWN 0
#define RIGHT 1
#define LEFT 0

typedef unsigned bool;

struct step_motor {
	bool pulse;
	bool direction;
};

struct step_motor create_step_motor(bool status, bool direction);

void set_step_motor_status(struct step_motor* step_motor, bool status);

void set_step_motor_direction(struct step_motor* step_motor, bool direction);

double control_motors(struct coordinates_pair* cp);

void precompute_drawing_total_steps(struct coordinates_pair *trajectories_array, int num_trajectories);

void track_drawing_progress(void);

#endif
