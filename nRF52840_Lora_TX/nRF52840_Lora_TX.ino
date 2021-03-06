/**
 * nRF52840_Lora_TX.ino
 * @date July-07-2022
 * @author Nakseung Choi
 * @version V1.0 = simple implementation for RFM9x using ItsBitsy nRF52840 express. (This is a TX version.)
 * @version V2.0 = implementation SCD41 (CO2 sensor) on the top of V1.0.
 * @brief A Simple sending/receiving data using Lora Radio (RFM9x) with ItsBitsy nRF52840 express.
 * SCD41 is going to be implemented to measure CO2 data. This data is going to be sent to RX. 
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// for ItsyBitsy nRF52840 express
#define RFM95_RST 10 //direct PIN numbers
#define RFM95_CS 11
#define RFM95_INT 12

// This frequency must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  delay(100);

  Serial.println("nRF52840 LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission
/**
 * void loop()
 * @param itoa  This function is a stdlib C library function that converts a number into string. 
 * @param itoa  (things to send, buff to store, 10 or 16) 8 for octal 10 for decial and 16 for hex value.
 * @param RSSI  RSSI (Received Signal Strength Indicator) is a measurement of how well your device can hear a signal from an access point.
 */
void loop()
{
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting..."); // Send a message to rf95_server
  
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);    // Typecasted into unsigned 8-bit integer.  

  Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // buff to store data from RX
  uint8_t len = sizeof(buf);            

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);     // typecasted into char pointer to print out.
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }

}
