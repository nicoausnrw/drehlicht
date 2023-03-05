// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Arduino.h"
// Basic demo for accelerometer readings from Adafruit MPU6050

#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  //mpu6050.calcGyroOffsets(true);
}

void loop() {
  mpu6050.update();
  Serial.print("angleX : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(mpu6050.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(mpu6050.getAngleZ());
}


#################################################################################################################################################################
#################################################################################################################################################################
#################################################################################################################################################################
#################################################################################################################################################################

// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Arduino.h"
// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


#include <FastLED.h>
#define LED_PIN 14
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];


Adafruit_MPU6050 mpu;










float testWert = 0;
float testWertZuvor = 0;
int hue = 0;
int faktor = 5;
int modus = 0; // 0 = Glas steht, 1 = Glas liegt. 
int modusZuvor = 0; // Wird gebraucht um den wechsel zwischen den Modis zu regestrieren. 




void setup(void) {

  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);





// TEST SETUP LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 0, 240); // Set color to warm white
  }
  FastLED.show();
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);



  testWert = a.acceleration.x;
     // Print out the values 
  Serial.print(a.orientation.x);
  Serial.print(", ");
  Serial.print(a.current.x);
  Serial.print(", ");
  Serial.println(testWert);
/*  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print(", ");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");
*/

// Pürfenn ob das Glas steht oder liegt. 
if(a.acceleration.x > -4.0){
  // Das Gals wird jetzt Seitlich gehalten. 
  modus = 1; // liegemodus Speichern

  if (modusZuvor == 0){
    // sind das erste mal im Moduswechsel gelandet, daher  muss ich die  z & y Achse hier relativ auf Null setzen und von hier aus weiter berechnen welche Farbe dran kommt.

    modusZuvor = 1;

  // Funktion Winkelrechner. Einbauen


  }



}


  if(testWert-testWertZuvor > 0.25){
    hue = hue+faktor;
  }else if(testWert-testWertZuvor < -0.25){
    hue = hue-faktor;

  }

  // Set all LEDs to the current hue value
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
  //  Serial.println(hue);
  FastLED.show();


  testWertZuvor = testWert;

  delay(100);

  /*
  okay, Folgende Pläne, ich muss auf jeden Fall das Delay raus arbeiten und eventuell als kleinen schutz rein packen das nur wenn das Gyroskop ausschlägt eine Freigabe erteilt ist etwas an den Farbwerten zu ändern.

  Ich sollte bevor ich hier weiter machen mich auf eine gute Bib festlegen und verstehen wie ich die 360° auf diesen Sensor aufteile.
  die eine Bib light ist eventuell hilfreich mit der Winkelmessung.  Dann sollte ich weiter überlegen ob ich samrte Funktionen bauen kann um die Sensoeren abzufragen da ja alle drei achsen ähnliche behandelt werden. (bzw zwei) 
  und was ist wenn ich den Sensor schräg einbauen?
 
  
  
  
  
  */
}