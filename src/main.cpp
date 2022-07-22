/*
 * Firebase ESP32
 * https://www.electroniclinic.com/
 */

#include <WiFi.h>
#include <FirebaseESP32.h>

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


#include "EmonLib.h"
#define vCalibration 83.3
#define currCalibration 0.50


#define FIREBASE_HOST "https://hello-52c2a-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "PlhqAYWRrLfO1HqtO5GzR9TsNGDXt1QUlEWkbTvA"
#define WIFI_SSID "Shoaib"
#define WIFI_PASSWORD "A0123456789"

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
EnergyMonitor emon;

float kWh = 0;
unsigned long lastmillis = millis();



//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;


int   Vo = 32; 
int Vodata = 0;

int Cu = 33;
int Cudata = 0;

int KWh;
int kwdata = 0;

int Po;
int Pdata = 0;



void file()
{
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  yield();
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
 
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");
 
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
 
  Serial.print("\tkWh: ");
  Serial.print(kWh, 5);
  Serial.println("kWh");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(emon.Irms, 4);
  lcd.print("A");
  delay(2500);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Power:");
  lcd.print(emon.apparentPower, 4);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("kWh:");
  lcd.print(kWh, 4);
  lcd.print("W");
  delay(2500);
 
  lastmillis = millis();
 
 
}






void setup()
{
  lcd.init();
  lcd. backlight ();
  Serial.begin(115200);
  lcd.begin(16, 2);
 



  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(firebaseData, true);
  */

  String path = "/data";
  

  Serial.println("------------------------------------");
  Serial.println("Connected...");



  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.
 
  
  lcd.setCursor(3, 0);
  lcd.print("IoT Energy");
  lcd.setCursor(5, 1);
  lcd.print("Meter");
  delay(3000);
  lcd.clear();
  
}


void loop()
{

 Vodata = analogRead(emon.Vrms);
 int Vdata = map(Vodata,0,4095,0,1000);
 Serial.println(Vdata); 

 Cudata = analogRead(emon.Irms);
 int Cdata = map(Cudata,0,4095,0,1000);
 Serial.println(Cdata);

 Po = analogRead(emon.apparentPower);
 int Pdata = map(Po,0,4095,0,1000); 
 Serial.println(Pdata); 

  KWh = analogRead(kWh);
 int Kdata = map(KWh,0,4095,0,1000); 
 Serial.println(Kdata); 

  json.set("/Data", Cdata);
  Firebase.updateNode(firebaseData,"Current",json);

  json.set("/Data", Vdata);
  Firebase.updateNode(firebaseData,"Voltage",json);

  json.set("/Data", Pdata);
  Firebase.updateNode(firebaseData,"Power",json);
 
  json.set("/Data", Kdata);
  Firebase.updateNode(firebaseData,"kWh",json);
  

} 



