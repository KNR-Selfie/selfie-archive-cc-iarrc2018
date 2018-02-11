/*
 * Governor.h
 *
 *  Created on: 14.01.2018
 *      Author: mice
 */

#ifndef GOVERNOR_GOVERNOR_H_
#define GOVERNOR_GOVERNOR_H_

enum driving_state_e
{
	disarmed = 0,
	paused,
	autonomous,
	semi,
	fullcontrol,
}driving_state;

enum autonomous_task_e
{
	await = 0,
	stopped,
	lanefollower,
	laneswitch,
	crossing,
	parkingsearch,
	parking,
}autonomous_task;

float parking_depth;
float parking_turn_sharpness;
float parking_dead_fwd;

#endif /* GOVERNOR_GOVERNOR_H_ */
