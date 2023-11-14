/*
 * Scheduler.cpp
 *
 *  Created on: 17 paź 2019
 *      Author: dom
 */

#include <memory>
#include "Scheduler.h"
#include "DevicesConfiguration.h"
#include "SensorEventsDatabase.h"


int  Scheduler::schedulerThreadFunction(SensorType sensorType)
{
	int tenthOfmilisec = 0;
	int scanningTime = 0;
	DevicesRegister devRegister;
	shared_ptr<DeviceInfoInterface> deviceConfiguration = DevicesConfiguration::getInstance();
	vector<int> devicesId;
	int scanningPeriod = 0;
	DeviceInfoData reading;

	devRegister.registerDevices();
	devicesId = devRegister.getRegistredDevicesId();

	// setup consumers of readings
	for (auto &deviceId : devicesId)
	{
		if (sensorType != any_cast<SensorParameters>(deviceConfiguration->getData(deviceId)).sensorType)
				continue;

		for (auto &source : readingSources)
			source->prepareDeviceInfoSetup(deviceId, devRegister.getRegisteredDevice(deviceId));
	}

	//start to read sensors
	while(1)
	{
		for (auto &deviceId : devicesId)
		{
			if (sensorType != any_cast<SensorParameters>(deviceConfiguration->getData(deviceId)).sensorType)
				continue;
			
			try
			{
				scanningPeriod = any_cast<SensorParameters>(deviceConfiguration->getData(deviceId)).scanningPeriod;
			}
			catch(bad_any_cast &e)
			{
				scanningPeriod = 1;
				//todo:wrong reading - log it
			}

			//if (0 == (tenthOfmilisec % scanningPeriod))
			if (0 == (scanningTime % scanningPeriod))
			{
				reading = readingSources[0]->getData(deviceId);

				if (any_cast<DatabaseReadingEntry>(reading).setupDeviceWithNewReadingValue)
				{
					devRegister.getRegisteredDevice(deviceId)->updateDeviceReading(any_cast<DatabaseReadingEntry>(reading).newReadingValue);
					readingSources[0]->clearNewReadingValue(deviceId);
				}

				//devRegister.getRegisteredDevice(deviceId)->updateDeviceReading(reading.);
				///todo:handle the status (may return value different then success)
				//feed all connected sources
				reading = devRegister.getRegisteredDevice(deviceId)->getDeviceReading();

				for (auto &source : readingSources)
					source->setData(deviceId, reading);
			}
		}
	
		// this_thread::sleep_for(chrono::milliseconds(scanningTimer));
		// tenthOfmilisec = (tenthOfmilisec % 10);
		// tenthOfmilisec++;
		this_thread::sleep_for(chrono::milliseconds(1000));		
		scanningTime++;
	}

	return STATUS_OK;
}

void Scheduler::operator ()(int sensorType)
{
	schedulerThreadFunction(static_cast<SensorType>(sensorType));
	cout << "Scheduler : end thread" << sensorType << endl;
}
