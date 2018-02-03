/*
 * Driving.h
 *
 *  Created on: 14.01.2018
 *      Author: mice
 */

#ifndef DRIVING_DRIVING_H_
#define DRIVING_DRIVING_H_

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
	parking,
}autonomous_task;


#endif /* DRIVING_DRIVING_H_ */
