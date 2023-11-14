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


	public:
		CamSensor(string address);
		virtual ~CamSensor() {}
		SensorReading getDeviceReading() override;
};




#endif /* CAMSENSOR_H_ */