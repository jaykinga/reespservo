#ifndef _DEVICEINFO_H_
#define _DEVICEINFO_H_
#include <WString.h>
#include <FS.h>
//屏幕设备相关操作信息
class SCREENINFO
{
public:
    String ip;      //屏幕的IP
    int desport;    //屏幕的端口
    String oncode;  //屏幕的开机代码
    String offcode; //屏幕的关机代码
};

//电脑设备的信息系
class PCINFO
{
public:
    String ip;      //主机的IP
    int desport;    //目标主句的端口
    String mac;     //目标主机的，用于MAC地址网络唤醒
    String offcode; //目标主机的关机代码
};
//要操作设备的信息里面含有刚定义的SCREENINF and PCINFO两个数据类型的组
// class DEVICEINFO
// {
// public:
//     int screens;              // 这个设备的屏幕数量
//     SCREENINFO screeninfo[2]; //每个屏幕的具体操作参数

//     int pcs;          //这个设备的主机数量
//     PCINFO pcinfo[9]; //每个主句的操作参数

//     int ondelaytime;  //开机时延时打开屏幕的时间
//     int offdelaytime; //关机时延时关闭屏幕的时间
// };

class DEVICEINFO
{
public:
    int screens;            // 这个设备的屏幕数量
    SCREENINFO *screeninfo; //每个屏幕的具体操作参数

    int pcs;        //这个设备的主机数量
    PCINFO *pcinfo; //每个主句的操作参数

    int ondelaytime;  //开机时延时打开屏幕的时间
    int offdelaytime; //关机时延时关闭屏幕的时间
public:
    DEVICEINFO()
    {
        this->screens = 0;
        this->pcs = 0;
        this->screeninfo = NULL;
        this->pcinfo = NULL;
        this->ondelaytime = 0;
        this->offdelaytime = 0;
    }
    DEVICEINFO(int screens, int pcs)
    {
        this->screens = screens;
        this->pcs = pcs;
        this->screeninfo = new SCREENINFO[screens];
        this->pcinfo = new PCINFO[pcs];
        this->ondelaytime = 0;
        this->offdelaytime = 0;
    }
    DEVICEINFO &operator=(const DEVICEINFO &obj)
    {
        // Serial.println("==constructor");
        this->screens = obj.screens;
        this->pcs = obj.pcs;
        this->ondelaytime = obj.ondelaytime;
        this->offdelaytime = obj.offdelaytime;
        this->screeninfo = new SCREENINFO[this->screens];
        this->pcinfo = new PCINFO[this->pcs];
        for (int screenindex = 0; screenindex < this->screens; screenindex++)
        {
            this->screeninfo[screenindex].desport = obj.screeninfo[screenindex].desport;
            this->screeninfo[screenindex].ip = obj.screeninfo[screenindex].ip;
            this->screeninfo[screenindex].oncode = obj.screeninfo[screenindex].oncode;
            this->screeninfo[screenindex].offcode = obj.screeninfo[screenindex].offcode;
        }
        for (int pcindex = 0; pcindex < this->pcs; pcindex++)
        {
            this->pcinfo[pcindex].desport = obj.pcinfo[pcindex].desport;
            this->pcinfo[pcindex].ip = obj.pcinfo[pcindex].ip;
            this->pcinfo[pcindex].mac = obj.pcinfo[pcindex].mac;
            this->pcinfo[pcindex].offcode = obj.pcinfo[pcindex].offcode;
        }
        return *this;
    }
    ~DEVICEINFO()
    {
        delete[] screeninfo;
        delete[] pcinfo;
        screeninfo = NULL;
        pcinfo = NULL;
    }
};
#endif
