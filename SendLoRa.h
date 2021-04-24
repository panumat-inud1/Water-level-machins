// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

// ESP32  |  Arduino |              LoRa                  |
//--------|----------|------------------------------------|
// GPIO5  |    13    |   SX1278's SCK                     |
// GPIO19 |    12    |   SX1278's MISO                    |
// GPIO27 |    11    |   SX1278's MOSI                    |
// GPIO18 |    10    |   SX1278's CS                      |
// GPIO14 |    9     |   SX1278's RESET                   |
// GPIO26 |    2     |   SX1278's IRQ(Interrupt Request)  |
//--------------------------------------------------------|

#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

#if defined(ESP32)
#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DI0     26
#else
#define SS      10
#define RST     9
#define DI0     2
#endif

#define BAND    433E6

#define gateway  0xFF

uint8_t dataReceived[8];
int Rssi;

void onReceive(int packetSize);
void setupLora() {


#if defined(ESP32)
  SPI.begin(SCK, MISO, MOSI, SS);
  //  SPI.begin();
#else
  SPI.begin();
#endif
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    // while (1);
  }
//    LoRa.onReceive(onReceive);
//    LoRa.receive();

  Serial.println("LoRa Initial OK!");
}

void SendLora(int value,int ctrl, byte addr, byte client) {
  delay(50);
  byte byteSend[8];

  byteSend[0] = client;
  byteSend[1] = client;
  byteSend[2] = addr;
  byteSend[3] = (byte)(value >> 8);
  byteSend[4] = (byte)(value & 0xFF);
  byteSend[5] = (byte)(ctrl >> 8);
  byteSend[6] = (byte)(ctrl & 0xFF);
  int checkSum = ~(byteSend[1] + byteSend[2] + byteSend[3] +
                        byteSend[4] + byteSend[5] + byteSend[6]);
  byteSend[7] = (byte)(checkSum & 0xFF);

  delay(10);

  LoRa.beginPacket();
  Serial.print("Send >> ");
  //  for (int i = 0; i < 8; i++) {
  LoRa.write(byteSend[0]); Serial.print(byteSend[0], HEX); Serial.print(" ");
  LoRa.write(byteSend[1]); Serial.print(byteSend[1], HEX); Serial.print(" ");
  LoRa.write(byteSend[2]); Serial.print(byteSend[2], HEX); Serial.print(" ");
  LoRa.write(byteSend[3]); Serial.print(byteSend[3], HEX); Serial.print(" ");
  LoRa.write(byteSend[4]); Serial.print(byteSend[4], HEX); Serial.print(" ");
  LoRa.write(byteSend[5]); Serial.print(byteSend[5], HEX); Serial.print(" ");
  LoRa.write(byteSend[6]); Serial.print(byteSend[6], HEX); Serial.print(" ");
  LoRa.write(byteSend[7]); Serial.print(byteSend[7], HEX);

  //  }
  Serial.print('\n');
  LoRa.endPacket();


}

void onReceive(int packetSize) {
  //  Serial.println(packetSize);
  //  LoRa.begin(BAND);
  if (packetSize == 0) return;

  Rssi = LoRa.packetRssi();
  if (Rssi < -110) {
    Serial.println("error: E01");
    return;
  } else Serial.print("Received packet '");

  for (int i = 0; i < packetSize; i++) {
    dataReceived[i] = LoRa.read();
    Serial.print(dataReceived[i], HEX);
    Serial.print(" ");
    delay(10);
  }

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());

  if(dataReceived[0] & 0xFF && dataReceived[1] & 0xFF != 0xAA){
    Serial.println("Header invalid.");
    return;
  }
  if (dataReceived[2] != 0x01) {
    Serial.println(F("Address ID invalid"));
    return;
  }
  int sumCheck = ~(dataReceived[1] + dataReceived[2] + dataReceived[3] +
                   dataReceived[4] + dataReceived[5] + dataReceived[6]);
  Serial.println(sumCheck & 0xFF, HEX);
  Serial.println(dataReceived[7],HEX);
  if (dataReceived[7] != (byte)(sumCheck)) {
    Serial.println(F("This message is not for me."));
    return;
  }
  
  reg = (dataReceived[3] << 8 | dataReceived[4]);
  Serial.println("Value >> " + String(reg));
  ctrl = (dataReceived[5] << 8 | dataReceived[6]);
  Serial.println("Ctrl >> " + String(ctrl));
//  return value;
  
}

//bool check_lora(String msg) {
//  String Received = "", key = "";
//  bool flg = false, flg_loop = true;
//  long time_check = 0, count_lora = 0;
//
//  SendLora("<" + msg + ">");
//  //  delay(500);
//  while (1) {
//    //    Serial.println("lora");
//    int packetSize = LoRa.parsePacket();
//    //    Serial.println("Size : " + String(packetSize));
//    if (packetSize) {
//      Serial.println("RSSI : " + (String)LoRa.packetRssi());
//      Serial.println("Size : " + String(packetSize));
//
//      while (LoRa.available() > 0) {
//        char c = LoRa.read();
//
//        if ((int(c) >= 48 && int(c) <= 57) ||
//            (int(c) >= 97 && int(c) <= 121) ||
//            (int(c) >= 65 && int(c) <= 90) ||
//            int(c) == 46 || int(c) == 44 ||
//            int(c) == 32 || int(c) == 95 || int(c) == 47) {
//          Received += c;
//
//          //          Serial.println(Received);
//        }
//        if (c == '!') {
//          key = Received;
//          Received = "";
//          flg_loop = false;
//          break;
//        }
//      }
//      if (key == msg) {
//        flg = true;
//        Serial.println(key);
//        break;
//        return;
//      }
//    }
//    else if (packetSize == 0) {
//      count_lora++;
//      if (count_lora == 1000) {
//        Serial.println("return");
//        count_lora = 0;
//        flg = false;
//        break;
//      }
//      delay(1);
//      //      Serial.println(count_lora);
//    }
//  }
//  return flg;
//}
