#ifndef _THEADONDEVICE_H_
#define _THEADONDEVICE_H_

#include "Controller.h"
#include "DeviceInfo.h"
#include "ritos.h"
#include <FS.h>
class ThreadDevice : public Ritos
{
public:
    String ID = "";
    String devicename = "";
    /*线程状体标识说明 一共有四个标记为 为 二进制 0b0000，
      1.左边第一位为线程为初始化         失败为0, 成功为1，
      2.左边第二位为PC开机状态位         完成动作为1，
      3.左边第三位为SCREEN屏幕状态位     完成动作为1，
      3.左边第四位为线程完成态位        进行中为0，完成为1。
      */
    int ThreadState; //记录线程的运行状态
    DEVICEINFO deviceinfo;
    int ondelaytime = 0;
    int offdelaytime = 0;

public:
    ThreadDevice() {}
    ThreadDevice(const char *devicename) //:  devicename(_devicename),deviceinfo(getDeviceInfo(_devicename))
    {
        ID = millis();
        this->devicename = devicename;
        deviceinfo = getDeviceInfo(devicename);
        ondelaytime = deviceinfo.ondelaytime;
        offdelaytime = deviceinfo.offdelaytime;
        ThreadState = 0b1000;
    }
    ~ThreadDevice()
    {
        detach();
    }
};
#endif // !_THEADONDEVICECLASS_H_#define