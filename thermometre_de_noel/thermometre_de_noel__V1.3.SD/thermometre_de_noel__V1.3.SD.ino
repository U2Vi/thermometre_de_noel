//------------------------------------------------------------------------------------------------
//  By Sébastien DYCKE
//
//  Instagram : @ingenior.seb
//  SD-Radio  : https://sd-radio.fr/
//  GitHUB    : https://github.com/U2Vi
//
//  Arduino IDE : 1.8.57.0 
//  µC          : ESP32-WROOM-32D
//  Code        : V1.3.SD
//
//  ©Copyright 2022-2023
//------------------------------------------------------------------------------------------------

#include <Wire.h>              //I2C lib
#include "SHTSensor.h"         //SHT85 sensor
#include <WiFi.h>              //WIFI
#include <WiFiClient.h>        //WIFI CL
#include <WiFiAP.h>            //WIFI AP
#include <Preferences.h>       //EEPROM
#include <Adafruit_NeoPixel.h> //LEDs WS2812X

#define led_01      04
#define led_02      16
#define led_03      17
#define led_04      14
#define led_05      27
#define led_06      26
#define smartLed_01 25
#define bouton_01   05

#define LED_COUNT 6 //WS2812X

SHTSensor sht;                                                         //SHT85 sensor init
Preferences preferences;                                               //EEPROM init
Adafruit_NeoPixel strip(LED_COUNT, smartLed_01, NEO_GRB + NEO_KHZ800); //WS2812X init

//Timers
unsigned long currentTime_0  = 0;
unsigned long previousTime_0 = 0;
unsigned long currentTime_1  = 0;
unsigned long previousTime_1 = 0;
unsigned long currentTime_2  = 0;
unsigned long previousTime_2 = 0;

//MODEs var
int PWM_0 = 0;
int PWM_1 = 0;
int PWM_2 = 0;
int PWM_3 = 0;
int PWM_4 = 0;
int PWM_5 = 0;

long firstPixelHue = 0;

int stripR_0 = 50;
int stripR_1 = 50;
int stripR_2 = 50;
int stripR_3 = 50;
int stripR_4 = 50;
int stripR_5 = 50;

//SSID name
String nom_reseau_temperature;

//MODE select
int MODE;
int newMODE;

void setup() {
  
 Wire.begin();
 Serial.begin(9600);
 delay(100);

 //WS2812X Init
 strip.begin();
 strip.show();
 strip.setBrightness(50);

 preferences.begin("myfile", false); //eeprom init
 MODE = newMODE = preferences.getUInt("MODE", 0); //If the case "MODE" does not exist, creat and place "0". Else if, read the value of case "MODE". 
   
 if (sht.init()){Serial.print("init(): success\n");}else{Serial.print("init(): failed\n");}
 sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);

 pinMode(bouton_01, INPUT  );
 
 ledcSetup(0, 5000, 8); //PWM -> Channel,frequence,resolution
 ledcAttachPin(led_01, 0); // Broche, channel
 ledcSetup(1, 5000, 8);
 ledcAttachPin(led_02, 1);
 ledcSetup(2, 5000, 8);
 ledcAttachPin(led_03, 2);
 ledcSetup(3, 5000, 8);
 ledcAttachPin(led_04, 3);
 ledcSetup(4, 5000, 8);
 ledcAttachPin(led_05, 4);
 ledcSetup(5, 5000, 8);
 ledcAttachPin(led_06, 5);
  
}

void loop(){

  //Get humidity and tmp
  if (sht.readSample()){nom_reseau_temperature = "Tmp : "+String(sht.getTemperature(), 2)+"°C  HR : "+String(sht.getHumidity(), 1)+"%";}
  else{Serial.println("Error in readSample() [tmp/rh sensor]");}

  modeSelect(); //Select mode wth switch

  //Switch mode  
  switch(MODE){
    case 1:
      modeDiscret();
      break;
    case 2:
      modeNormal();
      break;
    case 3:
      modeNoel_01();
      break;
    case 4:
      modeNoel_02();
      break;  
    case 5:
      modeNoel_03();
      break;
    case 6:
      modeNoel_04();
      break;
    default:
      modeDiscret();
      break;
  }

  modeUpdate(); //Update mode value in EEPROM

  //Update name of SSID 
  currentTime_1 = millis();
  if((currentTime_1 - previousTime_1)>10000){
    previousTime_1 = currentTime_1;
    wifi(nom_reseau_temperature); //Update tmp/HR into wifi ssid. 
    Serial.print("[MODE : ");Serial.print(MODE);Serial.println("]");
  }
  
}




//Select mode wth switch
void modeSelect(){
  if (digitalRead(bouton_01) == 1) {
    previousTime_2 = millis();
    
    //Preds une postion en fonction du mode en cours.
    MODE = preferences.getUInt("MODE", 0); //Dans eeprom -> Si case "MODE" vide, remplie par "0". Sinon lit le contenu de la case "MODE".
    if (MODE == 2){
      previousTime_2 = previousTime_2-1000;
      Serial.println("[Start to MODE : 2]");
    }
    if (MODE == 3){
      previousTime_2 = previousTime_2-2000;
      Serial.println("[Start to MODE : 3]");
    }
    if (MODE == 4){
      previousTime_2 = previousTime_2-3000;
      Serial.println("[Start to MODE : 4]");
    }
    if (MODE == 5){
      previousTime_2 = previousTime_2-4000;
      Serial.println("[Start to MODE : 5]");
    }
    if (MODE == 6){
      previousTime_2 = previousTime_2-5000;
      Serial.println("[Start to MODE : 6]");
    }
    
    while(digitalRead(bouton_01)){
      currentTime_2 = millis();
      if (currentTime_2-previousTime_2<1000){
        Serial.print("1|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 255);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
        ledcWrite(4, 0);
        ledcWrite(5, 0);
        newMODE = 1;
      }else if((currentTime_2-previousTime_2>1000)&&(currentTime_2-previousTime_2<2000)){
        Serial.print("2|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 0);
        ledcWrite(1, 255);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
        ledcWrite(4, 0);
        ledcWrite(5, 0);
        newMODE = 2;
      }else if((currentTime_2-previousTime_2>2000)&&(currentTime_2-previousTime_2<3000)){
        Serial.print("3|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 255);
        ledcWrite(3, 0);
        ledcWrite(4, 0);
        ledcWrite(5, 0);
        newMODE = 3;
      }else if((currentTime_2-previousTime_2>3000)&&(currentTime_2-previousTime_2<4000)){
        Serial.print("4|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 255);
        ledcWrite(4, 0);
        ledcWrite(5, 0);
        newMODE = 4;
      }else if((currentTime_2-previousTime_2>4000)&&(currentTime_2-previousTime_2<5000)){
        Serial.print("5|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
        ledcWrite(4, 255);
        ledcWrite(5, 0);
        newMODE = 5;
      }else if((currentTime_2-previousTime_2>5000)&&(currentTime_2-previousTime_2<6000)){
        Serial.print("6|");Serial.println(currentTime_2-previousTime_2);
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
        ledcWrite(4, 0);
        ledcWrite(5, 255);
        newMODE = 6;
      }else{
        previousTime_2 = millis();  
      }
    }
  }
}


// Update MODE value into the EEPROM.
void modeUpdate(){ 
  MODE = preferences.getUInt("MODE", 0);
  if (MODE != newMODE){ // Update MODE value into the EEPROM.
    Serial.println("[EEPROM UPDATE]");
    preferences.putUInt("MODE", newMODE);
  }
}

//Create wifi network 
void wifi(String nom_reseau_tmp){
  const char *ssid = nom_reseau_tmp.c_str();
  const char *password = "";
  WiFiServer server(80);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
  Serial.println(nom_reseau_temperature);
  Serial.println("(server started)");
}


//[OK] All light off
void modeDiscret(){ 
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  ledcWrite(5, 0); 
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.setPixelColor(2, strip.Color(0, 0, 0));
  strip.setPixelColor(3, strip.Color(0, 0, 0));
  strip.setPixelColor(4, strip.Color(0, 0, 0));
  strip.setPixelColor(5, strip.Color(0, 0, 0));
  strip.show();
  
}

//[OK] Just WS2812X animations 
void modeNormal(){
currentTime_0 = millis();
  if((currentTime_0 - previousTime_0)>100){
    previousTime_0 = currentTime_0;
    
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    ledcWrite(4, 0);
    ledcWrite(5, 0);
  
    stripR_0 += random(0, 11);
    if(stripR_0 >= 256){stripR_0 = 50;}
    stripR_1 += random(0, 11);
    if(stripR_1 >= 256){stripR_1 = 50;}
    stripR_2 += random(0, 11);
    if(stripR_2 >= 256){stripR_2 = 50;}
    stripR_3 += random(0, 11);
    if(stripR_3 >= 256){stripR_3 = 50;}
    stripR_4 += random(0, 11);
    if(stripR_4 >= 256){stripR_4 = 50;}
    stripR_5 += random(0, 11);
    if(stripR_5 >= 256){stripR_5 = 50;}
  
    
    strip.setPixelColor(0, strip.Color(stripR_0, 255, 50));
    strip.setPixelColor(1, strip.Color(stripR_1, 255, 50));
    strip.setPixelColor(2, strip.Color(stripR_2, 255, 50));
    strip.setPixelColor(3, strip.Color(stripR_3, 255, 50));
    strip.setPixelColor(4, strip.Color(stripR_4, 255, 50));
    strip.setPixelColor(5, strip.Color(stripR_5, 255, 50));
    strip.show();
  }
}

//[MANQUE NEOPIXL] WS2812X and LEDS animations 1
void modeNoel_01(){
  currentTime_0 = millis();
  if((currentTime_0 - previousTime_0)>100){
    previousTime_0 = currentTime_0;

    PWM_0 += random(0, 11);
    if(PWM_0 >= 256){PWM_0 = 0;}
    PWM_1 += random(0, 11);
    if(PWM_1 >= 256){PWM_1 = 0;}
    PWM_2 += random(0, 11);
    if(PWM_2 >= 256){PWM_2 = 0;}
    PWM_3 += random(0, 11);
    if(PWM_3 >= 256){PWM_3 = 0;}
    PWM_4 += random(0, 11);
    if(PWM_4 >= 256){PWM_4 = 0;}
    PWM_5 += random(0, 11);
    if(PWM_5 >= 256){PWM_5 = 0;}
    
    ledcWrite(0, random(0, PWM_0));
    ledcWrite(1, random(0, PWM_1));
    ledcWrite(2, random(0, PWM_2));
    ledcWrite(3, random(0, PWM_3));
    ledcWrite(4, random(0, PWM_4));
    ledcWrite(5, random(0, PWM_5));

    strip.setPixelColor(0, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.setPixelColor(1, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.setPixelColor(2, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.setPixelColor(3, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.setPixelColor(4, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.setPixelColor(5, strip.Color(random(0, 256), random(0, 256), random(0, 256)));
    strip.show();
  }
}

//[OK] Just LEDS animations 
void modeNoel_02(){
  currentTime_0 = millis();
  if((currentTime_0 - previousTime_0)>100){
    previousTime_0 = currentTime_0;

    PWM_0 += random(0, 11);
    if(PWM_0 >= 256){PWM_0 = 0;}
    PWM_1 += random(0, 11);
    if(PWM_1 >= 256){PWM_1 = 0;}
    PWM_2 += random(0, 11);
    if(PWM_2 >= 256){PWM_2 = 0;}
    PWM_3 += random(0, 11);
    if(PWM_3 >= 256){PWM_3 = 0;}
    PWM_4 += random(0, 11);
    if(PWM_4 >= 256){PWM_4 = 0;}
    PWM_5 += random(0, 11);
    if(PWM_5 >= 256){PWM_5 = 0;}
    
    ledcWrite(0, random(0, PWM_0));
    ledcWrite(1, random(0, PWM_1));
    ledcWrite(2, random(0, PWM_2));
    ledcWrite(3, random(0, PWM_3));
    ledcWrite(4, random(0, PWM_4));
    ledcWrite(5, random(0, PWM_5));
  }
  
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.setPixelColor(2, strip.Color(0, 0, 0));
  strip.setPixelColor(3, strip.Color(0, 0, 0));
  strip.setPixelColor(4, strip.Color(0, 0, 0));
  strip.setPixelColor(5, strip.Color(0, 0, 0));
  strip.show();
}

//[OK] WS2812X and LEDS animations 2
void modeNoel_03(){

  //Gestion leds WS2812X
  int wait = 5;
  if(firstPixelHue < 5*65536){
    firstPixelHue += 256;
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }else{
    firstPixelHue = 0;
  }
  
  //Gestion leds
  currentTime_0 = millis();
  if((currentTime_0 - previousTime_0)>500){
    previousTime_0 = currentTime_0;
    if (random(0, 2)==1){ledcWrite(0, 255);}else{ledcWrite(0, 0);}
    if (random(0, 2)==1){ledcWrite(1, 255);}else{ledcWrite(1, 0);}
    if (random(0, 2)==1){ledcWrite(2, 255);}else{ledcWrite(2, 0);}
    if (random(0, 2)==1){ledcWrite(3, 255);}else{ledcWrite(3, 0);}
    if (random(0, 2)==1){ledcWrite(4, 255);}else{ledcWrite(4, 0);}
    if (random(0, 2)==1){ledcWrite(5, 255);}else{ledcWrite(5, 0);}
  }
}

//[OK] WS2812X animations 1
void modeNoel_04(){

  //Gestion leds WS2812X
  int wait = 5;
  if(firstPixelHue < 5*65536){
    firstPixelHue += 256;
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }else{
    firstPixelHue = 0;
  }
  
  //Gestion leds
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  ledcWrite(5, 0); 
}
