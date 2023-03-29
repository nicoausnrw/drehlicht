#include "Arduino.h"

//#include <MPU6050_tockn.h>
#include <MPU6050_light.h>
#include <Wire.h>

#include <FastLED.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MPU6050 mpu6050(Wire);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Beschleuniguns Sensor
void writeRegister(uint16_t reg, byte value){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}


// TODO Welche davon benötige ich wirklich

// Beschleuniguns Sensor -------------------------------------------------------
#define MPU6050_PWR_MGT_1         0x6B // Power Management 1 Register
#define MPU6050_INT_PIN_CFG       0x37 // Interrupt Pin / Bypass Enable Configuration Register
#define MPU6050_INT_ENABLE        0x38 //## Interrupt Enable Register
//#define MPU6050_LATCH_INT_EN      0x05 // Latch Enable Bit for Interrupt 
    #define MPU6050_ACTL              0x07 // Active-Low Enable Bit
    #define MPU6050_WOM_EN            0x06 //## Wake on Motion Enable bit
#define MPU6050_WOM_THR           0x1F //## Wake on Motion Threshold Register
#define MPU6050_MOT_DUR           0x20 //## Motion Detection Duration Register

#define MPU6050_ACCEL_CONFIG      0x1c
// aus der Doku gezogen keine Ahnung ob das Hilft
//#define MPU6050_ACCEL_INTEL_CTRL  0x69 // Accelaration Interrupt Control Register
#define MPU6050_SIGNAL_PATH_RESET 0x68 // Signal Path Reset Register





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setInterrupt(byte threshold){
//writeRegister(MPU6050_SIGNAL_PATH_RESET, 0b00000111);  // not(?) needed
writeRegister(MPU6050_INT_PIN_CFG, 1<<MPU6050_ACTL); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000001);
  writeRegister(MPU6050_WOM_THR, threshold); 
  writeRegister(MPU6050_MOT_DUR, 0b00000001);  // set duration (LSB = 1 ms)
//writeRegister(MPU6050_ACCEL_INTEL_CTRL, 0x15);  // not needed (?)
  writeRegister(MPU6050_INT_ENABLE, 1<<MPU6050_WOM_EN);
  Serial.println("LÄUFT");
}



void setInterrupt3(byte threshold){
  writeRegister(MPU6050_SIGNAL_PATH_RESET, 0b00000111);  // not(?) needed
  
  writeRegister(MPU6050_INT_PIN_CFG, 1<<0x06); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_INT_PIN_CFG, 0<<MPU6050_ACTL); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_INT_ENABLE, 1<<0x00);
  
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000001);
  writeRegister(MPU6050_WOM_THR, threshold); 
  writeRegister(MPU6050_MOT_DUR, 0b00000001);  // set duration (LSB = 1 ms)
  //writeRegister(MPU6050_ACCEL_INTEL_CTRL, 0x15);  // not needed (?)
  writeRegister(MPU6050_INT_ENABLE, 0<<MPU6050_WOM_EN);
}




void setInterrupt2(byte threshold){
  writeRegister(MPU6050_SIGNAL_PATH_RESET, 0b00000111);  // not(?) needed
  writeRegister(MPU6050_INT_PIN_CFG, 0<<MPU6050_ACTL); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000001);
  writeRegister(MPU6050_WOM_THR, threshold); 
  writeRegister(MPU6050_MOT_DUR, 0b00000001);  // set duration (LSB = 1 ms)
  //writeRegister(MPU6050_ACCEL_INTEL_CTRL, 0x15);  // not needed (?)
  writeRegister(MPU6050_INT_ENABLE, 1<<MPU6050_WOM_EN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void disableInterrupt(){
  //setInterrupt(255);
  if(true){
  writeRegister(MPU6050_SIGNAL_PATH_RESET, 0b00000111);  // not(?) needed
  writeRegister(MPU6050_INT_PIN_CFG, 0<<MPU6050_ACTL); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_INT_PIN_CFG, 0<<0x06); //
  writeRegister(MPU6050_INT_PIN_CFG, 0<<0x02); //
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000001);
  writeRegister(MPU6050_WOM_THR, 255); 
  writeRegister(MPU6050_MOT_DUR, 0b00000001);  // set duration (LSB = 1 ms)
  //writeRegister(MPU6050_ACCEL_INTEL_CTRL, 0x15);  // not needed (?)

  writeRegister(MPU6050_INT_ENABLE, 0x00 );
}

}
/*


okay, es ist eine große Herrausforderung den ESP8266 mit dem MPU6050 zu wecken, eine Idee die mir noch gekommen ist den GPIO16 zu nehmen und damit erstmal den RESET Pin zu blocken da der 16ner HIGH at boot ist. 
denn müsste ich dann erstmal high halten. Dann wäre interannst was passiert wenn ich low gehe mit dem MPU, hier mal mit Bruno sprechen und dann am besten Andy der mir mit dem REsiger vom MPU helfen kann.
eventuell kann ich ja einfach GPIO16 dauerhaft high lassen und damit dem rest pin blocken.  Eventuell irgendwo Wiedersände dazwischen damit das klappt.

Plan B wäre ein ESP 32 der problemlos via pin geweckt werden kann TODO im code ist dazu auch noch jede menge schrott der hier nicht funktioniert.
Plan C ist ein Rest Button am Glas

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int modus = 0; // 0 = Glas steht, 1 = Glas Liegt;


// Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default bei Start
int hueHelligkeit = 128;
int hueColor = 108;
int hueSaettigung = 200;

// Drehempfindlichkeit
int hellFaktor = 6;
int rgbFaktor = 3;
int SaettigungFaktor = 3;

// nach wieviel sec soll die Lampe in den Deep Sleep.
int secBisDeepSleep = 5; 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define LED_PIN 14
#define NUM_LEDS 12
#define WAKEUP_PIN 12
CRGB leds[NUM_LEDS];

unsigned long previousMillis;  // speichert die vergangene Zeit in Millisekunden
unsigned long currentMillis; // speichert später die aktuelle Zeit in Millisekunden für das abarbeiten des Loops.

const unsigned long interval = 20; // die Zeit in Millisekunden zwischen jeder Überprüfung - 20ms sind  50Hz / 50fps

int count = 0; //helfer damit ich jede sec einen Wert Anzeige.

// Speicherplatz für Winkeldaten.
float jetzigerWinkel[3];
float letzterWinkel[3];
float durschnittAbweichung[3];  // kann vermutlich weg

unsigned long startZeitLichtAus = 0;  // Wenn das licht das erste mal aus geht, speichere ich die Laufzeittime und kann diese nutzen um in den DeepSleep zu gehen


float differenz; // Differenz jetziger und letzter Winkel.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IRAM_ATTR wakeUp(){ // das IRAM_ATTR braucht man bei dem ESP8266 um eine Interupt Funktion für den ESP klar zumachen, diese funktion rufe ich später auf. Details dazu: https://www.arduinoforum.de/arduino-Thread-Interrupt-Routinren-bei-ESP 
  // startZeitLichtAus = 0;
  Serial.println("Hey! Bin wieder da!");

}


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

// TODO kann vermutlich raus
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


void runterfahren(){

  setInterrupt(15); // set Wake on Motion Interrupt / Sensitivity; 1(highest sensitivity) - 255

  Serial.println("ciao ciao");

  ESP.deepSleep(0);

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void checkSleep(){
  if (hueHelligkeit == 0) { // direkt mal Prüfen ob Helligkeit größer null sein kann aber das Licht trozdem aus ist



    if(startZeitLichtAus == 0){
      
      startZeitLichtAus = currentMillis;
      
      Serial.println("Licht ist Aus");
    
    }else if (currentMillis - startZeitLichtAus >= secBisDeepSleep*1000){ // Die Wartezeit ist überschritten

    runterfahren();

    }  
  }else{
    startZeitLichtAus = 0;
  }

  }

// 0 = Steh Modus, 1 Liegt Mouds, 2 Kopfüber Modus
void setLightStyle(int modus, float differenz){
       
        // Funktion wird nur ausgelöst wenn Differenz seit der letzten prüfung groß genug ist
        if( differenz > 0.2 || differenz < -0.2){
        
          switch (modus) {
            
            
            case 0: // Steht Modus
              hueHelligkeit = hueHelligkeit+int( differenz*hellFaktor );

              if (hueHelligkeit > 255){
                hueHelligkeit = 255;
              }else if(hueHelligkeit < 0){
                hueHelligkeit = 0;
              }
              break;
            
            
            
            case 1: // Liegt Modus
              hueColor = hueColor+int( differenz*rgbFaktor );
              break;
            
            
            case 2: // Kopfüber Modus
              hueSaettigung = hueSaettigung+int( differenz*SaettigungFaktor );

              if (hueSaettigung > 255){
                hueSaettigung = 255;
              }else if(hueSaettigung < 0){
                hueSaettigung = 0;
              }
              break;


            default:
              Serial.println("Lol was ist los das ich das hier ausgeben muss?!");
              break;
          }







      }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int gpioPin = 12;

void setup() {
  Serial.begin(9600);
setInterrupt(1);

    pinMode(gpioPin, INPUT); // DEBUG

  // SETUP LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CHSV(hueColor, hueSaettigung, hueHelligkeit));

  FastLED.show();


  Wire.begin();
  mpu6050.begin();
  
  //disableInterrupt();
  
  mpu6050.calcOffsets(true,false);
  saveWinkel();



  if(false){
    // Beschleuniguns Sensor ------------------------------------------------------------------------------------------------------------------------
    //Wire.begin();
    writeRegister(MPU6050_PWR_MGT_1, 0);

  // Konfiguriere den GPIO12-Pin als Wake-up-Pin
    pinMode(WAKEUP_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN), wakeUp, RISING);
  }


}

void loop() {

//DEBUG
  pinMode(gpioPin, INPUT); // DEBUG
  if (digitalRead(gpioPin) == HIGH) {
    Serial.println("Pang pang");};




  currentMillis = millis(); // speichert die aktuelle Zeit in Millisekunden

  saveWinkel(); // holt die aktuellen Daten. 



  // Überprüft, ob die Zeit seit der letzten Überprüfung größer oder gleich dem Intervall ist
  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis; // setzt die Zeit der letzten Überprüfung auf die aktuelle Zeit


    differenz = letzterWinkel[2]-jetzigerWinkel[2]; // der Unterschied wie sich das Gals auf der Z Achse verdreht hat.

    // TODO genauer die Position vom Glas rausfinden.
    // Welcher Modus ist gerade dran? sprich steht, liegt oder ist das Glas auf dem Kopf.

    if( (abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) > 70 && abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) < 110 )){
      modus = 1; // Liegt Modus

    }else if( (abs(jetzigerWinkel[0])+abs(jetzigerWinkel[1]) > 160)){
      modus = 2; // Kopfüber Modus

    }else{
      modus = 0; // Steht Modus

    }

  // LEDs werden eingestellt und geupdatet.
  setLightStyle(modus, differenz);
  fill_solid(leds, NUM_LEDS, CHSV(hueColor, hueSaettigung, hueHelligkeit));
  FastLED.show();



    // gibt im 1sec Takt die Werte aus.
    count++;
    if (count >= 50 && false){
      count = 0;

      
      Serial.print("angleX : ");
      Serial.print(jetzigerWinkel[0]);
      Serial.print("\t\tangleY : ");
      Serial.print(jetzigerWinkel[1]);
      Serial.print("\t\tangleZ : ");
      Serial.print(jetzigerWinkel[2]);
      Serial.print("\t\tModus: ");
      Serial.print(modus);
      Serial.print("\t\tletzterWinkel: ");
      Serial.print(letzterWinkel[2]);
      Serial.print("\t\tDurschnittAbweichung: ");
      Serial.print(durschnittAbweichung[2]);
      
      
      Serial.print("\n");
    }
  
  }


  checkSleep(); // Prüft ob die Lampe aus ist und es in den Tiefschlaf gehen soll.

  saveLetzterWinkel(); // Speichert aus dieser Runde die Winkel weg um diese in der Nächsten Runde vergleichen zu können.

}
