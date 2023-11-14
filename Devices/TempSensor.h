/*
 * TempSensor.h
 *
 *  Created on: 10 lis 2019
 *      Author: dom
 */

#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

#include <DeviceInterface.h>
#include "CommonDefs.h"
#include <iostream>
#include <mutex>
#include <thread>


using namespace std;


class TempSensor : public DeviceInterface
{
	private:
		string deviceAddress;
		SensorReading reading;
		const string devices_directory = "/sys/bus/w1/devices/";
		const string reading_file = "/w1_slave";

    public:
		TempSensor(string address);
		SensorReading getDeviceReading() override;
		virtual ~TempSensor() {}
};

#endif /* TEMPSENSOR_H_ */
