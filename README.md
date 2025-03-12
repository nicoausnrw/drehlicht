# Shake Glas / Drehlicht v2 2024



Diesmal Wechsel auf größeren ESP mit besseren Deep Sleep



## Modelle die in Frage kommen



### Seeed Studio XIAO ESP32C3

- Vorteil

  - Preis

  - Größe

  - direkter Akku Anschluss

- Nachteile

  - Akku kann nur über Analogpin ausgelesen werden.
  - Angeblich nur 50/100 mAh Ladepower, wenn das stimmt **unbrauchbar**



[Details](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)


# Drehlicht v2 2024

In der neuen Version wird auf einen ESP32 C3 gewechselt um den Deep Sleep zu verbessern. Dabei gibt es im Detail noch weitere verbesserungen. ein MOSFET um die LEDs vom Strom zu nehmen. 
Vorteil ich muss nicht mehr über den REST Pin gehen

WICHTIG: beim Platinen Layout muss für alle DeepSleep wichtigen Pins ein RTC GPIO verwenden, Dazu zählt der Interrupt vom LAgesensor (MPU6050)

### Details zum Deep Sleep Wakeup

This wake-up source allows you to use a pin to wake up the ESP32. The ext0 wake‑up source option uses RTC GPIOs to wake up. So, RTC  peripherals are kept on during deep sleep if this wake-up source is  requested.

To use this wake-up source, you use the following function:

```c
esp_sleep_enable_ext0_wakeup(GPIO_NUM_X, level)
```

This function accepts as the first argument the pin you want to use, in this format GPIO_NUM_X, in which **X** represents the GPIO number of that pin.

The second argument, level, can be either 1 or 0. This represents the state of the GPIO that will trigger wake-up.

[Quelle](https://randomnerdtutorials.com/esp32-external-wake-up-deep-sleep/)

### 201124

Bruno getroffen um alles zusammen zu bauen in der Erstauflage
der c3 kann wohl kein richtiges deep sleep, das versuche ich gerade noch raus zu finden. GGf ist nur lightsleep und über pin wekcen möglich



### 120325

Beim alten Platinen Design habe ich das HIGH Interupt Signal mit einem NPN Transistor invertiert damit ein LOW Signal am, REST pin des 8266 ankommt. Dabei ist es etwas verwunderlich da man eigenltich frei wählen kann was für ein Signal aus dem INT am MPU Kommt.

### Pin Belegung:

- MPU
  - GPIO5 Interrupt
  - GPIO4 SDA
  - GPIO3 SCL

- Akku Spannungsprüfung
  - GPIO0 Spannungsteiler
- MOSFET
  - GPIO7
- LEDs
  - GPIO6

| GPIO   | Analog Function | Comment                    |
| ------ | --------------- | -------------------------- |
| GPIO0  | ADC1_CH0        | RTC                        |
| GPIO1  | ADC1_CH1        | RTC                        |
| GPIO2  | ADC1_CH2        | Strapping pin；RTC         |
| GPIO3  | ADC1_CH3        | RTC                        |
| GPIO4  | ADC1_CH4        | RTC                        |
| GPIO5  | ADC2_CH0        | RTC                        |
| GPIO6  |                 |                            |
| GPIO7  |                 |                            |
| GPIO8  |                 | Strapping pin // BLAUE LED |
| GPIO9  |                 | Strapping pin              |
| GPIO10 |                 |                            |
| GPIO11 |                 |                            |
| GPIO12 |                 | SPI0/1                     |
| GPIO13 |                 | SPI0/1                     |
| GPIO14 |                 | SPI0/1                     |
| GPIO15 |                 | SPI0/1                     |
| GPIO16 |                 | SPI0/1                     |
| GPIO17 |                 | SPI0/1                     |
| GPIO18 |                 | USB-JTAG                   |
| GPIO19 |                 | USB-JTAG                   |
| GPIO20 |                 |                            |
| GPIO21 |                 |                            |

![](esp32c3superminipinout.jpeg)



Pinout / nutzungs Liste
| GPIO | Input   | Output  | Notes                                 |
| ---- | ------- | ------- | ------------------------------------- |
| 0    | OK      | OK      |                                       |
| 1    | OK      | OK      |                                       |
| 2    | OK      | OK      |                                       |
| 3    | OK      | OK      |                                       |
| 4    | OK      | OK      | &nbsp;                                |
| 5    | OK      | OK      |                                       |
| 6    | OK      | OK      |                                       |
| 7    | OK      | OK      |                                       |
| 8    | OK naja | OK naja | BLAUE LED                             |
| 9    | OK      | OK      |                                       |
| 10   | OK      | OK      |                                       |
| 11   | X       | X       | connected to the integrated SPI flash |
| 12   | X       | X       | connected to the integrated SPI flash |
| 13   | X       | X       | connected to the integrated SPI flash |
| 14   | X       | X       | connected to the integrated SPI flash |
| 15   | X       | X       | connected to the integrated SPI flash |
| 16   | X       | X       | connected to the integrated SPI flash |
| 17   | X       | X       | connected to the integrated SPI flash |
| 18   | X       | OK      | &nbsp;                                |
| 19   | X       | OK      | &nbsp;                                |
| 20   | U0RXD   | OK      | &nbsp;                                |
| 21   | OK      | U0TXD   | &nbsp;                                |





# Drehlicht v1 2023

Das Drehlicht ist ein Dekolicht welches Akku betrieben ist.
Dreh man dies auf der Stelle so wird es Heller und Dunkler,
Legt man es auf die Seite kann man durchs drehen um die Symetrieachse die Farbe ändern.
Hält man es Kopfüber kann man die Sättigung einstellen. 

In Planung ist noch das man mit Schütteln das Glas ein und ausschalten kann und hierbei den Deepsleep Modus vom ESP nutzt.

## todo
- Software
  - [x] wie mache ich das am besten mit dem Kaliebrieren?
  - [x] Aufräumen
    - [x] Besser die Lage auslesen (Modus)
  - [x] DeepSleep
    - [ ] Wenn DeepSleep dann letzte Setting speichern.
  - [ ] Akku Reglung, soll es ein Signal geben wenn der Akku leer geht.
  - [ ] nice to have
    - [ ] Serila Monitor OTA
    - [ ] Update OTA
  - [ ] macht uns irgendwann der Software Bug ein Problem?
- [ ] Hardware
  - [x] Glas trüb oder nicht???
  - [x] welcher Akkuregler?
  - [x] LEDs, welche?
  - [x] Platine die Komponenten Hält
  - [x]  Hauptschalter mit einbauen
  - [x] 3D Druck Halterung für alles!



## Material Liste



- LEDs (Ringlicht 24 Neonpixel)
- Wemos D1 mini
- MPU6050
- Akku
- NPN Transistor
- 4,7K Wiederstand
- Akku Halterung
- Glas
- Platine
- 3D Druck
- Ladekabel
- Ladecontroller
- Füllmaterial
- Ladegerät?

ca 35€

## Ablaufplan
- Drehlicht vorführen
- Löteinführung
- Löten der Platine
  - klein Bauteile?
  - Pinleisten auf Platine
  - Pinleiste Microcontroller
  - Akkufach
- Schalter



## noch vorbereiten
Für den Workshop müsste noch **6x** vorbereitet werden:
- LEDring Verkabeln (einheitliche Farben die sich gut unterscheiden)
- Schalter Verkabeln (einheitliche Farben)
- Status LED MPU entfernen
-  3D Druck am besten weiß