#include <WiFi.h>
#include <FirebaseESP32.h>
 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <LiquidCrystal.h>
#include "EmonLib.h"    
 
#define FIREBASE_HOST "https://final-98305-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Hb68fePoOC5RhGPbFM8Sg9NhjTpPCs3dV55Rfk00"
#define WIFI_SSID "robeel047"
#define WIFI_PASSWORD "33445566"
 
 
//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;
//int Vresistor = A0; 
//int Vrdata = 0; 

EnergyMonitor emon1;             // Create an instance
float kWh = 0;
unsigned long lastmillis = millis();


int Irms = 0;
int KWh = 0;
int supplyVoltage = 0;
int powerFActor = 0;
int apparentPower = 0;
int realPower = 0;
 
void setup()
{
 
  Serial.begin(115200);
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  emon1.voltage(35, 220, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(34, 0.5);   
 
 //pinMode(Vresistor, INPUT);

 
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
 
  //String path = "/data";
  
 
  Serial.println("------------------------------------");
  Serial.println("Connected...");
  
}
 
void loop()
{
  
emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  kWh = kWh + emon1.apparentPower * (millis() - lastmillis) / 3600000000.0;
           

  float realPower       = emon1.realPower;  //extract Real Power into variable
  Serial.print("\tReal Power: ");
  Serial.println(realPower);
  lcd.setCursor(0, 0);
  lcd.print("R.Power:");
  lcd.print(realPower);
  lcd.print("W");
  delay(100); 
  json.set("/realPower", realPower);
  Firebase.updateNode(firebaseData,"/",json);


  
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  Serial.print("\tApparent Power: ");
  Serial.println(apparentPower);
  lcd.setCursor(0, 1);
  lcd.print("A.Power:");
  lcd.print(apparentPower);
  lcd.print("W");
  delay(100); 
  json.set("/apparentPower", apparentPower);
  delay(2000);


  
   
  lcd.clear();
  float powerFActor = emon1.powerFactor;      //extract Power Factor into Variable
  Serial.print("\tPower Factor: ");
  Serial.println(powerFActor);
  lcd.setCursor(0, 0);
  lcd.print("P.Factor:");
  lcd.print(powerFActor);
  lcd.print("W");
  delay(100); 
  json.set("/powerFActor", powerFActor);
 


   
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  Serial.print("\tVoltage: ");
  Serial.println(supplyVoltage); 
  lcd.setCursor(0, 1);
  lcd.print("Voltage:");
  lcd.print(supplyVoltage);
  lcd.print("V");
  delay(100); 
  json.set("/supplyVoltage", supplyVoltage);
 
  delay(3000);






  lcd.clear();
  float Irms = emon1.Irms;             //extract Irms into Variable
  Serial.print("\tIrms: ");
  Serial.println(Irms);
  lcd.setCursor(0, 0);
  lcd.print("Irms:");
  lcd.print(Irms);
  lcd.print("A");  
  delay(100); 
  json.set("/Irms", Irms);



  
  Serial.print("\tKwh: ");
  Serial.println(kWh);
  lastmillis = millis();
  lcd.setCursor(0, 1);
  lcd.print("Kwh:");
  lcd.print(kWh);
  lcd.print("W");
  delay(100); 
  json.set("/KWh", kWh);

  delay(1000);

 
}

   

