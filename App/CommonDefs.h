/*
 * CommonDefs.h
 *
 *  Created on: 29 paź 2019
 *      Author: dom
 */

#ifndef COMMONDEFS_H_
#define COMMONDEFS_H_

#include <string>
#include <chrono>
#include <any>
#include <map>
#include <memory>

using namespace std;

typedef enum Status
{
    STATUS_OK,
    STATUS_SENSOR_NOT_EXIST,
    STATUS_SETTING_NOT_EXIST,
    STATUS_READING_EXCEPTION,
    STATUS_XML_PARSE_ERROR,
    STATUS_XML_NO_MORE_SENSORS,
    STATUS_XML_NO_MORE_SETTINGS,
    STATUS_AUTH_ERR,
    STATUS_GENERIC_ERR,
	STATUS_WRONG_READING,
	STATUS_READING_NOT_READY,
    STATUS_MAX
} Status;


typedef enum ReadingType
{
    READING_INT,
    READING_FIXED_INT,
    READING_STATE,
    READING_MAX
} ReadingType;

typedef enum SensorType
{
    MOVE_SENSOR,
    TEMP_SENSOR,
    ENERGY_SENSOR,
    STATE_SENSOR,
	SWITCH_SENSOR,
    CAMERA_SENSOR,
    ALARM_SENSOR,
    SENSOR_MAX
} SensorType;

typedef struct SettingParameters
{
    string settingName;
    string value;
    Status status;
} SettingParameters;

typedef struct SensorParameters
{
    string sensorName;
    int scanningPeriod;
    SensorType sensorType;
    string sensorAddress;
    int enableThresholdValue;
    bool enabled;
    Status status;
} SensorParameters;


typedef struct SensorReading
{
    int lastReadingValue;
    ReadingType readingType;
    Status status;
} SensorReading;

typedef struct AlarmReading
{
    string sensorName;
    bool isActivate;
    Status status;
} AlarmReading;

typedef struct DatabaseReadingEntry
{
	SensorReading reading;
	chrono::time_point<chrono::system_clock> lastExecedThrReadingTimestamp;
	chrono::time_point<chrono::system_clock> readingTimestamp;
	bool isSensorGeneratingAlarm;
	bool isValidExecedTimeStamp;
    int newReadingValue;
    bool setupDeviceWithNewReadingValue;
} DatabaseReadingEntry;


typedef any DeviceInfoData;

typedef map<int, map<string, string> > xmlResponse;

#endif /* COMMONDEFS_H_ */
