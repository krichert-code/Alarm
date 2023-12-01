/*
 * CamSensor.cpp
 *
 *  Created on: 11 lis 2023
 *      Author: dom
 */

#include "CamSensor.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <curl/curl.h>

CamSensor::CamSensor(string address)
{
    deviceAddress = address;
    readingType = READING_STATE;
}

/*
SensorReading CamSensor::getDeviceReading()
{
    SensorReading reading;
//ffprobe -v quiet -print_format json -show_streams  -stimeout 10 rtsp://
    string cmd = "ffprobe -hide_banner  -stimeout 8000 " + deviceAddress + " 2>&1";    
    array<char, 128> buffer;
    string result;

    
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    reading.readingType = this->readingType;

    if (result.find("404 Stream Not Found") == string::npos) reading.lastReadingValue = 1;
        else reading.lastReadingValue = 0;
	
	reading.status = STATUS_OK;
    
	// cout << "CAM SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
	return reading;
}
*/
SensorReading CamSensor::getDeviceReading()
{
    SensorReading reading;
    CURL *curl;
    CURLcode res;

    streamAvailable = true;
    reading.readingType = this->readingType;
    reading.status = STATUS_OK;
    reading.lastReadingValue = 0;

   /* initialize curl */
    res = curl_global_init(CURL_GLOBAL_ALL);
    if(res == CURLE_OK)
    {
      /* initialize this curl session */
      curl = curl_easy_init();
      if(curl)
      {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CamSensor::HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CamSensor::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_URL, deviceAddress.c_str());

        /* request server options */
        //snprintf(uri, strlen(url) + 32, "%s", url);
        curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, deviceAddress.c_str());
        curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_OPTIONS);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
          reading.status = STATUS_READING_NOT_READY;
        }

        /* request session description and write response to sdp file */
        curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_DESCRIBE);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
          reading.status = STATUS_READING_NOT_READY;
        }

        /* cleanup */
        curl_easy_cleanup(curl);
        curl = NULL;

        if (streamAvailable) reading.lastReadingValue = 1;
      }
      else
      {
          reading.status = STATUS_GENERIC_ERR;
      }
      curl_global_cleanup();
    }
    else
    {
        reading.status = STATUS_GENERIC_ERR;
    }

    // cout << "CAM SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
    return reading;
}

size_t CamSensor::HeaderCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    string headerLine((char*)contents, size * nmemb);    
    if (headerLine.find("404 Stream Not Found") != string::npos)
        streamAvailable = false;
    return size * nmemb;
}

size_t CamSensor::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    // parser for rtsp capabilities     
    //((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


bool CamSensor::streamAvailable = false;
