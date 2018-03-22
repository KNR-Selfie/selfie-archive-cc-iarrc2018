/*
 * Allshit.cpp
 *
 *  Created on: 22.03.2018
 *      Author: mice
 */
#include "cmsis_os.h"
#include "Allshit.h"

class TestClass{
	float nic = 0;
public:
	float cos = 5.f;
	void init (void);
	TestClass();
	~TestClass();
};
TestClass::TestClass():nic(5){};
void TestClass::init(void){
	cos+=nic;
}
TestClass::~TestClass(){};

TestClass testclass;

void StartLightingTask(void const * argument) {
	for (;;) {
		testclass.init();
		osDelay(1);
	}

}
void StartGyroTask(void const * argument) {
	for (;;) {
		testclass.cos -= 3.f;
		osDelay(1);
	}

}
void StartBatteryManager(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
void StartSteeringTask(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
void StartBTTask(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
void StartCzujnikiTask(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
void StartFutabaTask(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
void StartGovernorTask(void const * argument) {
	for (;;) {
		osDelay(1);
	}

}
