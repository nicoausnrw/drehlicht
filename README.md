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