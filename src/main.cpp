#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "EmonLib.h"    

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins 
#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN   16
#define CS_PIN   17
#define RST_PIN  5

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

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


float Irms = 0;
float KWh = 0;
float supplyVoltage = 0;
float powerFActor = 0;
float apparentPower = 0;
float realPower = 0;

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);  
float p = 3.1415926;



void setup()
{
 
  Serial.begin(9600);
  Serial.print("hello!");
  tft.begin();

  Serial.println("init");

//  tft.fillRect(0, 0, 128, 128, BLUE);
//  delay(2000);
  tft.fillScreen(BLACK);
  
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
           

  realPower       = emon1.realPower;  //extract Real Power into variable
  Serial.print("\tReal Power: ");
  Serial.println(realPower); 
  String realpower = String(realPower,2);
  json.set("/Real Power", realpower);
  Firebase.updateNode(firebaseData,"/",json);


  
  apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  Serial.print("\tApparent Power: ");
  Serial.println(apparentPower); 
  String apparentpower = String(apparentPower,2);
  json.set("/Apparent Power", apparentpower);
  
  
  powerFActor = emon1.powerFactor;      //extract Power Factor into Variable
  Serial.print("\tPower Factor: ");
  Serial.println(powerFActor);
  String powerfactor = String(powerFActor,2);
  json.set("/Power Factor", powerfactor);
 

  supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  Serial.print("\tVoltage: ");
  Serial.println(supplyVoltage); 
  String supplyvoltage = String(supplyVoltage,2);
  json.set("/Voltage", supplyvoltage);


  
  Irms = emon1.Irms;             //extract Irms into Variable
  Serial.print("\tIrms: ");
  Serial.println(Irms);
  String IRMS = String(Irms,2);
  json.set("/Current", IRMS);

  
  Serial.print("\tKwh: ");
  Serial.println(kWh);
  lastmillis = millis();
  String kwh = String(kWh,2);
  json.set("/KWh", kwh);

    tft.setTextColor(WHITE);
    tft.setCursor(13,65);
    tft.print("RP");
    tft.setCursor(60,65);
    tft.print(realPower);
    tft.drawRoundRect(0, 62, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 63, 123, 18, 5,CYAN);
    tft.drawLine(45, 63, 45, 89, CYAN);
    tft.drawLine(46, 63, 46, 89, CYAN);

    tft.setTextColor(CYAN);
    tft.setCursor(13,85);
    tft.print("AP"); 
    tft.setCursor(60,85);
    tft.print(apparentPower);
    tft.drawRoundRect(0, 82, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 83, 123, 18, 5,CYAN);
    tft.drawLine(45, 83, 45, 109, CYAN);
    tft.drawLine(46, 83, 46, 109, CYAN);

    tft.setTextColor(WHITE);
    tft.setCursor(7,105);
    tft.print("P.F");
    tft.setCursor(60,105);
    tft.print(powerFActor);
    tft.drawRoundRect(0, 102, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 103, 123, 18, 5,CYAN);
    tft.drawLine(45, 103, 45, 120, CYAN);
    tft.drawLine(46, 103, 46, 120, CYAN);

    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(18,5);
    tft.print("V");
    tft.setCursor(60,5);
    tft.print(supplyVoltage);
    tft.drawRoundRect(0, 2, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 3, 123, 18, 5,CYAN);
    tft.drawLine(45, 3, 45, 20, CYAN);
    tft.drawLine(46, 3, 46, 20, CYAN);

    tft.setTextColor(WHITE);
    tft.setCursor(18,25);
    tft.print("I");
    tft.setCursor(60,25);
    tft.print(Irms);
    tft.drawRoundRect(0, 22, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 23, 123, 18, 5,CYAN);
    tft.drawLine(45, 23, 45, 39, CYAN);
    tft.drawLine(46, 23, 46, 39, CYAN);


    tft.setTextColor(CYAN);
    tft.setCursor(7,45);
    tft.print("KWh");
    tft.setCursor(60,45);
    tft.print(KWh);    
    tft.drawRoundRect(0, 42, 125, 20, 5,WHITE);
    tft.drawRoundRect(1, 43, 123, 18, 5,CYAN);
    tft.drawLine(45, 43, 45, 65, CYAN);
    tft.drawLine(46, 43, 46, 65, CYAN);
  delay(3000);
 
 tft.fillScreen(BLACK);
}




