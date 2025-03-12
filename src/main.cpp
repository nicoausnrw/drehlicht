#include "Arduino.h"


//#include <MPU6050_tockn.h>
#include <MPU6050_light.h>
#include <Wire.h>

#include "esp_sleep.h"

#include <FastLED.h>



// DEBUG
#define PRINTDEBUG 1


// damit kann ich schnell wechseln auf was der INT beobachtungs PIN achten soll.
#define VARIANTE_L  // VARIANTE_H / VARIANTE_L



#ifdef VARIANTE_H
    #define WAKEUP_AT ESP_GPIO_WAKEUP_GPIO_HIGH
    #define WAKEUP_MPU_SETTING 0b00000000
    #define LOW_OR_HIGH HIGH
    #define PULL_FOR_LOW_OR_HIGH INPUT_PULLDOWN


#elif defined(VARIANTE_L)
    #define WAKEUP_AT ESP_GPIO_WAKEUP_GPIO_LOW
    #define WAKEUP_MPU_SETTING 0b10000000
    
    #define LOW_OR_HIGH LOW
    #define PULL_FOR_LOW_OR_HIGH INPUT_PULLUP

#endif


// Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default bei Start
int hueHelligkeit = 128;
int hueColor = 32; //108;
int hueSaettigung = 200;

// Drehempfindlichkeit
int hellFaktor = 12;
int rgbFaktor = 6;
int SaettigungFaktor = 12;

// nach wieviel sec soll die Lampe in den Deep Sleep.
int secBisDeepSleep = 2; 

// LEDs
#define LED_PIN 6
#define NUM_LEDS 24


#define SLEEPBUTTON 0

// MOSFET / TRANSISTOR
#define StandbyMosfet_PIN 3

// MPU6050

// INTERRUPT PIN - WICHTIG: Pin sollte ein RTC-GIPIO Sein. diese sind im Deep Sleep System eingebunden.
#define MPUInterrupt_PIN 5

// Erschütterungs Empfindlichkeit & Dauer. 
// byte kann werte zwischen 0 & 255 Speichern. 
byte shakeFaktor = 20;
byte shakeDauer = 40;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CRGB leds[NUM_LEDS];


int modus = 0; // 0 = Glas steht, 1 = Glas Liegt;
boolean geradeHochgefahren = true;


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


MPU6050 mpu6050(Wire);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Beschleuniguns Sensor
void writeRegister(uint16_t reg, byte value){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte readRegister(byte reg) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(MPU6050_ADDR, 1);
    return Wire.read();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if PRINTDEBUG
void printRegisterValues(const std::initializer_list<int>& registers) {
    Serial.println("Register | Value");
    Serial.println("----------------");

    for (int reg : registers) {
        byte value = readRegister(reg);
        Serial.print(reg);
        Serial.print("       | ");
        for (int i = 7; i >= 0; i--) {
            Serial.print(bitRead(value, i));
        }
        Serial.println();
    }
}
#endif
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


// Den Stromsprmodus-MOSFET. Damit kann alles vom Strom genommen werden was nicht relevant ist für den DeepSleep.

void standbyStromON( boolean aktivieren){

// wenn "aktivieren"== TRUE, dann HIGH, sonst LOW.
digitalWrite(StandbyMosfet_PIN, aktivieren ? HIGH : LOW);


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





// TODO Welche davon benötige ich wirklich

// Beschleuniguns Sensor -------------------------------------------------------
#define MPU6050_ADDR              0x68 // Alternatively set AD0 to HIGH  --> Address = 0x69
#define MPU6050_ACCEL_CONFIG      0x1C // Accelerometer Configuration Register
#define MPU6050_PWR_MGT_1         0x6B // Power Management 1 Register
#define MPU6050_INT_PIN_CFG       0x37 // Interrupt Pin / Bypass Enable Configuration Register
#define MPU6050_INT_ENABLE        0x38 // Interrupt Enable Register
#define MPU6050_LATCH_INT_EN      0x05 // Latch Enable Bit for Interrupt 
#define MPU6050_ACTL              0x07 // Active-Low Enable Bit
#define MPU6050_WOM_THR           0x1F // Wake on Motion Threshold Register
#define MPU6050_MOT_DUR           0x20 // Motion Detection Duration Register
#define MPU6050_ACCEL_INTEL_CTRL  0x69 // Accelaration Interrupt Control Register
#define MPU6050_SIGNAL_PATH_RESET 0x68 // Signal Path Reset Register


#define MPU6050_WOM_EN            0x06 // Wake on Motion Enable bit




// die hatte ich eigentlich fleißig erarbeiet, doch weiter unten ist jetzt die Version vom esp2866, eventuell hilft diese das ich das mit dem INt hinbekomme. Komisch das so CFG nicht angepackt wird.
void setInterrupt__2025Version(byte threshold, byte dauer){

    
  // MPU6050 Interrupt Setup

  // Konfiguriert den MPU6050 für Wake-on-Motion Interrupt
  // - Interrupt-Pin als Eingang
  // - Aktiviert Motion Detection Interrupt
  // - Setzt Schwellenwert (1 LSB = 4mg) und Dauer (1 LSB = 1 ms)
  // - Interrupt-Pin Konfiguration (Push-Pull, aktives High-Signal)
  // - Wake-on-Motion aktivieren
  // - Keine Schlafoptimierung
  writeRegister(MPU6050_INT_ENABLE, 0b01000000); // Motion Detection Interrupt aktivieren
  writeRegister(MPU6050_WOM_THR, threshold); // Schwellenwert ab welcher Bewegung erkannt wird. 1 LSB = 4mg
  writeRegister(MPU6050_MOT_DUR, dauer);  // set duration (LSB = 1 ms) // wie Lang soll "die Bewegung" sein. angabe in ms
  writeRegister(MPU6050_INT_PIN_CFG, 0b00000000); // Interrupt aktivieren ---- tellen Sie sicher, dass das Bit 7 (ACTL) im MPU6050_INT_PIN_CFG-Register auf 0 gesetzt ist, um den Interrupt-Pin als aktives High-Signal zu konfigurieren. Verwenden Sie 0b00000000 für das MPU6050_INT_PIN_CFG-Register, um den Interrupt-Pin als Push-Pull und aktives High-Signal zu konfigurieren.
  writeRegister(MPU6050_PWR_MGT_1, 0b00000000); // Wake on Motion aktivieren
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000000);

}


void setInterrupt(byte threshold, byte shakeDauer){
//writeRegister(MPU6050_SIGNAL_PATH_RESET, 0b00000111);  // not(?) needed
//writeRegister(MPU6050_INT_PIN_CFG, 1<<MPU6050_ACTL); // 1<<MPU6050_LATCH_INT_EN
//writeRegister(MPU6050_INT_PIN_CFG, 1<<MPU6050_LATCH_INT_EN); // 1<<MPU6050_LATCH_INT_EN
  writeRegister(MPU6050_ACCEL_CONFIG, 0b00000001);
  writeRegister(MPU6050_WOM_THR, threshold); 
  writeRegister(MPU6050_MOT_DUR, 50);  // set duration (LSB = 1 ms) // wie Lang soll "die Bewegung" sein. angabe in ms
//writeRegister(MPU6050_ACCEL_INTEL_CTRL, 0x15);  // not needed (?)
  writeRegister(MPU6050_INT_ENABLE, 1<<MPU6050_WOM_EN);

  // jetzt hinzugefügt damit ich festlegen kann ob low oder high weckt
  writeRegister(MPU6050_INT_PIN_CFG, WAKEUP_MPU_SETTING); // Interrupt aktivieren ---- tellen Sie sicher, dass das Bit 7 (ACTL) im MPU6050_INT_PIN_CFG-Register auf 0 gesetzt ist, um den Interrupt-Pin als aktives High-Signal zu konfigurieren. Verwenden Sie 0b00000000 für das MPU6050_INT_PIN_CFG-Register, um den Interrupt-Pin als Push-Pull und aktives High-Signal zu konfigurieren.
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void checkInterruptPin() {
    if (digitalRead(MPUInterrupt_PIN) == LOW_OR_HIGH) {
        Serial.println("WACKEL WACKEL");
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void runterfahren() {
    #if PRINTDEBUG
        Serial.println("ciao ciao");

        // FÜR TESTZWECKE
        FastLED.clear();  // clear all pixel data 
        FastLED.show();
    #endif

    setInterrupt(shakeFaktor, shakeDauer); // set Wake on Motion Interrupt / Sensitivity; 1(highest sensitivity) - 255

    // Strom für alle unwichtigen Komponenten ausschalten.
    // Nur Lagesensor und ESP sollen am Strom bleiben um Energie zu sparen
    standbyStromON(false);

    // ESP ins Bett bringen
    esp_err_t result = esp_deep_sleep_enable_gpio_wakeup((1ULL << MPUInterrupt_PIN), WAKEUP_AT); 

    #if PRINTDEBUG
        Serial.println(result == ESP_OK ? "ESP_OK" : "ESP_ERR_INVALID_ARG");

        unsigned long startTime = millis();
        while (millis() - startTime < 500) {
            // Hier kannst du optional etwas einfügen, was in der Schleife passieren soll
        }
    #endif

    esp_deep_sleep_start();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
              hueHelligkeit = hueHelligkeit-int( differenz*hellFaktor );

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


void setup() {
    Serial.begin(115200);
  
    #if PRINTDEBUG
        Serial.println("HELLO!");
        pinMode(SLEEPBUTTON, INPUT_PULLUP);

        
    #endif

    // Strom für alle Komponenten einschalten.
    standbyStromON(true);

    // SETUP LEDs
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(64);

    // SETUP Lagesensor - MPU6050
    Wire.begin(); 
    mpu6050.begin();
    // writeRegister(MPU6050_PWR_MGT_1, 0b10000000); // MPU FULL RESET

    // über die Varibale kann ich schnell switchen ob das SETUP für Wakeup bei HIGH oder LOW gesetzt ist.
    pinMode(MPUInterrupt_PIN, PULL_FOR_LOW_OR_HIGH);
    setInterrupt(shakeFaktor, shakeDauer);


    #if PRINTDEBUG

        printRegisterValues({55, 56, 57, 58}); // Beispiel: Register 55, 56, 57 und 58 auslesen


        Serial.println("SETUP ENDE");
    #endif
}


void loop() {
  #if PRINTDEBUG
    if (digitalRead(SLEEPBUTTON) == LOW)
    {
      runterfahren();
    }
    

  #endif

  checkInterruptPin();

  
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Eingabe bis zum Zeilenumbruch lesen
    input.trim(); // Entfernt führende oder nachfolgende Leerzeichen

    // Eingabe überprüfen
    if (input == "x") {
      runterfahren();
    } else {
      Serial.println("Unbekannter Befehl: " + input);
    }
  }


  if(geradeHochgefahren){
    geradeHochgefahren = false;


    fill_solid(leds, NUM_LEDS, CHSV(hueColor, hueSaettigung, hueHelligkeit));
    FastLED.show();
    
    saveWinkel();

  }



  currentMillis = millis(); // speichert die aktuelle Zeit in Millisekunden

  saveWinkel(); // holt die aktuellen Daten. 



  // Überprüft, ob die Zeit seit der letzten Überprüfung größer oder gleich dem Intervall ist
  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis; // setzt die Zeit der letzten Überprüfung auf die aktuelle Zeit


    differenz = letzterWinkel[2]-jetzigerWinkel[2]; // der Unterschied wie sich das Gals auf der Z Achse verdreht hat.

    // TODO genauer die Position vom Glas rausfinden.
    // Welcher Modus ist gerade dran? sprich steht, liegt oder ist das Glas auf dem Kopf.

    if (abs(jetzigerWinkel[0]) < 45 && abs(jetzigerWinkel[1]) < 45 ){
      modus = 2; // Kopfüber Modus

    }else if( abs(jetzigerWinkel[0]) > 150 && abs(jetzigerWinkel[1]) < 45 ){
      modus = 0; // Steht Modus

    }else{
      modus = 1; // Liegt Modus

    }

  // LEDs werden eingestellt und geupdatet.
  setLightStyle(modus, differenz);
  fill_solid(leds, NUM_LEDS, CHSV(hueColor, hueSaettigung, hueHelligkeit));
  FastLED.show();



    // gibt im 1sec Takt die Werte aus.
    count++;
    if (count >= 50 && true){
      count = 0;

      // WERTE DES SENSOR 
    if(false){
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
      // WERTE DES REGISTERS
      printRegisterValues({55, 56, 57, 58}); // Beispiel: Register 55, 56, 57 und 58 auslesen

    }
  
  }


  checkSleep(); // Prüft ob die Lampe aus ist und es in den Tiefschlaf gehen soll.

  saveLetzterWinkel(); // Speichert aus dieser Runde die Winkel weg um diese in der Nächsten Runde vergleichen zu können.

}
