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

  Serial.println("\r\nInitializing radio...");  

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


char tmpc[64];


uint8_t LUT[] = { 0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0, 
                  0x17,	0x2F,	0x49,	0xAB,	0x21,	0xD3,	0xD9,	0xDB,	0x19,	0x2, 
                  0x2E,	0x5E,	0x93,	0x56,	0x43,	0xA7,	0xB3,	0xB6,	0x32,	0x5, 
                  0x39,	0x71,	0xDA,	0xFD,	0x62,	0x74,	0x6A,	0x6D,	0x2B,	0x7, 
                  0x5C,	0xBD,	0x26,	0xAC,	0x87,	0x4F,	0x67,	0x6C,	0x64,	0xA, 
                  0x4B,	0x92,	0x6F,	0x7,	0xA6,	0x9C,	0xBE,	0xB7,	0x7D,	0x8, 
                  0x72,	0xE3,	0xB5,	0xFA,	0xC4,	0xE8,	0xD4,	0xDA,	0x56,	0xF, 
                  0x65,	0xCC,	0xFC,	0x51,	0xE5,	0x3B,	0xD,	0x1,	0x4F,	0xD }; 

uint8_t deltaCodes[] = { 0x12, 0xB5, 0xE2, 0x75, 0x12, 0x2B, 0x72, 0x02, 0xDE, 0, 0, 
                         0x11, 0x71, 0xD9, 0x6A, 0x7F, 0xDB, 0xBC, 0x4A, 0x6D, 0, 0, 
                         0x10, 0x2F, 0x48, 0xD9, 0xD5, 0x49, 0x6B, 0xC6, 0x24, 0, 0,
                         0 };

void parseMADA2Frame(uint8_t * data) {
  uint8_t deltaCode[10] = {0};
  uint8_t ci = 0;
  uint8_t decData[10] = {0};

  uint8_t frameCounter = data[0];

  // Prepare the XOR sum of delta codes known until now
  while (deltaCodes[ci*11] != 0) {
    for (int i = 0; i < 10; i++) 
      deltaCode[i] ^= deltaCodes[ci*11 + i + 1];
    if (deltaCodes[ci*11] == data[1]) break;
    ci++;
  }
  if (deltaCodes[ci*11] == 0) {
    Serial.print("Error: Unknown delta code");
    return;
  }

  frameCounter = (frameCounter >> 4) - 8;
  for (int i = 0; i < 10; i++) {
    decData[i] = data[5 + i] ^ LUT[frameCounter * 10 + i] ^ deltaCode[i];
  }

  /*
  Serial.print(" Data: ");
  for (int i = 0; i < 10; i++) {
    sprintf(tmpc, "%02X ", decData[i]);
    Serial.print(tmpc);
  }
  */

  uint32_t * meterValue = (uint32_t*)(decData);
  uint32_t * meterValueCapture = (uint32_t*)(decData + 4);
  uint16_t * dateCode = (uint16_t*)(decData + 8);

  sprintf(tmpc, "Meter: %ld.%02d m3", *meterValue / 100, (*meterValue) % 100);
  Serial.print(tmpc);

  sprintf(tmpc, " (captured %ld.%02d m3)", *meterValueCapture / 100, (*meterValueCapture) % 100);
  Serial.print(tmpc);

  sprintf(tmpc, " date: %04X", *dateCode);
  Serial.print(tmpc);
}


uint8_t outData[64] = {0};

uint8_t decodeTable[] = {
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 3, 255, 1, 2, 255, 
  255, 255, 255, 7, 255, 255, 0, 255, 255, 5, 6, 255, 4, 255, 255, 255, 255, 
  255, 255, 11, 255, 9, 10, 255, 255, 15, 255, 255, 8, 255, 255, 255, 255, 13, 
  14, 255, 12, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

//#define IGNORE_ERRORS 1

// Decode message using 6-to-4 bit (3 out of 6) encoding (as specified in EN13757-4 / 5.4.1)
uint8_t decodeMsg(uint8_t * msg, int msgLen, int RSSI) { 
  uint16_t tmpBuffer = 0;
  int bufferLen = 0;
  uint16_t mask = 0;
  int errorCnt = 0;
  int tmpNibble = 0;
  int nibblePtr = 0;
  int dataLeft = msgLen;

  while(dataLeft--) {   
    mask = 0xFFFF >> bufferLen;
    tmpBuffer &= ~mask;
    tmpBuffer |= (uint16_t)(*msg++) << (8 - bufferLen); // Put the new data into the tmpBuffer
    bufferLen += 8;

    while (bufferLen >= 6) {
      // Take out upper 6 bits and decode them
      uint8_t dec = decodeTable[tmpBuffer >> 10];
      if (dec > 15) {
        dec = 0;
        errorCnt++;
      }

      if ((nibblePtr % 2) == 0) {
        // Upper nibble
        outData[nibblePtr / 2] = dec << 4;
      } else {
        // Lower nibble
        outData[nibblePtr / 2] |= dec;
      }     
      nibblePtr++; 

      // Shift the tmpBuffer up for 6 bits
      tmpBuffer <<= 6;
      bufferLen -= 6;
    }
  }

#ifndef IGNORE_ERRORS
  if (errorCnt == 0) {  
#endif
    sprintf(tmpc, "Frame [%d dB] ", RSSI);
    Serial.print(tmpc);
#ifdef IGNORE_ERRORS
    Serial.print(" (");
    Serial.print(errorCnt);
    Serial.print(" errors)");
#endif
    Serial.print(": ");
    for (int i = 0; i < msgLen * 4 / 6; i++) {    
      sprintf(tmpc, "%02X ", outData[i]);
      Serial.print(tmpc);
    }  
#ifndef IGNORE_ERRORS
  }
#endif
  return errorCnt;
}

void loop() {

 if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
          
      int8_t RSSI = rf69.lastRssi();
      //Serial.print("\r\nRSSI: ");
      //Serial.print(rf69.lastRssi(), DEC);      
      if (decodeMsg(buf, len, RSSI) == 0) {
        digitalWrite(LED,HIGH);

        // Something received and successfully decoded...
        uint8_t L = outData[0];
        uint8_t C = outData[1];

        if (L == 0x19 && C == 0x44) {
          Serial.print(" T1");

          Serial.print(" ['");
          uint16_t Mf = (outData[3] << 8) + outData[2];
          char M[4];
          M[0] = 'A' + (Mf >> 10) - 1;
          M[1] = 'A' + ((Mf >> 5) & 31) - 1;
          M[2] = 'A' + (Mf & 31) - 1;
          M[3] = 0;
          Serial.print(M);          
          sprintf(tmpc, "' %02X%02X%02X%02X%02X%02X]", outData[4], outData[5], outData[6], outData[7], outData[8], outData[9]);
          Serial.print(tmpc);

          if (outData[4] == 0x81 && outData[5] == 0x07) {
            //sprintf(tmpc, " => Arrow Evo 868 serial=%02X%02X%02X%02X CRC=%02X%02X", outData[9], outData[8], outData[7], outData[6], outData[10], outData[11]);
            //Serial.print(tmpc);        

            switch (outData[12]) {
              case 0xA2:
                Serial.print(" A2 data: ");
                parseMADA2Frame(outData + 13);
                break;

              case 0x51:
                Serial.print(" Readout data (EN13757-4)");
                break;
              case 0x71:
                Serial.print(" Alarm report (EN13757-4)");
                break;
              case 0x72:
                Serial.print(" Application layer, full header (EN13757-4)");
                break;
              case 0x78:
                Serial.print(" Application layer, no header (EN13757-4)");
                break;
              case 0x7A:
                Serial.print(" Application layer, short header (EN13757-4)");
                break;
              case 0x82:
                Serial.print(" Reserved for future use (EN13757-4)");
                break;

              default:
                sprintf(tmpc, "Unknown CI value %02X", outData[12]);
                Serial.print(tmpc);
            }
          }  

          Serial.println("");      
        }
        delay(100);
        digitalWrite(LED,LOW);
      }
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
