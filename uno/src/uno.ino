#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn =  0xE8E8F0F0E1LL;

RF24 radio(7, 8);

Servo throttleServo;
Servo steeringServo;
Servo extraDevice;
Servo extraDevice2;

int rsl = 2;
int ledState = LOW;
int flashrate = 2000;
unsigned long previousMillis = 0;


// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte axis1;
  byte axis2;
  byte axis3;
  byte axis4;
  byte switches;
};

MyData data;

/**********************************************/

void setup()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  //radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1,pipeIn);
  radio.startListening();

  throttleServo.attach(5);
  steeringServo.attach(3);
  extraDevice.attach(6);
  extraDevice2.attach(9);

  pinMode(rsl, OUTPUT);

  data.axis1 = 90;
  data.axis2 = 90;
  data.axis3 = 90;
  data.axis4 = 90;
  data.switches = 0;
  delay(750);
}

/**********************************************/

unsigned long lastRecvTime = 0;

void loop()
{
  if ( radio.available() ) {
    radio.read(&data, sizeof(MyData));
    lastRecvTime = millis();
  }

  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    data.axis1 = 127;
    data.axis2 = 127;
    data.axis3 = 127;
    data.axis4 = 127;
    flashrate = 2000;
  }

  // manual control
  byte x = map(data.axis1, 0, 255, 0, 180);
  byte s = map(data.axis2, 0, 255, 0, 180);
  byte t = map(data.axis3, 0, 255, 0, 180);
  byte y = map(data.axis4, 0, 255, 0, 180);

  if ( (data.switches & 0x01) ) {
    s = 90;
    t = 90;
    x = 90;
    y = 90;
    flashrate = 200;
  }

  steeringServo.write( s );
  throttleServo.write( t );
  extraDevice.write( x );
  extraDevice2.write( y );

  //RSL Flasher
  unsigned long currentMillis = millis();
  if (!data.switches){
    if (currentMillis - previousMillis >= flashrate) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(rsl, ledState);
    }
  }else{
    digitalWrite(rsl, HIGH);
  }
}
