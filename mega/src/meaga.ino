// /*
// * Arduino Wireless Communication Tutorial
// *     Example 1 - Transmitter Code
// *
// * by Dejan Nedelkovski, www.HowToMechatronics.com
// *
// * Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
// */
// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>
// RF24 radio(7, 8); // CNS, CE
// const byte address[6] = "00001";
// void setup() {
//   radio.begin();
//   radio.openWritingPipe(address);
//   radio.setPALevel(RF24_PA_MIN);
//   radio.stopListening();
// }
// void loop() {
//   const char text[] = "Nice Data";
//   radio.write(&text, sizeof(text));
//   delay(1000);
// }
//
//
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
int disableLight1 = 40;
int disableLight2 = 41;
int ledState = LOW;
unsigned long previousMillis = 0;

RF24 radio(7, 8); // CNS, CE


// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte axis1;
  byte axis2;
  byte axis3;
  byte axis4;
  byte switches;
};

MyData data;

void setup()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  //radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(pipeOut);

  data.axis1 = 90;
  data.axis2 = 90;
  data.axis3 = 90;
  data.axis4 = 90;
  radio.write(&data, sizeof(MyData));

  pinMode(disableLight1, OUTPUT);
  pinMode(disableLight2, OUTPUT);

  delay(750);
}

/**********************************************/
//DATA SENDING
void loop()
{
  data.axis1 = map( analogRead(8), 0, 1023, 0, 255); //yaw
  data.axis2 = map( analogRead(0), 0, 1023, 0, 255); //throttle
  data.axis3 = map( analogRead(1), 0, 1023, 0, 255); //roll
  data.axis4 = map( analogRead(10), 0, 1023, 0, 255); //pitch
  data.switches = !digitalRead(22); //robot enable switch
  radio.write(&data, sizeof(MyData));
//Enable Flasher
  unsigned long currentMillis = millis();
  if (!data.switches){
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(disableLight1, ledState);
      digitalWrite(disableLight2, !ledState);
    }
  }else{
    digitalWrite(disableLight1, LOW);
    digitalWrite(disableLight2, LOW);
  }

}
