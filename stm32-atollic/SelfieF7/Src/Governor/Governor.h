/*
 * Governor.h
 *
 *  Created on: 14.01.2018
 *      Author: mice
 */

#ifndef GOVERNOR_GOVERNOR_H_
#define GOVERNOR_GOVERNOR_H_

typedef enum driving_state_e
{
	disarmed = 0,
	paused,
	autonomous,
	semi,
	fullcontrol,
}driving_state_t;

driving_state_t driving_state;
enum autonomous_task_e
{
	await = 0,
	stopped,
	lanefollower,
	laneswitch,
	crossing,
	crossing_on_parking,
	parkingsearch,
	parking,
}autonomous_task;

float speed_freerun;
float speed_corners;
float speed_obstacles;
float speed_parking;

float parking_depth;
float parking_turn_sharpness;
float parking_dead_fwd;
float podjedz_pan_distance;

#endif /* GOVERNOR_GOVERNOR_H_ */
