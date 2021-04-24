int counter = 0; //This variable will increase or decrease depending on the rotation of encoder
int tem, distanc, lastDis; // tem เก็บ counter
int reg, ctrl;
//bool flg_sen = false, first = true;
//long t_sen = 0, t_lora = 0;
int A = 33 , B = 32; // ขาของเซ็นเซอร
long wide = 4600 , longs = 3500 , high = 600 , sum;
long area;

#define led_stat  2
long t_first = 0, t_blink = 0;
int blynk_delay = 1000;
//******************************************************
boolean ActiveHigh = true; //Set Active Low/Hight Relay
//******************************************************

//#define pinValve  34
int wdcCount = 0;
int rsCount = 120;

#include "SendLoRa.h"
#include <Ticker.h>
Ticker secondTick;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_stat, OUTPUT);
  pinMode(A, INPUT_PULLUP); // กำหนดให้เป็น pullup input
  pinMode(B, INPUT_PULLUP); // กำหนดให้เป็น pullup input
  //  pinMode(pinValve, INPUT_PULLUP);
  setupLora();

  //Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(A, ai0, RISING);
  //B rising pulse from encodenren activated ai1(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.
  attachInterrupt(B, ai1, RISING);
  secondTick.attach(1, ISRWDC);
}

void loop() {
  // put your main code here, to run repeatedly:
  onReceive(LoRa.parsePacket());
  
  if ((millis() - t_blink) >= blynk_delay) {
    digitalWrite(led_stat, ActiveHigh);
    t_blink = millis();
    ActiveHigh = !ActiveHigh;
  }
  
  distanc = tem / 70;
  if (distanc < 0) distanc = 0;

  //  sum = high - (long)distanc;
  //  area = (wide * longs * sum)/1000;
  //
  //  long liter = area / 1000;
  //  int volume = liter / 1000;

  if (reg == 1022) {
    SendLora(reg, distanc, 0x02, gateway);
    reg = 0;
  }

  //  if (digitalRead(pinValve) == LOW) {
  //    if (millis() - t_sen >= 500 && flg_sen == false || first == true) {
  ////      sensor = digitalRead(pinValve);
  //      Serial.println(F("วาล์วเปิด"));
  //      SendLora(2000, 2001, 0x02, gateway);
  //      flg_sen = true;
  //      first = false;
  //      t_sen = millis();
  //    }
  //  } else if(digitalRead(pinValve) == HIGH){
  //    if (millis() - t_sen >= 500  && flg_sen == true || first == true) {
  ////      sensor = digitalRead(pinValve);
  //      Serial.println(F("วาล์วปิด"));
  //      SendLora(2000, 2000, 0x02, gateway);
  //      flg_sen = false;
  //      first = false;
  //      t_sen = millis();
  //    }
  //  }
  //
  //  if (reg == 2000) {
  //    if (digitalRead(pinValve) == LOW) {
  //      Serial.println(F("วาล์วเปิด"));
  //      SendLora(2000, 2001, 0x02, gateway);
  //    } else {
  //      Serial.println(F("วาล์วปิด"));
  //      SendLora(2000, 2000, 0x02, gateway);
  //    }
  //    reg = 0;
  //  }
  //  if (reg == 3000) {
  //    if (ctrl == 3001) {
  //      Serial.println("System ON");
  //      SendLora(reg, ctrl, 0x02, gateway);
  //    } else if (ctrl == 3000) {
  //      Serial.println("System OFF");
  //      SendLora(reg, ctrl, 0x02, gateway);
  //    }
  //    reg = 0;
  //  }

  //  Serial.print ("sum :  ");
  //  Serial.print (sum);
  //  Serial.print ("  volume :  ");
  //  Serial.print(area);
  if (distanc != lastDis) {
    Serial.print("distance: ");
    Serial.print(distanc);
    Serial.println();
    lastDis = distanc;
  }
  //  SendLora(300, 0x02);
  //  delay(1000);
}



void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if (digitalRead(B) == LOW) {
    counter++;
    tem = counter;
  } else {
    counter--;
    tem = counter;
  }

  secondTick.attach(1, ISRWDC);
}

void ai1() {
  // ai0 is activated if DigitalPin nr 3 is going from LOW to HIGH
  // Check with pin 2 to determine the direction
  if (digitalRead(A) == LOW) {
    counter--;

    if (counter < 0) counter = 0;
    else tem = counter;

  } else {
    counter++;
    tem = counter;

  }
}

void ISRWDC() {
  wdcCount++;
  if (wdcCount > rsCount) {
    Serial.println("Rebooting!!!");
    system_restart();
    //    resetFunc();
    //    ESP.reset();
  }
}
