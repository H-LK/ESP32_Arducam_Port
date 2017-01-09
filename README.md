# ESP32 with Arducam Mini 2MP OV2640
This is an early port of the Arduino Arducam-Software for the ESP32 https://github.com/ArduCAM

Currently it only works with the Arducam Mini 2MP OV2640. I doesn't own any other Arducams at the moment.

It is still not 100% stable. Tested with Arduino 1.8.0 .

1. Download and install the ESP32 Arduino library https://github.com/espressif/arduino-esp32
2. Copy the Arducam library into your Arduino library folder (ARDUCAM_ESP32_TCP.ino)
3. Open the Arduinosketch
4. Set your Wifi-Settings
5. Upload the sketch to the ESP32
6. Open a TCP Server on your PC

  for i in {1..1000}; do sudo nc -l 80 | xxd -p -r > ${i}.jpg; done
7. (Re)-Start the ESP32




| Arducam Mini 2MP       | ESP32         |
| ------------- |:-------------:|
| VCC | 5V |
| GND | GND |
| SCL | GPIO 22|
| SDA | GPIO 21|
| CS | GPIO 05|
| SCK | GPIO 18|
| MOSI | GPIO 23|
| MISO | GPIO 19|

Ignore the right side of the picture. It is just a button and a pir sensor which aren't necessary for this code.
![alt tag](wiring.jpg)

Test image. I have to clean my sensor somehow....
![alt tag](test.jpg)
