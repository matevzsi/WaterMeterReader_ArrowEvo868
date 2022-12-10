// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 868.95

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0)
  // Feather M0 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
  #define LED           13
#endif

#if defined(ESP8266)    // ESP8266 feather w/wing
  #define RFM69_CS      2    // "E"
  #define RFM69_IRQ     15   // "B"
  #define RFM69_RST     16   // "D"
  #define LED           0
#endif

#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_FEATHER_SENSE)
  #define RFM69_INT     9  // "A"
  #define RFM69_CS      10  // "B"
  #define RFM69_RST     11  // "C"
  #define LED           13

#elif defined(ESP32)    // ESP32 feather w/wing
  #define RFM69_RST     13   // same as LED
  #define RFM69_CS      33   // "B"
  #define RFM69_INT     27   // "A"
  #define LED           13
#endif

#if defined(ARDUINO_NRF52832_FEATHER)
  /* nRF52832 feather w/wing */
  #define RFM69_RST     7   // "A"
  #define RFM69_CS      11   // "B"
  #define RFM69_INT     31   // "C"
  #define LED           17
#endif

/* Teensy 3.x w/wing
#define RFM69_RST     9   // "A"
#define RFM69_CS      10   // "B"
#define RFM69_IRQ     4    // "C"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/
 
/* WICED Feather w/wing 
#define RFM69_RST     PA4     // "A"
#define RFM69_CS      PB4     // "B"
#define RFM69_IRQ     PA15    // "C"
#define RFM69_IRQN    RFM69_IRQ
*/

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

float freq = RF69_FREQ;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
    
  if (!rf69.init()) {    
    Serial.println("RFM69 radio init failed");
    while (1);
  }

/*
  char tmp[50] = {0};
  for (int addr = 0; addr < 0x70; addr++) {
    sprintf(tmp, "%02X: %02X", addr, rf69.spiRead(addr));
    Serial.println(tmp);
  }  
*/
  Serial.println("RFM69 radio init OK!");

  rf69.setModeRx();
}

uint8_t outData[64] = {0};

uint8_t decodeTable[] = {
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 3, 255, 1, 2, 255, 
  255, 255, 255, 7, 255, 255, 0, 255, 255, 5, 6, 255, 4, 255, 255, 255, 255, 
  255, 255, 11, 255, 9, 10, 255, 255, 15, 255, 255, 8, 255, 255, 255, 255, 13, 
  14, 255, 12, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

// Decode message using 6-to-4 bit (3 out of 6) encoding (as specified in EN13757-4 / 5.4.1)
void decodeMsg(uint8_t * msg, int msgLen) { 
  uint16_t tmpBuffer = 0;
  int bufferLen = 0;
  uint16_t mask = 0;
  int errorCnt = 0;
  int tmpNibble = 0;
  int nibblePtr = 0;
  int dataLeft = msgLen;

  char tmpc[64];

  while(dataLeft--) {
    /*
    // Print out the tmpBuffer
    sprintf(tmpc, "Fill data [l=%d] with %02X ", bufferLen, *msg);
    Serial.print(tmpc);    
    
    for (int b = 15; b >= 0; b--) {
      Serial.print((tmpBuffer & (1<<b)) ? '1' : '0');
    }
    Serial.print(" > ");
    */
    mask = 0xFFFF >> bufferLen;
    tmpBuffer &= ~mask;
    tmpBuffer |= (uint16_t)(*msg++) << (8 - bufferLen); // Put the new data into the tmpBuffer
    bufferLen += 8;
    /*
    for (int b = 15; b >= 0; b--) {
      Serial.print((tmpBuffer & (1<<b)) ? '1' : '0');
    }
    */

    while (bufferLen >= 6) {
      // Print out the tmpBuffer
      /*
      Serial.print("tmpBuffer: ");
      for (int b = 15; b >= 0; b--) {
        Serial.print((tmpBuffer & (1<<b)) ? '1' : '0');
      }
      Serial.println("");
      */

      // Take out upper 6 bits and decode them
      uint8_t dec = decodeTable[tmpBuffer >> 10];
      if (dec < 255) {
        if ((nibblePtr % 2) == 0) {
          // Upper nibble
          outData[nibblePtr / 2] = dec << 4;
        } else {
          // Lower nibble
          outData[nibblePtr / 2] |= dec;
        }
      } else {
        errorCnt++;
      }
      nibblePtr++;

      // Shift the tmpBuffer up for 6 bits
      tmpBuffer <<= 6;
      bufferLen -= 6;
    }
  }

  Serial.print(" decoded data, errors=");
  Serial.print(errorCnt);
  Serial.print(": ");
  for (int i = 0; i < msgLen * 4 / 6; i++) {    
    sprintf(tmpc, "%02X ", outData[i]);
    Serial.print(tmpc);
  }  
}

void loop() {
  //rf69.spiWrite(0x23, 1);

  digitalWrite(LED,HIGH);
    delay(100);

  //Serial.println(rf69.spiRead(0x24) / 2.0);
    digitalWrite(LED,LOW);
    delay(100);

 if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      /*
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");

      for (int i = 0; i < len; i++) {
        char tmp[4];
        sprintf(tmp, "%02X ", buf[i]);
        Serial.print(tmp);
      }
      */
      //Serial.println((char*)buf);
      Serial.print("\r\nRSSI: ");
      Serial.print(rf69.lastRssi(), DEC);      
      decodeMsg(buf, len);
    } else {
      Serial.println("Receive failed");
    }
  }
}


void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
