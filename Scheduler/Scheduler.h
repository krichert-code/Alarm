/*
 * Scheduler.h
 *
 *  Created on: 17 paź 2019
 *      Author: dom
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


#include <DeviceRegister.h>
#include <DeviceServiceInterface.h>
#include <iostream>
#include <memory>
#include "CommonDefs.h"
#include "array"
#include "SensorEventsDatabase.h"
#include "AlarmService.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"



using namespace std;

class Scheduler
{
	private:
		static constexpr int maxSources = 2;
		array<shared_ptr<DeviceSetupInterface>,maxSources> readingSources;
		int  schedulerThreadFunction(SensorType sensorType, shared_ptr<spdlog::logger> logger);
		const int scanningTimer = 100;

	public:
		Scheduler() : readingSources(
				{
					SensorEventsDatabase::getInstance(),
					AlarmService::getInstance()
				}
		){}

		void operator()(int sensorType, shared_ptr<spdlog::logger> logger);
};

#endif /* SCHEDULER_H_ */
