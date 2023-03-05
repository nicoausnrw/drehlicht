// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Arduino.h"
// Basic demo for accelerometer readings from Adafruit MPU6050

#include <MPU6050_tockn.h>
#include <Wire.h>

#include <FastLED.h>

#define LED_PIN 14
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];




MPU6050 mpu6050(Wire);


unsigned long previousMillis = 0;  // speichert die vergangene Zeit in Millisekunden
const unsigned long interval = 20; // die Zeit in Millisekunden zwischen jeder Überprüfung

int count = 0; //helfer damit ich jede sec einen Wert Anzeige.
float jetzigerWinkel[3];
float letzterWinkel[3];

float durschnittAbweichung[3];

int modus = 0; // 0 = Glas steht, 1 = Glas Liegt;



int hueHelligkeit = 128;
int hueRGB = 108;
int hueSaettigung = 200;


int hellFaktor = 6;
int rgbFaktor = 3;
int SaettigungFaktor = 3;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void saveWinkel(){
      mpu6050.update();

      jetzigerWinkel[0] = mpu6050.getAngleX();
      jetzigerWinkel[1] = mpu6050.getAngleY();
      jetzigerWinkel[2] = mpu6050.getAngleZ();

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void saveLetzterWinkel(){
    for (int i = 0; i < 3; i++) {
        letzterWinkel[i] = jetzigerWinkel[i];
      }
  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void korrektur(boolean firstTime = false){

    saveWinkel();
      
    if (firstTime){
      saveLetzterWinkel();
      
    }else{
      for (int i = 0; i < 3; i++) {
        durschnittAbweichung[i] = (durschnittAbweichung[i] + letzterWinkel[i] - jetzigerWinkel[i])/2; // Hier berechne ich wie sehr die Winkel von einander abweichej e Messung
      }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  saveWinkel();




  // TEST SETUP LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  FastLED.setBrightness(255);
  
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 240));
  
  FastLED.show();


}

void loop() {
  unsigned long currentMillis = millis(); // speichert die aktuelle Zeit in Millisekunden


  mpu6050.update();
  saveWinkel();



  // Überprüft, ob die Zeit seit der letzten Überprüfung größer oder gleich dem Intervall ist
  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis; // setzt die Zeit der letzten Überprüfung auf die aktuelle Zeit


    float differenz = letzterWinkel[2]-jetzigerWinkel[2];

    // Welcher Modus ist gerade dran?
    if( (abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) > 70 && abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) < 110 )){
       // Das Gals wird jetzt Seitlich gehalten. 
      modus = 1; // liegemodus Speichern


      if( differenz > 0.2 || differenz < -0.2){
        hueRGB = hueRGB+int(differenz*rgbFaktor);

  

      }




    }else if( (abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) > 160)){

      modus = 3; // steh Modus speichern

      
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if( differenz > 0.2 || differenz < -0.2){
        hueSaettigung = hueSaettigung+int(differenz*SaettigungFaktor);

        if (hueSaettigung > 255){
          hueSaettigung = 255;
        }else if(hueSaettigung < 0){
          hueSaettigung = 0;
        }

      }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    }else{
      modus = 0; // steh Modus speichern

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if( differenz > 0.2 || differenz < -0.2){
        hueHelligkeit = hueHelligkeit+int(differenz*hellFaktor);

        if (hueHelligkeit > 255){
          hueHelligkeit = 255;
        }else if(hueHelligkeit < 0){
          hueHelligkeit = 0;
        }

      }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    }


  fill_solid(leds, NUM_LEDS, CHSV(hueRGB, hueSaettigung, hueHelligkeit));


  FastLED.show();



    // gibt im 1sec Takt die Werte aus.
    count++;
    if (count >= 50){
      count = 0;

      
      Serial.print("angleX : ");
      Serial.print(letzterWinkel[0]);
      Serial.print("\t\tangleY : ");
      Serial.print(letzterWinkel[1]);
      Serial.print("\t\tangleZ : ");
      Serial.print(letzterWinkel[2]);
      Serial.print("\t\tModus: ");
      Serial.print(modus);
      Serial.print("\t\tletzterWinkel: ");
      Serial.print(letzterWinkel[2]);
      Serial.print("\t\tDurschnittAbweichung: ");
      Serial.print(durschnittAbweichung[2]);
      
      
      Serial.print("\n");
    }
  
  }

  saveLetzterWinkel();

}
