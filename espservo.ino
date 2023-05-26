#include <ESP8266WiFi.h>
#include "HTTPServer.h"
#include "Controller.h"
#include "Mission.h"
#include "DoorControl.h"
void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.printf("\nSerial begined!\n");
  SPIFFS.begin();
  Serial.printf("SPIFFS begined!\n");
  Serial.println(WiFi.macAddress());
  // 检查文件系统是否上传了相关数据
  if (!(SPIFFS.exists("/reg.txt")))
  {
    Serial.printf("=======================================================");
    Serial.printf("=======================================================");
    Serial.println("register-file is not exists, system will be stopped!");
    Serial.println("please register app! more information call 18028551016");
    Serial.printf("=======================================================");
    return;
  }
  else
  {
    File regfile = SPIFFS.open("/reg.txt", "r");
    String temptxt = regfile.readString();
    int remaincounts = atoi(temptxt.c_str()) - 1;
    Serial.printf("remaincounts = %d\n", remaincounts);
    regfile.close();
    regfile = SPIFFS.open("/reg.txt", "w");
    regfile.println(String(remaincounts).c_str());
    regfile.close();
    if (remaincounts <= 0)
    {
      Serial.printf("第二次提交");
      Serial.printf("============================================");
      Serial.println("Times of use is zero,please register again!");
      Serial.printf("============================================");
      return;
    }
  }
  if (SPIFFS.exists("/biaoti.html"))
  {
    Serial.println("init data directory success");
  }
  else
  {
    Serial.println("data directory is not exists!");
  }
  connectwifi();
  if (WiFi.isConnected()) // WIFI链接成功
  {
    loadWebSite();                // 加载http网站服务器
    pinMode(LED_BUILTIN, OUTPUT); // 初始化内置的led等为wifi链接成功的指示灯，亮2秒，灭2秒
    static Ticker TTripLight;
    TTripLight.attach(2, []()
                      { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); });
    configTime(28800, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com"); // 设置系统时间
    TUpdateTime.attach(3, getTimeStatus);                                      // 每隔3秒检查系统时间是否更新完毕
    TListenMisson.attach(60, getMission);                                      // 挂载每日任务
  }
}

void loop()
{
  handleMission();         // 处理每日任务
  SystemCommandListener(); // 监听com口是否有命令要执行
  // MDNS.update();
  httpserver.handleClient();
  // 自动重连wifi
  if (WiFi.isConnected() == false)
  {
    connectwifi();
  }
}

void connectwifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("ZGZT", "zhengguo2019");
  // IPAddress IP(192, 168, 10, 226);
  // IPAddress gw(192, 168, 10, 1);
  // IPAddress sb(255, 255, 255, 0);
  // IPAddress dns(192, 168, 10, 1);
  // WiFi.config(IP, gw, sb, dns);
  // WiFi.begin("ZGZT", "zhengguo2019");
  Serial.printf("Begin to connect Wifi= %s\tPSW = %s.\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
  // 连接40次wifi，连接成功继续WIFI_STA模式，不成功打开WIFI_AP_STA模式
  for (int i = 1; i <= 40; i++)
  {
    Serial.printf("the %d time connect ", i);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("failed.");
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
    else
    {
      Serial.println("success.");
      Serial.printf("IP = %s\n", WiFi.localIP().toString().c_str());
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi链接失败,系统即将重启");
    ESP.reset();
  }
}
void SystemCommandListener()
{
  if (Serial.available())
  {
    String serialData = Serial.readString();                         // 将接收到的信息使用readString()存储于serialData变量
    Serial.printf("Received Serial Data: %s\n", serialData.c_str()); // 然后通过串口监视器输出serialData变量内容
    
    
    if (serialData == "1")
    {
      DoorControl dc;
      dc.normallyopen(1500);
      // char ledcode[4] = {0};
      // StringToHexEnCoder(ledcode, 4, "AA-A1-03-BB");
      // SendUDP("192.168.10.205", 3000, ledcode, 4);
      // Serial.println("AA-A1-03-BB");
      return;
    }
    if (serialData == "reset")
    {
      Serial.println("System will be reset.");
      ESP.reset();
      return;
    }
    if (serialData == "resetwifi")
    {
//      WiFiManager().resetSettings();
//      Serial.println("Wifi information had been reset!");
//      return;
    }
    if (serialData.indexOf("wifiset/") > -1)
    {
      serialData.replace("wifiset/", "");
      String wifiname = serialData.substring(0, serialData.indexOf("/"));
      String password = serialData.substring(serialData.indexOf("/") + 1);
      // WiFi.begin(wifiname, password);
      struct station_config config;
      strcpy(reinterpret_cast<char *>(config.ssid), wifiname.c_str());
      strcpy(reinterpret_cast<char *>(config.password), password.c_str());
      wifi_station_set_config(&config);
      Serial.println("WIFI set success,system will be reset.");
      delay(3000);
      ESP.restart();
      return;
    }
    if (serialData == "IP")
    {
      Serial.printf("IP = %s\n", WiFi.localIP().toString().c_str());
      return;
    }
    else
    {
      Serial.println("No this command!");
    }
  }
}
