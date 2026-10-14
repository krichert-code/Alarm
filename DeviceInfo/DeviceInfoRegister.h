/*
 * DeviceInfoRegister.h
 *
 *  Created on: 14 lis 2019
 *      Author: dom
 */

#ifndef DEVICEINFO_DEVICEINFOREGISTER_H_
#define DEVICEINFO_DEVICEINFOREGISTER_H_

#include <DeviceInfoInterface.h>
#include <vector>
#include <map>
#include <memory>
#include <array>
#include <functional>

using namespace std;

class DeviceInfoRegister
{
	private:
		vector<shared_ptr<DeviceInfoInterface>> deviceInfoSource;

	public:
		DeviceInfoRegister();
		xmlResponse getConfigurationDeviceInfo(int deviceId);
		xmlResponse getConfigurationDeviceInfo(string sensorName);
		xmlResponse getReadingDeviceInfo(int deviceId);
		xmlResponse changeSensorReadingValue(string sensorName, const int reading);

		xmlResponse getPresenceDeviceInfo();
		xmlResponse getTemperatureDeviceInfo();
		xmlResponse getEnergyDeviceInfo();
		xmlResponse getStateDeviceInfo();


};


#endif /* DEVICEINFO_DEVICEINFOREGISTER_H_ */
