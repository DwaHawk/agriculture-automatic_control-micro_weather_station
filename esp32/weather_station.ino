/* 
 weatherMainApp v.1
 Copyright (C) 2025, David Huang

 for devkit v4
 * pin info:
 * 21:SDA
 * 22:SCL
 * serial1:16 17
 * serial2:18 19
 * DHT digital:2
 * wind analog:39
四支土壤感測器 雨水回報四種變數 土壤感測器沒收到數據回報999
O 1 氣壓  BMP-180 or MPL3115A2
O 1 照度  GY-320 BH1750 
O 1 溫溼度 DHT 22    
O 4 土壤溫濕電導度 JXBS-3001-EC    
O 1 風向      
O 1 風速  修改成10s紀錄20次取平均    
O 1 雨量
*/
#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <BH1750.h>
#include "DHT.h"
#include <Adafruit_BMP085.h>//........>BMP180<
#include <WiFi.h>
#include <HTTPClient.h>
#include "rg15arduino.h"
//#include <Adafruit_MPL3115A2.h>//MPL3115A2

//---------------------------------------------------------
//wifi設定
char ssid[] = "your_ssid";
char password[] = "your_pass_word";
//google script seeting
String GOOGLE_SCRIPT_ID = "your_google_script_id";
int inf = 4;//不同google sheet table:school
//---------------------------------------------------------

//腳位設定

BH1750 lightMeter;///////////////////////////////light sensor
#define DHTPIN 2     
#define DHTTYPE DHT22   ///////////////////////// DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;/////////////////////////////BMP180
HardwareSerial mySerial1(1); ////////////////////RS485
HardwareSerial mySerial2(2); ////////////////////RS485? rain
//Adafruit_MPL3115A2 baro;//MPL3115A2
RG15Arduino rg15;
#define windSpeedPin 39//////////////////////////analog wind speed pin
//---------------------------------------------------------


unsigned char soil1[8] = {
    0x17, 0x03, 0x00, 0x12, 0x00, 0x02, 0x66, 0xF8}; //23: soil sensor temp and humidity 1 setting
unsigned char soil2[8] = {
    0x18, 0x03, 0x00, 0x12, 0x00, 0x02, 0x66, 0x07}; //24
unsigned char soil3[8] = {
    0x19, 0x03, 0x00, 0x12, 0x00, 0x02, 0x67, 0xD6}; //25
unsigned char soil4[8] = {
    0x1A, 0x03, 0x00, 0x12, 0x00, 0x02, 0x67, 0xE5}; //26

unsigned char conductivity1[8] = {
    0x17, 0x03, 0x00, 0x15, 0x00, 0x01, 0x97, 0x38};
unsigned char conductivity2[8] = {
    0x18, 0x03, 0x00, 0x15, 0x00, 0x01, 0x97, 0xC7};
unsigned char conductivity3[8] = {
    0x19, 0x03, 0x00, 0x15, 0x00, 0x01, 0x96, 0x16};
unsigned char conductivity4[8] = {
    0x1A, 0x03, 0x00, 0x15, 0x00, 0x01, 0x96, 0x25};


unsigned char windDirection[8] = {
    0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};


//wind speed setting
const int windpackage = 1;  // 封包數
const int numReadings = 20*windpackage;  // 收集數據次數
const int windwait = 500;  // 風速等待毫秒
//init
int readings[numReadings];
int readingIndex = 0;
int total = 0;
float average = 0.0;

//整個程序等待時間 unit:10s
int delayTime = 30 - (numReadings*windwait/10000);
//___________________________
void setup()
{
    Serial.begin(9600);
    //////////////////////////////////////////////////////////////
    //////////////////////// wifisetting /////////////////////////
    //////////////////////////////////////////////////////////////
    Serial.print("開始連線到無線網路SSID:");
    Serial.println(ssid);
    Wire.begin();
    WiFi.mode(WIFI_STA);//設定為wifi連線終端
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(2000);
    }
    //////////////////////////////////////////////////////////////
    Serial.println(F("wifi無線網路連線完成!"));
    Serial.println(F("rs485、光度、溫濕度、氣壓、雨量感測器啟動中:"));
    Serial.println(F("1. rs485 start"));                             
    mySerial1.begin(9600, SERIAL_8N1, 18, 19);////////////////////serial1
    //////////////////////////////////////////////////////////////
    //////////////////////// light init //////////////////////////
    //////////////////////////////////////////////////////////////
    lightMeter.begin();
    Serial.println(F("2. BH1750 (light) start"));
    //////////////////////////////////////////////////////////////
    /////////////////////////// DHT //////////////////////////////
    //////////////////////////////////////////////////////////////
    dht.begin();
    Serial.println(F("3. DHT22 (temperture) start"));
    //////////////////////////////////////////////////////////////
    ////////////////////// Pressure //////////////////////////////
    //////////////////////////////////////////////////////////////
    if (!bmp.begin()) {        ///////////////////////////////////BMP180
        Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    }
    Serial.println(F("4. pressure sensor start"));
    //if (!baro.begin()) {
    //  Serial.println("Could not find PRESSURE sensor.");
    //}
    //////////////////////////////////////////////////////////////
    ////////////////////// Rain init /////////////////////////////
    //////////////////////////////////////////////////////////////
    mySerial2.begin(9600, SERIAL_8N1, 16, 17);/////////////////////serial2---rain
    Serial.println(F("5. rain sensor start"));
    rg15.setStream(&mySerial2);
    ///////////////////////////////////////////////////////////////windspeed
    pinMode(windSpeedPin, INPUT);
    Serial.println(F("........................................................"));
    Serial.println();
    Serial.println();
}

void loop()
{
    double* sensor_data[12];
    char* str[12];
    double *winD;
    float lux = lightMeter.readLightLevel();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float bmpT;
    float bmpP;
    String rain;
    String rain1;
    String rain2;
    String rain3;
    bmpT = bmp.readTemperature();
    bmpP = bmp.readPressure();


    for (int i = 0; i < 12; i++) {
        str[i] = new char[8];
    }
    sensor_data[0] = readSoilData1(1);
    sensor_data[1] = sensor_data[0] + 1;
    sensor_data[2] = sensor_data[0] + 2;
    dtostrf(*sensor_data[0], 3, 2, str[0]);
    dtostrf(*sensor_data[1], 3, 2, str[1]);
    dtostrf(*sensor_data[2], 3, 2, str[2]);

    sensor_data[3] = readSoilData1(2);
    sensor_data[4] = sensor_data[3] + 1;
    sensor_data[5] = sensor_data[3] + 2;
    dtostrf(*sensor_data[3], 3, 2, str[3]);
    dtostrf(*sensor_data[4], 3, 2, str[4]);
    dtostrf(*sensor_data[5], 3, 2, str[5]);

    sensor_data[6] = readSoilData1(3);
    sensor_data[7] = sensor_data[6] + 1;
    sensor_data[8] = sensor_data[6] + 2;
    dtostrf(*sensor_data[6], 3, 2, str[6]);
    dtostrf(*sensor_data[7], 3, 2, str[7]);
    dtostrf(*sensor_data[8], 3, 2, str[8]);

    sensor_data[9] = readSoilData1(4);
    sensor_data[10] = sensor_data[9] + 1;
    sensor_data[11] = sensor_data[9] + 2;
    dtostrf(*sensor_data[9], 3, 2, str[9]);
    dtostrf(*sensor_data[10], 3, 2, str[10]);
    dtostrf(*sensor_data[11], 3, 2, str[11]);

    winD = readwinDData();

    Serial.print(F("WindDirection: "));
    Serial.println(*winD);
    Serial.print(F("Light: "));
    Serial.print(lux);
    Serial.println(" lx");
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F(" %t"));
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.println(F(" *C "));
    Serial.print(F("BMP Temperature = "));
    Serial.print(bmpT);
    Serial.println(F(" *C"));
    Serial.print(F("BMP Pressure = "));
    Serial.print(bmpP);
    Serial.println(F(" Pa"));
    delay(200);
    //////////////////////////////////////////////////////////////
    ////////////////////////windspeed/////////////////////////////
    //////////////////////////////////////////////////////////////
    Serial.println(F("wind speed reading..."));
    for (int i = 0; i < numReadings; i++) {// 收集数据
        readings[i] = analogRead(windSpeedPin);
        delay(windwait);
    }

    // calculate the average
    total = 0;
    for (int i = 0; i < numReadings; i++) {
        total += readings[i];
    }
    average = total / (float)numReadings;

    // convert to voltage and level
    float outvoltage = average * (5.0 / 1023.0);
    int level = 6 * outvoltage;

    // 風速結果
    Serial.print(F("Average outvoltage = "));Serial.print(outvoltage);Serial.println(F("V"));
    Serial.print(F("Average wind speed is "));Serial.print(level);Serial.println(F(" level now"));
    Serial.println();

    //////////////////////////////////////////////////////////////
    delay(200);

    //////////////////////////////////////////////////////////////
    //////////////////////////raining/////////////////////////////
    //////////////////////////////////////////////////////////////
    Serial.println(F("Rain reading..."));
    if(rg15.poll()) {//
            Serial.print(F("Accumulation: "));
            Serial.print(rg15.acc,3);
            Serial.print(rg15.unit);
            Serial.print(F(", Event Accumulation: "));
            Serial.print(rg15.eventAcc,3);
            Serial.print(rg15.unit);
            Serial.print(F(", Total Accumulation: "));
            Serial.print(rg15.totalAcc,3);          
            Serial.print(rg15.unit);
            Serial.print(F(", IPH: "));
            Serial.println(rg15.rInt,3);
            rain=rg15.acc;
            rain1=rg15.eventAcc;
            rain2=rg15.totalAcc;
            rain3=rg15.rInt;
    } else {
            Serial.println("Rain Timeout!");
            rain="NA";
            rain1="NA";
            rain2="NA";
            rain3="NA";
    }
    //////////////////////////////////////////////////////////////


    //_________________________________________________
    //-------------------------------------------------
    // upload data: wifi start
    int WifiTry=0;
    while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if(WifiTry++ >= 20){
        WiFi.disconnect();
        WiFi.reconnect();
    }
    }
    WifiTry=0;
    while(WiFi.status() != WL_CONNECTED) {
        if(WifiTry++ >= 20){
          Serial.println("連線失敗，將重啟連線");
          WiFi.disconnect();
          delay(5000);
          return;
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("啟動網頁連線...........");
        HTTPClient http;
        String url1 = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"
        +"information=" + String(inf)
        +"&si1t=" + String(str[0])
        +"&si1h=" + String(str[1])
        +"&si1c=" + String(str[2])
        +"&si2t=" + String(str[3])
        +"&si2h=" + String(str[4])
        +"&si2c=" + String(str[5])
        +"&si3t=" + String(str[6])
        +"&si3h=" + String(str[7])
        +"&si3c=" + String(str[8])
        +"&si4t=" + String(str[9])
        +"&si4h=" + String(str[10])
        +"&si4c=" + String(str[11])
        +"&wDi=" + String(*winD)
        +"&li=" + String(lux)
        +"&ti=" + String(t)
        +"&hi=" + String(h)
        +"&pi=" + String(bmpP)
        +"&wVi=" + String(outvoltage)
        +"&rain=" + rain
        +"&rain1=" + rain1
        +"&rain2=" + rain2
        +"&rain3=" + rain3;
        Serial.println((String) url1);

        //http client取得網頁內容
        http.begin(url1.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.print("網頁內容=");
            Serial.println(payload);
        } else {
            Serial.println("網路傳送失敗");
        }
        http.end();
    }


    //                                  upload wifi end
    //-------------------------------------------------
    //_________________________________________________

    for (int i = 0; i < 12; i++) {
        delete[] str[i];
        str[i] = nullptr;
    }
    //delay time
    for(int i=0;i< delayTime;i=i+1){
        delay(10000);//休息10秒
    }
}

//////////////////////////////////////////////////////////////
//////////////////////////  wind  ////////////////////////////
//////////////////////////////////////////////////////////////
double* readwinDData(){ 
  static double linshi_d[1];
  String dat = "";
  String info[7];
  int arraySize = sizeof(info) / sizeof(info[0]);
  Serial.println(F("風向迴圈開始....................."));
  Serial.print("send wind location ask:   ");
  for (int i = 0 ; i < sizeof(windDirection); i++) {
    Serial.print(windDirection[i]);
    Serial.print(',');
    mySerial1.write(windDirection[i]);
  }
  Serial.println(F("等待數據返回"));
  delay(100);
  dat = "";
  while (mySerial1.available()) {
    unsigned char in = (unsigned char)mySerial1.read();
    dat += in;
    dat += ',';
  }
  if (dat.length() > 0) {
     processData(dat, info, arraySize);
     linshi_d[0] =(info[3].toInt() * 256 + info[4].toInt())/10.0;
     Serial.print(F("TH返回:"));Serial.println(String(linshi_d[0]));
  }else{
     Serial.println(F("風向返回空值"));
     linshi_d[0] = 999;
  }
  Serial.println(F("風向迴圈結束..........................."));
  Serial.println(F("."));
  Serial.println(F("."));
  return linshi_d;
}
//////////////////////////////////////////////////////////////
//////////////////////////  soil  ////////////////////////////
//////////////////////////////////////////////////////////////
double* readSoilData1(int x) { 
  static double linshi_d[3];
  String dat = "";
  String info[9];
  int arraySize = sizeof(info) / sizeof(info[0]);
  unsigned char item[8];
  unsigned char item2[8];
  
  if (x == 1) {
    memcpy(item, soil1, sizeof(soil1));
    memcpy(item2, conductivity1, sizeof(conductivity1));
  } else if (x == 2) {
    memcpy(item, soil2, sizeof(soil2));
    memcpy(item2, conductivity2, sizeof(conductivity2));
  } else if (x == 3) {
    memcpy(item, soil3, sizeof(soil3));
    memcpy(item2, conductivity3, sizeof(conductivity3));
  }else if (x == 4) {
    memcpy(item, soil4, sizeof(soil4));
    memcpy(item2, conductivity4, sizeof(conductivity4));
  }
  Serial.println("soil loop all data output start: ____________" + String(x));
  Serial.print(F("詢問 "));Serial.print(x);Serial.print(F(" 號土壤感測器溫濕度..."));
  Serial.println();
  for (int i = 0; i < sizeof(item); i++) {
    // 發送指令
    Serial.print(item[i]);
    Serial.print(',');
    mySerial1.write(item[i]);
  }
  Serial.println();
  Serial.println(F("等待數據返回"));
  delay(100);
  while (mySerial1.available()) {
    unsigned char in = (unsigned char)mySerial1.read();
    dat += in;
    dat += ',';
  }
  if (dat.length() > 0) {
    processData(dat, info, arraySize);//解碼info
    linshi_d[0] = (info[3].toInt() * 256 + info[4].toInt()) / 10.0;
    linshi_d[1] = (info[5].toInt() * 256 + info[6].toInt()) / 10.0;
    Serial.print(F("TH返回: "));
    Serial.println(String(linshi_d[0]) + ", " + String(linshi_d[1]));
  }else{
    linshi_d[0] = 999;
    linshi_d[1] = 999;
    Serial.println(F("TH返回空值"));
  }
  Serial.print(F("詢問 "));Serial.print(x);Serial.print(F(" 號土壤感測器電導度..."));
  Serial.println();  
  for (int i = 0; i < sizeof(item2); i++) {
    Serial.print(item2[i]);
    Serial.print(',');
    mySerial1.write(item2[i]);
  }
  Serial.println();
  Serial.println(F("等待數據返回"));
  delay(100);
  dat = "";
  while (mySerial1.available()) {
    unsigned char in = (unsigned char)mySerial1.read();
    dat += in;
    dat += ',';
  }
  if (dat.length() > 0) {
    processData(dat, info, arraySize);
    linshi_d[2] = (info[3].toInt() * 256 + info[4].toInt()) / 10.0;
    Serial.print(F("電導返回:"));Serial.println(String(linshi_d[2]));
  }else{
    linshi_d[2] = 999;
    Serial.println(F("電導返回空值"));
  }
  Serial.println("____________soil loop all data outputfinish: " + String(x));
  Serial.println(F("."));
  Serial.println(F("."));
  
  return linshi_d;
}

void processData(String& dat, String info[], int& arraySize) {
    Serial.println("return row data: ");
    Serial.print(dat);
    Serial.println(F("解碼..."));
    int commaPosition = -1;
    for (int i = 0; i < arraySize; i++) {
      commaPosition = dat.indexOf(',');
      if (commaPosition != -1) {
        info[i] = dat.substring(0, commaPosition);
        dat = dat.substring(commaPosition + 1, dat.length());
      } else {
        if (dat.length() > 0) {
          info[i] = dat.substring(0, commaPosition);
        }
      }
    }
  }