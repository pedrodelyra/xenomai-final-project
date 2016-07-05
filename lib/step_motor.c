#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/mman.h>

#include <rtdk.h>
#include <native/task.h>
#include <native/mutex.h>

#include <include/step_motor.h>

#define MOTOR_X_TASK_NAME "X-axis motor's task"
#define MOTOR_X_TASK_PRIO 50

#define MOTOR_Y_TASK_NAME "Y-axis motor's task"
#define MOTOR_Y_TASK_PRIO 50

#define TASK_START 0
#define TASK_STKSZ 0
#define TASK_MODE T_JOINABLE

#define MUTEX_NAME "mutex"

typedef int rt_priority;

RT_TASK rt_tasks[3], rt_tracker;
char* rt_tasks_names[3]            = {NULL, MOTOR_X_TASK_NAME, MOTOR_Y_TASK_NAME};
rt_priority rt_tasks_priorities[3] = {0, MOTOR_X_TASK_PRIO, MOTOR_Y_TASK_PRIO};
RTIME rt_tasks_periods[3]          = {0};
RT_MUTEX mutex;

void move_motor_x(void* args);
void move_motor_y(void* args);
void monitor_tasks(void* args);

void (*rt_tasks_routine[3])(void*) = {NULL, move_motor_x, move_motor_y};

struct step_motor motor_x, motor_y;

extern int client_socket;
extern double drawing_velocity;

struct step_motor create_step_motor(bool pulse, bool direction) {
	struct step_motor new_step_motor;
	new_step_motor.pulse = pulse;
	new_step_motor.direction = direction;
	return new_step_motor;
}

void clean_up(void);
void execute_motors_rt_tasks(int num_steps_x, int num_steps_y);

int drawing_total_steps;

void precompute_drawing_total_steps(struct coordinates_pair *trajectories, int num_trajectories) {
	int i;
	for(i = drawing_total_steps = 0; i < num_trajectories; ++i) {
		struct coordinate source = trajectories[i].first, destiny = trajectories[i].second;
		double dist_x = fabs(source.x - destiny.x), dist_y = fabs(source.y - destiny.y);
		drawing_total_steps += ceil(dist_x / MM_PER_STEP) + ceil(dist_y / MM_PER_STEP); 
	}
}

static int motors_total_steps = 0;
static int given_steps;
static int current_drawing_steps;

void track_drawing_progress(void) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	if(rt_task_create(&rt_tracker, "Motors task's monitor", TASK_STKSZ, 99, TASK_MODE)) {
		rt_fprintf(stderr, "An error occurred while creating rt_tracker.\n");
		exit(EXIT_FAILURE);
	}

	if(rt_task_set_periodic(&rt_tracker, TASK_START, 1000000000LL)) {
		rt_fprintf(stderr, "An error occurred while setting rt_tracker period.\n");
		exit(EXIT_FAILURE);
	}

	if(rt_mutex_create(&mutex, MUTEX_NAME)) {
		fprintf(stderr, "An error occurred while creating mutex.\n");
		exit(EXIT_FAILURE);	
	}

	if(rt_task_start(&rt_tracker, monitor_tasks, NULL)) {
		rt_fprintf(stderr, "An error occurred while starting rt_tracker.\n");
		exit(EXIT_FAILURE);
	}
}

double control_motors(struct coordinates_pair* cp) {
	struct coordinate source = cp->first, destiny = cp->second;
	double total_distance = euclidean_distance(source, destiny);
	double dist_x = fabs(source.x - destiny.x), dist_y = fabs(source.y - destiny.y);
	double total_time = total_distance / drawing_velocity;
	int total_steps_x = ceil(dist_x / MM_PER_STEP), total_steps_y = ceil(dist_y / MM_PER_STEP); 
	double motor_x_period = total_time / total_steps_x, motor_y_period = total_time / total_steps_y;

	rt_tasks_periods[1] = motor_x_period * 1000000000LL, rt_tasks_periods[2] = motor_y_period * 1000000000LL; // from s to ns

	motor_x = create_step_motor(0, source.x < destiny.x ? RIGHT : LEFT);
	motor_y = create_step_motor(0, source.y < destiny.y ?    UP : DOWN);

	rt_printf("Drawing total time: %lfs\nTotal distance: %lfmm\n", total_time, total_distance);
	rt_printf("X-axis distance: %lfmm\nY-axis distance: %lfmm\n", dist_x, dist_y);
	rt_printf("X-axis motor period: %lfs\nY-axis motor period: %lfs\n\n", motor_x_period, motor_y_period);

	current_drawing_steps = total_steps_x + total_steps_y;
	given_steps = motors_total_steps;

	execute_motors_rt_tasks(total_steps_x, total_steps_y);

	return total_time;
}

void execute_motors_rt_tasks(int num_steps_x, int num_steps_y) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	int rt_task_id, num_steps[3] = {0, num_steps_x, num_steps_y};
	for(rt_task_id = 1; rt_task_id <= 2; ++rt_task_id) {
		if(num_steps[rt_task_id] == 0) continue;

		if(rt_task_create(&rt_tasks[rt_task_id], rt_tasks_names[rt_task_id], TASK_STKSZ, rt_tasks_priorities[rt_task_id], TASK_MODE)) {
			rt_fprintf(stderr, "An error occurred while creating rt_task%d.\n", rt_task_id);
			exit(EXIT_FAILURE);
		}

		if(rt_task_set_periodic(&rt_tasks[rt_task_id], TASK_START, rt_tasks_periods[rt_task_id])) {
			rt_fprintf(stderr, "An error occurred while setting rt_task%d period.\n", rt_task_id);
			exit(EXIT_FAILURE);
		}
	}

	for(rt_task_id = 1; rt_task_id <= 2; ++rt_task_id) {
		if(num_steps[rt_task_id] == 0) continue;

		if(rt_task_start(&rt_tasks[rt_task_id], rt_tasks_routine[rt_task_id], (void*) num_steps[rt_task_id])) {
			rt_fprintf(stderr, "An error occurred while starting rt_task%d.\n", rt_task_id);
			exit(EXIT_FAILURE);
		}
	}

	for(rt_task_id = 1; rt_task_id <= 2; ++rt_task_id) {
		if(num_steps[rt_task_id] == 0) continue;

		if(rt_task_join(&rt_tasks[rt_task_id])) {
			rt_fprintf(stderr, "An error occurred while waiting for rt_task%d to finish.\n", rt_task_id);
			exit(EXIT_FAILURE);
		}
	}

	if(rt_task_join(&rt_tracker)) {
		rt_fprintf(stderr, "An error occurred while waiting for rt_tracker to finish.\n");
		exit(EXIT_FAILURE);
	}

	rt_printf("\n");
	clean_up();
}

void move_motor_x(void* args) {
	int i, num_steps = (int) args;
	
	for(i = 1; i <= num_steps; ++i) {
		rt_printf("X-axis motor %4d step(s) - Pulse: %d %-6s | Direction: %d %-7s\n",
				i, motor_x.pulse, motor_x.pulse ? "(High)" : "(Low)", motor_x.direction, motor_x.direction ? "(Right)" : "(Left)");

		if(rt_task_wait_period(NULL)) {
			rt_fprintf(stderr, "An error occurred while waiting for the next rt_task's periodic release point.\n");
			exit(EXIT_FAILURE);	
		}

		motor_x.pulse = !motor_x.pulse;
		rt_mutex_acquire(&mutex, TM_INFINITE);
		++motors_total_steps;
		rt_mutex_release(&mutex);
	}
}

void move_motor_y(void* args) {
	int i, num_steps = (int) args;

	for(i = 1; i <= num_steps; ++i) {
		rt_printf("Y-axis motor %4d step(s) - Pulse: %d %-6s | Direction: %d %-7s\n",
				i, motor_y.pulse, motor_y.pulse ? "(High)" : "(Low)", motor_y.direction, motor_y.direction ? "(Up)" : "(Down)");

		if(rt_task_wait_period(NULL)) {
			rt_fprintf(stderr, "An error occurred while waiting for the next rt_task's periodic release point.\n");
			exit(EXIT_FAILURE);	
		}

		motor_y.pulse = !motor_y.pulse;
		rt_mutex_acquire(&mutex, TM_INFINITE);
		++motors_total_steps;
		rt_mutex_release(&mutex);
	}
}

void monitor_tasks(void* args) {
	char response_message[512] = {0};
	int current_steps;

begin_tracker:;
	rt_mutex_acquire(&mutex, TM_INFINITE);
	current_steps = motors_total_steps;
	rt_mutex_release(&mutex);

	sprintf(response_message, "*****************************************\n"
				  "** Drawing status: %6.02lf%% (%4d/%4d) **\n"
				  "*****************************************\n", 100 * current_steps / (double) drawing_total_steps,
				     							current_steps, drawing_total_steps);

	send(client_socket, response_message, strlen(response_message), 0);
	if(rt_task_wait_period(NULL)) {
		rt_fprintf(stderr, "An error occurred while waiting for the next rt_tracker's periodic release point.\n");
		exit(EXIT_FAILURE);	
	}

	if((current_steps - given_steps) < current_drawing_steps) goto begin_tracker;
}

void clean_up(void) {
	int i;
	for(i = 1; i <= 2; ++i) {
		rt_task_delete(&rt_tasks[i]);
	}

	rt_task_delete(&rt_tracker);
	rt_mutex_delete(&mutex);
	munlockall();
}
