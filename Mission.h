#ifndef _MISSION_H_
#define _MISSION_H_
#include <Ticker.h>
#include <time.h>
#include "Controller.h"
#include "HTTPServer.h"
Ticker TUpdateTime;
Ticker TListenMisson; //任务监听器
String MissionType = "";

char *getCurrentTime()
{
    time_t now = time(nullptr);
    static char timestr[32] = "Obtain the system time first.";
    if (now > 8 * 3600 * 2)
    {
        struct tm tmnow;
        localtime_r(&now, &tmnow);
        String currenttime = "";
        currenttime = currenttime + (tmnow.tm_year + 1900) + "/";
        currenttime = currenttime + tmnow.tm_mon + "/";
        currenttime = currenttime + tmnow.tm_mday + "  ";
        if (tmnow.tm_hour < 10)
        {
            currenttime = currenttime + "0" + tmnow.tm_hour + ":";
        }
        else
        {
            currenttime = currenttime + tmnow.tm_hour + ":";
        }
        if (tmnow.tm_min < 10)
        {
            currenttime = currenttime + "0" + tmnow.tm_min + ":";
        }
        else
        {
            currenttime = currenttime + tmnow.tm_min + ":";
        }
        if (tmnow.tm_sec < 10)
        {
            currenttime = currenttime + "0" + tmnow.tm_sec + "";
        }
        else
        {
            currenttime = currenttime + tmnow.tm_sec + "";
        }

        memset(timestr, '\0', sizeof(timestr));
        strcpy(timestr, currenttime.c_str());
    }
    return timestr;
}
char *getTimeString()
{

    static char acTimeString[32];
    time_t now = time(nullptr);
    ctime_r(&now, acTimeString);
    size_t stLength;
    while (((stLength = strlen(acTimeString))) &&
           ('\n' == acTimeString[stLength - 1]))
    {
        acTimeString[stLength - 1] = 0; // Remove trailing line break...
    }
    return acTimeString;
}

void getTimeStatus()
{
    time_t now = time(nullptr); // Secs since 01.01.1970 (when uninitalized starts with (8 * 3600 = )
    if (now < 8 * 3600 * 2)
    {
        Serial.printf("Waiting for NTP time sync.\n");
        now = time(nullptr);
    }
    else
    {
        // getCurrentTime();
        //  Serial.printf("Current time: %s\n", getTimeString());
        Serial.printf("ESPTime[%s]\n", getCurrentTime());
        TUpdateTime.detach();
    }
}

void getMission()
{
    //  struct tm
    //{
    //  int tm_sec;   /* 秒，范围从 0 到 59        */
    //  int tm_min;   /* 分，范围从 0 到 59        */
    //  int tm_hour;  /* 小时，范围从 0 到 23        */
    //  int tm_mday;  /* 一月中的第几天，范围从 1 到 31    */
    //  int tm_mon;   /* 月，范围从 0 到 11        */
    //  int tm_year;  /* 自 1900 年起的年数        */
    //  int tm_wday;  /* 一周中的第几天，范围从 0 到 6    */
    //  int tm_yday;  /* 一年中的第几天，范围从 0 到 365    */
    //  int tm_isdst; /* 夏令时                */
    // };
    time_t now = time(nullptr);
    if (now > 8 * 3600 * 2)
    {
        struct tm tmnow;
        localtime_r(&now, &tmnow);
        int weekday = tmnow.tm_wday;
        int hour(tmnow.tm_hour);
        int minute(tmnow.tm_min);
        Serial.printf("ESPTime[%s]\n", getCurrentTime());
        Serial.println("EveryDayMission is Running!");

        if (hour == 23 && (minute == 58 || minute == 59))
        {
            MissionType = "RESTART";
            return;
        }
        if (weekday == 0 || weekday == 6)
        {
            MissionType = "HOLIDAY";
            return;
        }
        if (hour == 8 && (minute - 55 >= 0) && (minute - 55 <= 5))
        {
            MissionType = "ON";
            return;
        }
        if (hour == 11 && (minute - 55 >= 0) && (minute - 55 <= 5))
        {
            MissionType = "OFF";
            return;
        }
        if (hour == 13 && (minute - 0 >= 0) && (minute - 0 <= 5))
        {
            MissionType = "ON";
            return;
        }
        if (hour == 17 && (minute - 0 >= 0) && (minute - 0 <= 5))
        {
            MissionType = "OFF";
            return;
        }
    }
}

void handleMission()
{
    if (MissionType == "RESTART")
    {
        // Serial.printf("System reset,reset time is %s", getTimeString());
        Serial.printf("System reset,reset time is %s", getCurrentTime());
        ESP.reset();
    }
    if (MissionType == "HOLIDAY")
    {
    }
    if (MissionType == "ON")
    {
        BatchOnDevice("前门设备");
        toumingpingOn();
        
    }
    if (MissionType == "OFF")
    {
        BatchOffDevice("前门设备");
        toumingpingOff();
    }
    MissionType = "";
}
#endif