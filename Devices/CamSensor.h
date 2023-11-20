/*
 * CamSensor.h
 *
 *  Created on: 11 list 2023
 *      Author: dom
 */

#ifndef CAMSENSOR_H_
#define CAMSENSOR_H_

#include <DeviceInterface.h>
#include "CommonDefs.h"
#include <iostream>

using namespace std;

class CamSensor : public DeviceInterface
{
	private:
		string deviceAddress;
		ReadingType readingType;
		static bool streamAvailable;

	public:
		CamSensor(string address);
		virtual ~CamSensor() {}
		SensorReading getDeviceReading() override;
		static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
		static size_t HeaderCallback(void *contents, size_t size, size_t nmemb, void *userp);
};




#endif /* CAMSENSOR_H_ */