#include "Controller.h"
#include "ThreadCommand.h"
#include "ThreadDevice.h"
#include <FS.h>
#include <WiFiUdp.h>
#include <map>
char *handleritems = NULL;
typedef DynamicJsonDocument (*PFUN_GETJSON)();
std::map<String, ThreadDevice> threadpool;
std::map<String, ThreadCommand> threadpoolcmd;
DynamicJsonDocument getDeviceJson()
{
    SPIFFS.begin();
    const size_t capacity = 3 * JSON_OBJECT_SIZE(1) + 8 * JSON_OBJECT_SIZE(5) + 8 * JSON_OBJECT_SIZE(6) + 3 * JSON_OBJECT_SIZE(10) + 7 * JSON_OBJECT_SIZE(12) + 2 * JSON_OBJECT_SIZE(16) + JSON_OBJECT_SIZE(34) + JSON_OBJECT_SIZE(38) + 40 * JSON_OBJECT_SIZE(20) + 60; // 建立DynamicJsonDocument对象
    DynamicJsonDocument doc = DynamicJsonDocument(capacity);
    File file = SPIFFS.open("/device.json", "r"); // 从闪存文件系统中读取即将解析的json文件
    deserializeJson(doc, file);                   // 反序列化数据
    file.close();
    return doc;
}
DynamicJsonDocument getCommandJson()
{
    SPIFFS.begin();
    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4) + 10 * JSON_OBJECT_SIZE(8) + 60; // 建立DynamicJsonDocument对象
    DynamicJsonDocument doc = DynamicJsonDocument(capacity);
    File file = SPIFFS.open("/command.json", "r"); // 从闪存文件系统中读取即将解析的json文件
    deserializeJson(doc, file);                    // 反序列化数据
    file.close();
    return doc;
}

COMMANDINFO getCommandInfo(const char *devicename)
{
    COMMANDINFO commandinfo;
    DynamicJsonDocument doc = getCommandJson();
    commandinfo.delaytime = doc[devicename]["DelayTime"];
    commandinfo.destport = doc[devicename]["DestPort"];

    commandinfo.ip = doc[devicename]["IP"].as<String>();
    commandinfo.playcode = doc[devicename]["PlayCode"].as<String>();
    commandinfo.stopcode = doc[devicename]["StopCode"].as<String>();
    commandinfo.resetplay = doc[devicename]["ResetPlay"].as<String>();
    commandinfo.setvolume = doc[devicename]["SetVolume"].as<String>();
    commandinfo.other = doc[devicename]["Other"].as<String>();
    return commandinfo;
}
DEVICEINFO getDeviceInfo(const char *devicename)
{

    DynamicJsonDocument doc = getDeviceJson();
    DEVICEINFO deviceinfo(doc[devicename]["屏幕数量"], doc[devicename]["主机数量"]);
    for (int screenindex = 0; screenindex < deviceinfo.screens; screenindex++)
    {
        //注意doc[][]返回的是一个常量字符串指针，只能用const char * 接收,

        deviceinfo.screeninfo[screenindex].desport = doc[devicename]["屏幕端口" + String(screenindex + 1)];
        deviceinfo.screeninfo[screenindex].ip = doc[devicename]["屏幕IP" + String(screenindex + 1)].as<String>();
        deviceinfo.screeninfo[screenindex].oncode = doc[devicename]["开屏代码" + String(screenindex + 1)].as<String>();
        deviceinfo.screeninfo[screenindex].offcode = doc[devicename]["关屏代码" + String(screenindex + 1)].as<String>();
    }

    for (int pcindex = 0; pcindex < deviceinfo.pcs; pcindex++)
    {
        deviceinfo.pcinfo[pcindex].desport = doc[devicename]["目标端口" + String(pcindex + 1)];
        deviceinfo.pcinfo[pcindex].ip = doc[devicename]["主机IP" + String(pcindex + 1)].as<String>();
        deviceinfo.pcinfo[pcindex].mac = doc[devicename]["主机MAC" + String(pcindex + 1)].as<String>();
        deviceinfo.pcinfo[pcindex].offcode = doc[devicename]["关机代码" + String(pcindex + 1)].as<String>();
    }

    deviceinfo.ondelaytime = doc[devicename]["开机延迟"];
    deviceinfo.offdelaytime = doc[devicename]["关机延迟"];
    return deviceinfo;
}

int getItems(const char *gourpname, char *items[], PFUN_GETJSON getJson)
{
    int count = -1;
    DynamicJsonDocument doc = getJson();
    // char *str = (char *)malloc(256 * sizeof(char));
    char *str = (char *)malloc(strlen(doc[gourpname]["设备名称"]) * sizeof(char) + 1);
    handleritems = str;
    strcpy(str, doc[gourpname]["设备名称"]);
    // Serial.printf("str = %s\n", str);
    char const *delims = "@";
    char *result = NULL;

    result = strtok(str, delims);
    if (result != NULL)
    {
        count = 0;
    }
    while (result != NULL)
    {
        items[count] = result;
        // Serial.printf("result is \"%s\"\n", result);
        // Serial.printf("devicenameitems = %s\n", deviceitems[count]);
        count++;
        result = strtok(NULL, delims);
    }
    return count;
}
void freeItems()
{
    if (handleritems != NULL)
    {
        free(handleritems);
        handleritems = NULL;
    }
    
}

void SendUDP(const char *IP, const int desport, const char *data)
{
    WiFiUDP udp;                  //实例化WiFiUDP对象
    udp.beginPacket(IP, desport); //配置远端ip地址和端口
    udp.write(data);              //把数据写入发送缓冲区
    udp.endPacket();              //发送数据
}
void SendUDP(const char *IP, const int desport, const char *data, int datalen)
{
    WiFiUDP udp;                  //实例化WiFiUDP对象
    udp.beginPacket(IP, desport); //配置远端ip地址和端口
    udp.write(data, datalen);     //把数据写入发送缓冲区
    // upd.writeBytes(data,datalen);
    udp.endPacket(); //发送数据
}

void OnDevice(const char *threadID)
{
    ThreadDevice *thread = &(threadpool[threadID]);
    if (thread->ThreadState == 0b1000)
    {
        for (int screenindex = 0; screenindex < thread->deviceinfo.screens; screenindex++)
        {
            // int len = (strlen(thread->deviceinfo.screeninfo[screenindex].oncode) + 1) / 3;
            int len = (thread->deviceinfo.screeninfo[screenindex].oncode.length() + 1) / 3;
            char EnCoder[len];
            memset(EnCoder, '\0', sizeof(EnCoder));
            StringToHexEnCoder(EnCoder, len, thread->deviceinfo.screeninfo[screenindex].oncode.c_str());
            SendUDP(thread->deviceinfo.screeninfo[screenindex].ip.c_str(), thread->deviceinfo.screeninfo[screenindex].desport, EnCoder, len);
        }
        // Serial.printf("IP = %s\n", thread->deviceinfo.pcinfo[0].IP);
        Serial.printf("设备名称 = %s    ", thread->devicename.c_str());
        Serial.printf("延时时间 = %d    ", thread->deviceinfo.ondelaytime);
        Serial.printf("屏幕开启完成!\n");
        thread->ThreadState = 0b1010;
    }

    if (thread->ondelaytime > 0)
    {
        thread->ondelaytime -= 1000;
        // Serial.printf("thread->ondelaytime=%d\n", thread->ondelaytime);
    }

    if (thread->ThreadState == 0b1010 && thread->ondelaytime <= 0)
    {
        for (int pcindex = 0; pcindex < thread->deviceinfo.pcs; pcindex++)
        {
            char WOLPack[102] = {0};
            // Serial.printf("MAC[%d] = %s\n", pcindex, thread->deviceinfo.pcinfo[pcindex].MAC);
            MACToWOLPack(thread->deviceinfo.pcinfo[pcindex].mac.c_str(), WOLPack);
            SendUDP("192.168.10.255", 0, WOLPack, 102);
        }
        Serial.printf("%s电脑开启完成!\n", thread->devicename.c_str());
        thread->ThreadState = 0b1110;
    }

    if (thread->ThreadState == 0b1110)
    {
        // Serial.printf("%s开启完成!\n", thread->devicename.c_str());
        thread->ThreadState = 0b1111;
    }

    if (thread->ThreadState == 0b1111)
    {
        // Serial.printf("删除开机%s线程完成!\n", thread->devicename.c_str());
        threadpool.erase(thread->ID);
    }
}
void OffDevice(const char *threadID)
{
    /// Serial.println(" OffDevice was called");
    ThreadDevice *thread = &(threadpool[threadID]);
    if (thread->ThreadState == 0b1000)
    {
        for (int pcindex = 0; pcindex < thread->deviceinfo.pcs; pcindex++)
        {
            SendUDP(thread->deviceinfo.pcinfo[pcindex].ip.c_str(), thread->deviceinfo.pcinfo[pcindex].desport, thread->deviceinfo.pcinfo[pcindex].offcode.c_str());
        }

        Serial.printf("设备名称 = %s    ", thread->devicename.c_str());
        Serial.printf("延时时间 = %d    ", thread->deviceinfo.offdelaytime);
        Serial.printf("PC关闭完成!\n");
        thread->ThreadState = 0b1100;
    }

    if (thread->offdelaytime > 0)
    {
        thread->offdelaytime -= 1000;
    }

    if (thread->ThreadState == 0b1100 && thread->offdelaytime <= 0)
    {
        for (int screenindex = 0; screenindex < thread->deviceinfo.screens; screenindex++)
        {

            int len = (thread->deviceinfo.screeninfo[screenindex].offcode.length() + 1) / 3;
            char EnCoder[len];
            memset(EnCoder, '\0', sizeof(EnCoder));
            StringToHexEnCoder(EnCoder, len, thread->deviceinfo.screeninfo[screenindex].offcode.c_str());
            SendUDP(thread->deviceinfo.screeninfo[screenindex].ip.c_str(), thread->deviceinfo.screeninfo[screenindex].desport, EnCoder, len);
            // Serial.printf(thread->deviceinfo.screeninfo[screenindex].ip.c_str());
            // Serial.println(thread->deviceinfo.screeninfo[screenindex].desport);
            // Serial.println(thread->deviceinfo.screeninfo[screenindex].offcode.c_str());
        }
        Serial.printf("%s屏幕关闭完成!\n", thread->devicename.c_str());
        thread->ThreadState = 0b1110;
    }

    if (thread->ThreadState == 0b1110)
    {
        // Serial.printf("%s关闭完成!\n",  thread->devicename.c_str());
        thread->ThreadState = 0b1111;
    }

    if (thread->ThreadState == 0b1111)
    {
        // Serial.printf("删除关机%s线程完成!\n",  thread->devicename.c_str());
        threadpool.erase(thread->ID);
    }
}

void MACToWOLPack(const char *MAC, char WOLPack[])
{

    char HEXMAC[6] = {0};
    StringToHexEnCoder(HEXMAC, 6, MAC); //将传入的MAC文本转换为16进制的MAC地址

    // char WOLPack[102];魔法包默认长度为102字节
    for (int i = 0; i < 6; i++) //初始化魔法包WOLPack[0]~WOLPack[5]前6个字符为16进制FF 6字节
    {
        WOLPack[i] = 255;
    }

    for (int j = 0; j < 16; j++) // WOLPack[6]~WOLPack[102]在重复填入16次多播的HEXMAC地址  16*6=96字节
    {
        for (int k = 0; k < 6; k++)
        {
            WOLPack[j * 6 + 6 + k] = HEXMAC[k];
        }
    }
}
void StringToHexEnCoder(char to[], const int tolen, const char *from)
{
    char _from[strlen(from) + 1];
    strcpy(_from, from);

    char *result = NULL;
    const char *split = "-";
    result = strtok(_from, split);
    int count = 0;
    while (result != NULL && count < tolen)
    {

        to[count] = strtol(result, &result, 16);
        result = strtok(NULL, split);
        count++;
    }
}

String ThreadOnDevice(const char *devicename)
{
    // ThreadDevice threadtemp = ThreadDevice(devicename);
    ThreadDevice threadtemp(devicename);
    String threadID = threadtemp.ID;
    threadpool[threadID] = threadtemp;
    threadpool[threadID].task(1000, OnDevice, threadpool[threadID].ID.c_str());
    return threadID;
}
String ThreadOffDevice(const char *devicename)
{
    // ThreadDevice threadtemp = ThreadDevice(devicename);
    ThreadDevice threadtemp(devicename);
    String threadID = threadtemp.ID;
    threadpool[threadID] = threadtemp;
    threadpool[threadID].task(1000, OffDevice, threadpool[threadID].ID.c_str());
    return threadID;
}

void BatchOnDevice(const char *groupname)
{
    int itemcount = 0;
    char *items[50] = {0};
    itemcount = getItems(groupname, items, getDeviceJson);
    Serial.println("itemcounttest");
    Serial.printf("itemcount = %d\n", itemcount);
    for (int i = 0; i < itemcount; i++)
    {
        // Serial.printf("items[%d] = %s\n", i, items[i]);
        ThreadOnDevice(items[i]);
        // delay(2);
    }
    freeItems();
}
void BatchOffDevice(const char *groupname)
{
    int itemcount = 0;
    char *items[50] = {0};
    itemcount = getItems(groupname, items, getDeviceJson);
    for (int i = 0; i < itemcount; i++)
    {
        // Serial.printf("devicename = %s\n",items[i]);
        ThreadOffDevice(items[i]);
        // delay(2);
    }
    freeItems();
}

void Play(const char *devicename)
{
    COMMANDINFO commandinfo = getCommandInfo(devicename);
    // int len = (strlen(commandinfo.PlayCode) + 1) / 3;
    int len = (commandinfo.playcode.length() + 1) / 3;
    char EnCoder[len];
    memset(EnCoder, '\0', sizeof(EnCoder));
    StringToHexEnCoder(EnCoder, len, commandinfo.playcode.c_str());
    SendUDP(commandinfo.ip.c_str(), commandinfo.destport, EnCoder, len);
    Serial.printf("Play(%s) was called and playend\n", devicename);
}
void Stop(const char *devicename)
{
    COMMANDINFO commandinfo = getCommandInfo(devicename);
    // int len = (strlen(commandinfo.stopcode) + 1) / 3;
    int len = (commandinfo.stopcode.length() + 1) / 3;
    char EnCoder[len];
    memset(EnCoder, '\0', sizeof(EnCoder));
    StringToHexEnCoder(EnCoder, len, commandinfo.stopcode.c_str());
    SendUDP(commandinfo.ip.c_str(), commandinfo.destport, EnCoder, len);
    Serial.printf("Stop(%s) was called and Stopend\n", devicename);
}
void ResetPlay(const char *devicename)
{
    COMMANDINFO commandinfo = getCommandInfo(devicename);
    // int len = (strlen(commandinfo.stopcode) + 1) / 3;
    int len = (commandinfo.resetplay.length() + 1) / 3;
    char EnCoder[len];
    memset(EnCoder, '\0', sizeof(EnCoder));
    StringToHexEnCoder(EnCoder, len, commandinfo.resetplay.c_str());
    SendUDP(commandinfo.ip.c_str(), commandinfo.destport, EnCoder, len);
    Serial.printf("ResetPlay(%s) was called and ResetPlayend\n", devicename);
}
void SetVolume(const char *devicename, int volume)
{
    String volumestr = "";
    if (volume < 10)
    {
        volumestr = "0" + String(volume);
    }
    else
    {
        volumestr = String(volume);
    }
    COMMANDINFO commandinfo = getCommandInfo(devicename);
    commandinfo.setvolume.replace("AA", volumestr);
    Serial.printf("%s volume=%s\n", devicename, commandinfo.setvolume.c_str());
    SendUDP(commandinfo.ip.c_str(), commandinfo.destport, commandinfo.setvolume.c_str());
    // SendUDP(commandinfo.ip.c_str(), commandinfo.destport, "");
    Serial.printf("SetVolume(%s) was called and SetVolumeend\n", devicename);
}

void BatchStop(const char *groupname)
{
    int itemcount = 0;
    char *items[50] = {0};
    itemcount = getItems(groupname, items, getCommandJson);
    for (int i = 0; i < itemcount; i++)
    {
        // Serial.printf("items[%d] = %s\t", i, items[i]);
        String tempid = ThreadStop(items[i]);
        // Serial.printf("threadid = %s\n", tempid.c_str());
        // delay(2);
    }
    freeItems();
}
void StopCallBack(const char *threadID)
{
    ThreadCommand *thread = &(threadpoolcmd[threadID]);

    if (thread->delaytime > 0)
    {
        Serial.print("stop remaining time = ");
        Serial.println(thread->delaytime);
        thread->delaytime -= 1000;
        return;
    }
    Serial.printf("%s:  ", thread->devicename.c_str());
    Serial.printf("%s   ", thread->commandinfo.ip.c_str());
    Serial.printf("%s   ", thread->commandinfo.stopcode.c_str());
    // int len = (strlen(thread->commandinfo.stopcode) + 1) / 3;
    int len = (thread->commandinfo.stopcode.length() + 1) / 3;
    char EnCoder[len];
    memset(EnCoder, '\0', sizeof(EnCoder));
    StringToHexEnCoder(EnCoder, len, thread->commandinfo.stopcode.c_str());
    SendUDP(thread->commandinfo.ip.c_str(), thread->commandinfo.destport, EnCoder, len);
    Serial.printf("StopCallback(%s) was called and Stopend\n", thread->devicename.c_str());
    threadpoolcmd.erase(thread->ID);
}
String ThreadStop(const char *devicename)
{
    ThreadCommand threadtemp = ThreadCommand(devicename);
    String threadID = threadtemp.ID;
    threadpoolcmd[threadID] = threadtemp;
    threadpoolcmd[threadID].task(1000, StopCallBack, threadpoolcmd[threadID].ID.c_str());
    // threadpoolcmd[threadID].attach(1, StopCallBack, threadpoolcmd[threadID].ID.c_str());
    return threadID;
}
