/*
 * Initializer.cpp
 *
 *  Created on: 1 paź 2019
 *      Author: krichert
 */
#include "Initializer.h"
#include "MoveSensor.h"
#include "Scheduler.h"
#include "RestApiService.h"
#include "pistache/endpoint.h"

Initializer::Initializer() {}
Initializer * Initializer::getInstance()
{
	synch.lock();
	if (initializerInstance == nullptr)
	{
		initializerInstance = new Initializer();
	}
	synch.unlock();

	return initializerInstance;
}

void Initializer::init(int argc, char *params[])
{
	port = "*:9080";
	//todo:read input parameters ./alarm port=80 configile="Alarm.xml"
	// parse input parameters (Alarm httpPort=80 configFile=Alarm.xml)
	if (argc > 1)
	{
		string port_number(params[1]);		
		port = "*:" + port_number;		
	}
}

void Initializer::begin()
{
	// starts all threads	
	thread sensorsThread[SENSOR_MAX];

	for (auto sensorType=0; sensorType<SENSOR_MAX; sensorType++)
    	sensorsThread[sensorType] = thread(Scheduler(), sensorType);

	Pistache::Http::listenAndServe<RestApiService>(static_cast<Pistache::Address>(port));

	for (auto sensorType=0; sensorType<SENSOR_MAX; sensorType++)
		sensorsThread[sensorType].join();
}


Initializer *Initializer::initializerInstance = nullptr;
mutex Initializer::synch;


