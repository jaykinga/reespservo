#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "CommandInfo.h"
#include "DeviceInfo.h"
#include <ArduinoJson.h>

DynamicJsonDocument getDeviceJson();
DEVICEINFO getDeviceInfo(const char *devicename);
COMMANDINFO getCommandInfo(const char *devicename);

void SendUDP(const char *IP, const int desport, const char *data);
void SendUDP(const char *IP, const int desport, const char *data, int datalen);

void StringToHexEnCoder(char to[], const int tolen, const char *from);
/**
 * 给一个MAC地址转换为网咯唤醒包WOLPack[102]
 * 参数 MAC 要转换物理地址，格式为：FF-FF-FF-FF-FF-FF
 * 参数 WOLPack 用来装打包好的wolpack， 长度固定为 102的 字符数组
 */
void MACToWOLPack(const char *MAC, char WOLPack[]);
String ThreadOnDevice(const char *devicename);
String ThreadOffDevice(const char *devicename);

void BatchOnDevice(const char *groupname);
void BatchOffDevice(const char *groupname);

void BatchStop(const char *groupname);

String ThreadStop(const char *devicename);

void Play(const char *devicename);
void Stop(const char *devicename);
void ResetPlay(const char *devicename);
void SetVolume(const char *devicename, int volume);
// #include <map>
// #include "ThreadDeviceClass.h"
// extern std::map<String, ThreadDevice> threadpool;
#endif
