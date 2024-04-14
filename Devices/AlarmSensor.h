/*
 * AlarmSensor.h
 *
 *  Created on: 10 gru 2019
 *      Author: dom
 */

#ifndef DEVICES_ALARMSENSOR_H_
#define DEVICES_ALARMSENSOR_H_


#include <DeviceInterface.h>
#include "CommonDefs.h"
#include "AlarmTriggerInterface.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


using namespace std;

class AlarmSensor : public DeviceInterface, public AlarmTriggerInterface
{
	private:
		const string deviceAddress;
		const ReadingType readingType;


    public:
		AlarmSensor(string address, shared_ptr<spdlog::logger> logger) : deviceAddress(address), readingType(READING_STATE) {}

		virtual ~AlarmSensor() {}

		SensorReading getDeviceReading() override;
		Status triggerAlarm() override { cout << "!!!!!!!!!!!!!!!!!!!!!!" << endl;}
		Status disableAlarm() override {}
};



#endif /* DEVICES_ALARMSENSOR_H_ */
