/*
 *  09.01.2017
 *  Hendrik Linka
 *  Hochschule Bonn-Rhein-Sieg
 *
 */

#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

const char* ssid     = "******";
const char* password = "******";

const char* host = "192.168.*.*";
const int httpPort = 80;

const int CS = 5;

ArduCAM myCAM(OV2640, CS);
WiFiClient client;

void setup()
{
    uint8_t vid, pid;
    uint8_t temp;
    Wire.begin();
    pinMode(CS, OUTPUT);
    // initialize SPI:
    SPI.begin();
    SPI.setFrequency(4000000); //4MHz
    Serial.begin(115200);

    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(0x00, 0x55);
    temp = myCAM.read_reg(0x00);
    if (temp != 0x55){
      Serial.println("SPI1 interface Error!");
      while(1);
    }
  
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
      Serial.println("Can't find OV2640 module!");
    else
      Serial.println("OV2640 detected.");   
  
    //Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
    myCAM.clear_fifo_flag();    
}

void loop()
{  
    WiFi.begin(ssid, password);
    //client.setNoDelay(true);
    capture();
    delay(1000);
}

void capture(){
    uint8_t temp = 0;
    uint8_t temp_last = 0;
    String data = "";  
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);        
    }
    Serial.println("Connected");
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
    myCAM.clear_fifo_flag();
    delay(10);
    //Flush the FIFO
    myCAM.flush_fifo();
    //Clear the capture done flag
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    Serial.println("Start Capture");
    while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
    Serial.println("Capture Done!");
       
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        delay(10);
        return;
    }      

    size_t len = myCAM.read_fifo_length();
    //Serial.println(len);
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    SPI.transfer(0xFF);   
    //Read JPEG data from FIFO
    while ( (temp !=0xD9) | (temp_last !=0xFF)){      
      temp_last = temp;
      temp = SPI.transfer(0x00);
      if(temp<0x10){
        data += 0;
      }
      data += String(temp, HEX);    
      if(data.length()>500){
        client.print(data);
        data = "";
      }
    }
    if(data.length()!=0){
      client.print(data);
    }
    myCAM.CS_HIGH();    
    client.stop();  
}

