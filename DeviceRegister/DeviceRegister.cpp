/*
 * DeviceRegister.cpp
 *
 *  Created on: 20 paź 2019
 *      Author: dom
 */
#include <DeviceRegister.h>
#include "MoveSensor.h"
#include "TempSensor.h"
#include "AlarmSensor.h"
#include "EnergySensor.h"
#include "CamSensor.h"
#include "StateSensor.h"
#include "SwitchSensor.h"
#include "DevicesConfiguration.h"

DevicesRegister::DevicesRegister()
{
	deviceRegister.clear();
}


void DevicesRegister::registerDevices()
{
	shared_ptr<DeviceInfoInterface> config = DevicesConfiguration::getInstance();
	SensorParameters sensorConfiguration;
	int deviceId = 0;

	try
	{
		do
		{
			sensorConfiguration = any_cast<SensorParameters>(config->getData(deviceId));
			if (true == sensorConfiguration.enabled)
			{
				deviceRegister[deviceId] = deviceType[sensorConfiguration.sensorType](sensorConfiguration.sensorAddress);
			}
			deviceId++;
		}
		while (sensorConfiguration.status == STATUS_OK);
	}
	catch (bad_any_cast &e)
	{
		//todo: log critical event
	}
}

const vector<int> DevicesRegister::getRegistredDevicesId()
{
	vector<int> devicesId;

	for (auto &deviceElement : deviceRegister)
	{
		devicesId.push_back(deviceElement.first);
	}

	return devicesId;
}

const shared_ptr<DeviceInterface> DevicesRegister::getRegisteredDevice(const int deviceId)
{
	//todo: checki if it can throw exception if index is out of the range - catch it in scheduler
	return deviceRegister[deviceId];
}


constexpr int DevicesRegister::deviceTypes;
array<function<shared_ptr<DeviceInterface>(string)>, DevicesRegister::deviceTypes> DevicesRegister::deviceType =
{
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<MoveSensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<TempSensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<EnergySensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<StateSensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<SwitchSensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<CamSensor>(address); },
		[](string address)->shared_ptr<DeviceInterface> { return make_shared<AlarmSensor>(address); }
};
