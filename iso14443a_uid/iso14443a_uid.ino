/**************************************************************************/
/*! 
    @file     iso14443a_uid.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.   
   
    Note that you need the baud rate to be 115200 because we need to print
	out the data and read from the card at the same time!

This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 
These chips use SPI or I2C to communicate.

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_PN532.h"

// If using the breakout with SPI, define the pins for SPI communication.
//#define PN532_SCK  (2)
//#define PN532_MOSI (3)
//#define PN532_MISO (5)

#define PN532_SS   (9)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (11)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

void setup(void) {
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
  SerialUSB.begin(115200);
  SerialUSB.println("Hello, Maduino Zero NFC! Makerfabs");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    SerialUSB.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  SerialUSB.print("Found chip PN5"); SerialUSB.println((versiondata>>24) & 0xFF, HEX); 
  SerialUSB.print("Firmware ver. "); SerialUSB.print((versiondata>>16) & 0xFF, DEC); 
  SerialUSB.print('.'); SerialUSB.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  SerialUSB.println("Waiting for an ISO14443A card");
}

void loop(void) {
  boolean result_success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  result_success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (result_success) {
    SerialUSB.println("Found a card!");
    SerialUSB.print("UID Length: ");SerialUSB.print(uidLength, DEC);SerialUSB.println(" bytes");
    SerialUSB.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      SerialUSB.print(" 0x");SerialUSB.print(uid[i], HEX); 
    }
    SerialUSB.println("");
    result_success=0;
	// Wait 1 second before continuing
	delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    SerialUSB.println("Timed out waiting for a card");
  }
}
