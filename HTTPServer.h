#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
// #include <ESP8266mDNS.h>
#include <WString.h>
#include "Controller.h"
#include "utf8.h"
#include "DoorControl.h"
// const char *URL = "zgzt"; //局域网网址为zgzt.local
ESP8266WebServer httpserver;
void homepage()
{
    File file = SPIFFS.open("/devicecontrol.html", "r");
    httpserver.streamFile(file, "text/html");
    file.close();
}
void faviconico()
{
    // Serial.println("icon");
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "image/x-icon");
    file.close();
}
void JSday()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "application/javascript");
    file.close();
}
void JSaxios()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "application/javascript");
    file.close();
}

void JSvue()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "application/javascript");
    file.close();
}
void JSdevicecontrol()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "application/javascript");
    file.close();
}
void CSSdevicecontrol()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "text/css");
    file.close();
}

void WiFiConfig()
{
    String name = httpserver.arg("name");
    String password = httpserver.arg("password");
    Serial.println("wifiname = " + name);
    Serial.println("wifipassword = " + password);
    if (name != "")
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(name, password);
        delay(3000);
        httpserver.send(200, "text/plain,text/html", "");
        Serial.println("WiFi信息写入成功");
        ESP.reset();
    }
}

void handleNotFound()
{
    String Address = httpserver.uri();
    Serial.printf("网页不存在%s\n", Address.c_str());
    httpserver.send(404, "text/html;charset=UTF-8", "404 网页不存在");
}

void ExhibitionOn()
{
    // http://zgzt.local/zhanting/on
    httpserver.send(200, "text/html;charset=UTF-8", "展厅设备已打开,success");
    BatchOnDevice("展厅设备");
    // BatchStop("展厅设备");
}
void ExhibitionOff()
{
    // http://zgzt.local/zhanting/off
    httpserver.send(200, "text/html;charset=UTF-8", "展厅设备已关闭,success");
    BatchOffDevice("展厅设备");
}

void menqianOn()
{
    // http://zgzt.local/menqian/on
    httpserver.send(200, "text/html;charset=UTF-8", "门前设备已打开,success");
    BatchOnDevice("前门设备");
}
void menqianOff()
{
    // http://zgzt.local/menqian/off
    httpserver.send(200, "text/html;charset=UTF-8", "门前设备已关闭,success");
    BatchOffDevice("前门设备");
}

void rendaOn()
{
    // http://zgzt.local/renda/on
    httpserver.send(200, "text/html;charset=UTF-8", "人大设备已打开,success");
    BatchOnDevice("人大设备");
}
void rendaOff()
{
    // http://zgzt.local/renda/off

    httpserver.send(200, "text/html;charset=UTF-8", "人大设备已关闭,success");
    BatchOffDevice("人大设备");
}

void biaoti()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "text/html");
    file.close();
}

unsigned char gb2312buf[128];
int len = 0;
void caption(String arg, int port)
{
    String content = httpserver.arg(arg);
    // Serial.printf("content = %s\n", content.c_str());
    content.replace("开", "寗"); // 开gbk:{191,170}寗gkb:{140,132}
    len = u8f_gb2312((unsigned char *)(content.c_str()), gb2312buf);
    for (int i = 0; i < len; i += 2)
    {
        // 开gbk:{191,170}寗gkb:{140,132}
        if (gb2312buf[i] == 140 && gb2312buf[i + 1] == 132)
        {
            gb2312buf[i] = 191;
            gb2312buf[i + 1] = 170;
        }
    }
    httpserver.send(200, "text/html", "");
    SendUDP("192.168.10.90", port, (char *)gb2312buf, len);
}
void caption01()
{
    // http://zgzt.local/biaoticaption01.html
    caption("caption01", 8888);
}
void caption02()
{
    // http://zgzt.local/biaoticaption02.html
    caption("caption02", 9999);
}

void rendahuiyishion()
{
    ThreadOnDevice("人大会议室");
    httpserver.send(200, "text/html;charset=UTF-8", "人大会议室已打开,success");
}
void rendahuiyishioff()
{
    ThreadOffDevice("人大会议室");
    httpserver.send(200, "text/html;charset=UTF-8", "人大会议室已关闭,success");
}

void toumingpingOn()
{
    // 创建 WiFiClient 实例化对象
    WiFiClient client;

    // 创建http对象
    HTTPClient httpclient;
    httpclient.begin(client, "http://192.168.10.117/api/action");
    httpclient.POST("{\"command\":\"wakeup\"}");
    httpclient.end();

    httpserver.send(200, "text/html;charset=UTF-8", "透明屏打开,success");
}
void toumingpingOff()
{
    // 创建 WiFiClient 实例化对象
    WiFiClient client;

    // 创建http对象
    HTTPClient httpclient;
    httpclient.begin(client, "http://192.168.10.117/api/action");
    httpclient.POST("{\"command\":\"sleep\"}");
    httpclient.end();
    httpserver.send(200, "text/html;charset=UTF-8", "透明屏关闭,success");
}

void xutingPlay()
{
    Play("习近平思想");
    char leddata[4] = {0xAA, 0xA1, 0x03, 0xBB}; // 灯光比较暗
    SendUDP("192.168.10.205", 3000, leddata, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "习近平思想设备播放,success");
}
void xutingStop()
{
    Stop("习近平思想");
    char leddata[4] = {0xAA, 0xA1, 0x01, 0xBB};
    SendUDP("192.168.10.205", 3000, leddata, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "习近平思想停止,success");
}
void xutingResetPlay()
{
    ResetPlay("习近平思想");
    char leddata[4] = {0xAA, 0xA1, 0x03, 0xBB};
    SendUDP("192.168.10.205", 3000, leddata, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "习近平思想重播,success");
}
void xutingSetVolume()
{
    String volumestr = httpserver.arg("volume");
    int volume = atoi(volumestr.c_str());
    SetVolume("习近平思想", volume);
    httpserver.send(200, "text/html;charset=UTF-8", "习近平思想声音,success");
}

void shapanPlay()
{
    Play("沙盘投影1");
    Play("沙盘投影2");
    // AA A3 02 BB
    char leddata[4] = {0xAA, 0xA3, 0x02, 0xBB};
    // char leddata1[4] = {0xAA, 0xA3, 0x05, 0xBB};
    SendUDP("192.168.10.205", 3000, leddata, 4);
    // delay(50);
    // SendUDP("192.168.10.205", 3000, leddata1, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "沙盘投影播放,success");
}
void shapanStop()
{
    Stop("沙盘投影1");
    Stop("沙盘投影2");
    // AA A3 01 BB
    char leddata[4] = {0xAA, 0xA3, 0x01, 0xBB};
    SendUDP("192.168.10.205", 3000, leddata, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "沙盘投影停止,success");
}
void shapanResetPlay()
{
    ResetPlay("沙盘投影1");
    ResetPlay("沙盘投影2");
    // AA A3 02 BB
    char leddata[4] = {0xAA, 0xA3, 0x02, 0xBB};
    // char leddata1[4] = {0xAA, 0xA3, 0x05, 0xBB};
    SendUDP("192.168.10.205", 3000, leddata, 4);
    // delay(50);
    // SendUDP("192.168.10.205", 3000, leddata1, 4);
    httpserver.send(200, "text/html;charset=UTF-8", "沙盘投影重播,success");
}
void shapanSetVolume()
{
    String volumestr = httpserver.arg("volume");
    int volume = atoi(volumestr.c_str());
    // Serila.println(volume);
    SetVolume("沙盘投影1", volume);
    SetVolume("沙盘投影2", volume);
    httpserver.send(200, "text/html;charset=UTF-8", "沙盘投影声音,success");
}

void politicsHome()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_Wait");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅主页,success");
}
void politicsTakePhotoMode()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_Canon");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅拍照模式,success");
}
void politicsTakePhoto()
{
    SendUDP("192.168.10.21", 8888, "TakePhoto");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅拍照,success");
}

void politicsWordOfChange()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_Shici_Yanbian");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅誓词演变,success");
}
void politicsWordOfParty()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_Rudang_Shici");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅入党誓词,success");
}
void politicsRightsAndObligation()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_Quanli_Yiwu");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅权力与义务,success");
}

void politicsSong()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_HongGe");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅爱国歌曲,success");
}
void politicsPrevious()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_HongGe_Pre");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅上一首,success");
}
void politicsNext()
{
    SendUDP("192.168.10.21", 8888, "ShowMode_HongGe_Next");
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅下一首,success");
}
void politicsSetVolume()
{
    String volume = httpserver.arg("volume");
    String volumedata = "volume%00AA#";
    if (volume.length() < 2)
    {
        volume = "00";
    }
    volumedata.replace("AA", volume);
    SendUDP("192.168.10.21", 22311, volumedata.c_str());
    httpserver.send(200, "text/html;charset=UTF-8", "政治生活厅声音,success");
}

void tvScreen()
{
    String name = httpserver.arg("name");
    ThreadOnDevice(name.c_str());
    httpserver.send(200, "text/html;charset=UTF-8", "电视屏幕,success");
}

void Light()
{
    String name = httpserver.arg("name");
    // Serial.print("name = ");
    // Serial.println(name);
    if (name == "on")
    {
        ThreadOnDevice("展厅灯");
    }
    else if (name == "off")
    {
        ThreadOffDevice("展厅灯");
    }
    httpserver.send(200, "text/html;charset=UTF-8", "展厅灯,success");
}
void Light7()
{
    String name = httpserver.arg("name");
    // Serial.print("name = ");
    // Serial.println(name);
    if (name == "on")
    {
        ThreadOnDevice("区域7");
        httpserver.send(200, "text/html;charset=UTF-8", "区域7开灯,success");
    }
    else if (name == "off")
    {
        ThreadOffDevice("区域7");
        httpserver.send(200, "text/html;charset=UTF-8", "区域7关灯,success");
    }
}
void Light71()
{
    String name = httpserver.arg("name");
    // Serial.print("name = ");
    // Serial.println(name);
    if (name == "light")
    {
        ThreadOnDevice("区域71");
        httpserver.send(200, "text/html;charset=UTF-8", "区域7适中,success");
    }
    else if (name == "dark")
    {
        ThreadOffDevice("区域71");
        httpserver.send(200, "text/html;charset=UTF-8", "区域7较暗,success");
    }
}

void remainCounts()
{
    File regfile = SPIFFS.open("/reg.txt", "r");
    String remaincounts = regfile.readString();
    // Serial.println(remaincounts);
    regfile.close();
    httpserver.send(200, "text/html;charset=UTF-8", remaincounts.c_str());
}

void getnumber()
{
    // Serial.println(httpserver.uri());
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "text/html");
    file.close();
}
void detailnumber()
{
    String num = httpserver.arg("num");
    httpserver.send(200, "text/html", "");
    SendUDP("192.168.10.229", 19731, num.c_str());
    // SendUDP("192.168.10.227", 19731, num.c_str());
}

void doorcontrol()
{
    File file = SPIFFS.open(httpserver.uri(), "r");
    httpserver.streamFile(file, "text/html");
    file.close();
}
void buttontype()
{
    String type = httpserver.arg("type");
    String name = httpserver.arg("name");
    DoorControl dc;
    if (type == "a")
    {
        dc.alllock();
    }
    if (type == "b")
    {
        dc.singleopen();
    }
    if (type == "c1")
    {
        dc.clickopen();
    }
    if (type == "c2")
    {
        dc.normallyopen(1500);
    }
    if (type == "d")
    {
        dc.clickopen();
    }
    httpserver.send(200, "text/html", name);
}

void loadWebSite()
{
    httpserver.on("/", homepage);
    httpserver.on("/favicon.ico", faviconico);
    httpserver.on("/js/axios.min.js", JSaxios);
    httpserver.on("/js/vue.min.js", JSvue);
    httpserver.on("/js/dayjs.min.js", JSday);
    httpserver.on("/devicecontrol.js", JSdevicecontrol);
    httpserver.on("/devicecontrol.css", CSSdevicecontrol);
    httpserver.on("/WiFiConfig.html", WiFiConfig);

    httpserver.on("/biaoti.html", biaoti);
    httpserver.on("/biaoticaption01.html", caption01);
    httpserver.on("/biaoticaption02.html", caption02);

    httpserver.on("/rendahuiyishi/on.html", rendahuiyishion);
    httpserver.on("/rendahuiyishi/off.html", rendahuiyishioff);

    httpserver.on("/menqian/on", menqianOn);
    httpserver.on("/menqian/off", menqianOff);

    httpserver.on("/renda/on", rendaOn);
    httpserver.on("/renda/off", rendaOff);

    httpserver.on("/zhanting/on", ExhibitionOn);
    httpserver.on("/zhanting/off", ExhibitionOff);

    httpserver.on("/toumingping/on", toumingpingOn);
    httpserver.on("/toumingping/off", toumingpingOff);

    httpserver.on("/xuting/play", xutingPlay);
    httpserver.on("/xuting/stop", xutingStop);
    httpserver.on("/xuting/resetplay", xutingResetPlay);
    httpserver.on("/xuting/setvolume", xutingSetVolume);

    httpserver.on("/shapan/play", shapanPlay);
    httpserver.on("/shapan/stop", shapanStop);
    httpserver.on("/shapan/resetplay", shapanResetPlay);
    httpserver.on("/shapan/setvolume", shapanSetVolume);

    httpserver.on("/politics/home", politicsHome);
    httpserver.on("/politics/takephotomode", politicsTakePhotoMode);
    httpserver.on("/politics/takephoto", politicsTakePhoto);

    httpserver.on("/politics/wordofchange", politicsWordOfChange);
    httpserver.on("/politics/wordofparty", politicsWordOfParty);
    httpserver.on("/politics/rightsandobligation", politicsRightsAndObligation);

    httpserver.on("/politics/song", politicsSong);
    httpserver.on("/politics/previous", politicsPrevious);
    httpserver.on("/politics/next", politicsNext);
    httpserver.on("/politics/setvolume", politicsSetVolume);

    httpserver.on("/tv/screen", tvScreen);

    httpserver.on("/light", Light);
    httpserver.on("/light7", Light7);
    httpserver.on("/light71", Light71);

    httpserver.on("/remaincounts", remainCounts);

    httpserver.on("/getnumber.html", getnumber);
    httpserver.on("/detailnumber.html", detailnumber);

    httpserver.on("/doorcontrol.html", doorcontrol);
    httpserver.on("/buttontype.html", buttontype);

    httpserver.onNotFound(handleNotFound);
    httpserver.begin();
    // if (MDNS.begin(URL))
    // { // Start mDNS with name esp8266
    //     Serial.println("MDNS started");
    // }
    // else
    // {
    //     Serial.println("MDNS error!");
    // }
    // MDNS.addService("http", "tcp", 80);
    Serial.println("Website loaded finished!");
}
#endif
