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

struct USRData{
    shared_ptr<spdlog::logger> logger;
    bool streamAvailable;
};

CamSensor::CamSensor(string address, shared_ptr<spdlog::logger> logger)
{
    deviceAddress = address;
    this->logger = logger;
    readingType = READING_STATE;
}

SensorReading CamSensor::getDeviceReading()
{
    SensorReading reading;
    CURL *curl;
    CURLcode res;
    struct USRData userParams;

    reading.readingType = this->readingType;
    reading.status = STATUS_OK;
    reading.lastReadingValue = 0;

    userParams.streamAvailable = true;
    userParams.logger = logger;

if(counter > 600)
{
 logger->critical("CAM : Begin reading address = {0:s}", deviceAddress);
 logger->flush();
}
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
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &userParams);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CamSensor::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_URL, deviceAddress.c_str());

        /* request server options */
        //snprintf(uri, strlen(url) + 32, "%s", url);
        curl_easy_setopt(curl, CURLOPT_RTSP_STREAM_URI, deviceAddress.c_str());
        curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_OPTIONS);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
          logger->critical("CAM : Setup fails - wrong reading");
          logger->flush();          
          reading.status = STATUS_READING_NOT_READY;
        }

        /* request session description and write response to sdp file */
        curl_easy_setopt(curl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_DESCRIBE);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
          logger->critical("CAM : No response - wrong reading");
          logger->flush();
          reading.status = STATUS_READING_NOT_READY;
        }

if(counter > 600)
{
 logger->critical("CAM : Compleat reading address = {0:s} streamAvailable={1:d}\n-----------------\n", deviceAddress, userParams.streamAvailable);
 logger->flush();
}
        /* cleanup */
        curl_easy_cleanup(curl);
        curl = NULL;

        if (userParams.streamAvailable) reading.lastReadingValue = 1;
      }
      else
      {
          logger->critical("CAM : Cannot initialize CURL session");
          logger->flush();
          reading.status = STATUS_GENERIC_ERR;
      }
      curl_global_cleanup();
    }
    else
    {
        logger->critical("CAM : Cannot initialize CURL");
        logger->flush();
        reading.status = STATUS_GENERIC_ERR;
    }

counter++;
if (counter == 603) counter =0;
    // cout << "CAM SENSOR with address = " << deviceAddress << " Value = " << reading.lastReadingValue << endl;
    return reading;
}

size_t CamSensor::HeaderCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    string headerLine((char*)contents, size * nmemb);
    USRData *userParams = (USRData *)userp;

    if (headerLine.find("404 Stream Not Found") != string::npos)
    {
        userParams->streamAvailable = false;
	if(counter > 600){
            userParams->logger->critical("CAM : Stream not found :  {0:s}", headerLine);
	    userParams->logger->flush();
	}
    }
    return size * nmemb;
}

size_t CamSensor::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    // parser for rtsp capabilities     
    //((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int CamSensor::counter = 0;