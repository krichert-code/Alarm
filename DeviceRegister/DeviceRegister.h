#include <DeviceInterface.h>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace std;

class DevicesRegister
{
	private:
		map<int, shared_ptr<DeviceInterface>> deviceRegister;
		static constexpr int deviceTypes = SENSOR_MAX;
		static array<function<shared_ptr<DeviceInterface>(string, shared_ptr<spdlog::logger>)>, deviceTypes> deviceType;

	public:
		DevicesRegister();
		void registerDevices(SensorType sensorType, shared_ptr<spdlog::logger> logger);
		const vector<int> getRegistredDevicesId();
		const shared_ptr<DeviceInterface> getRegisteredDevice(const int deviceId);
};
