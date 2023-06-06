#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct MyData {
  byte axis1;
  byte axis2;
  byte axis3;
  byte axis4;
};

MyData data;

void setup()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(pipeOut);

  data.axis1 = 50;
  data.axis2 = 50;
  data.axis3 = 50;
  data.axis4 = 50;
  radio.write(&data, sizeof(MyData));

  delay(750);
}

/**********************************************/

void loop()
{  
  data.axis1 = map( analogRead(3), 0, 1023, 0, 100); //yaw
  data.axis2 = map( analogRead(0), 0, 1023, 100, 0); //throttle
  data.axis3 = map( analogRead(1), 0, 1023, 0, 100); //roll
  data.axis4 = map( analogRead(2), 0, 1023, 100, 0); //pitch

  radio.write(&data, sizeof(MyData));
}









#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn =  0xE8E8F0F0E1LL;

RF24 radio(9, 10);
Servo throttleServo;
Servo steeringServo;

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

  radio.openReadingPipe(1,pipeIn);
  radio.startListening();

  throttleServo.attach(5);
  steeringServo.attach(3);
    
  data.axis1 = 127;
  data.axis2 = 127;
  data.axis3 = 127;
  data.axis4 = 127;
  data.switches = 0;
  
  delay(750);
}

/**********************************************/

unsigned long lastRecvTime = 0;

void loop()
{ 
  while ( radio.available() ) {        
    radio.read(&data, sizeof(MyData));
    lastRecvTime = millis();
  }
  
  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    data.axis2 = 127;
    data.axis3 = 127;
  }
  
  // manual control
  byte s = map(data.axis2, 0, 255, 40, 140);
  byte t = map(data.axis3, 0, 255, 140, 40);
  
  if ( (data.switches & 0x01) ) {
    s = 90;
    t = 90;
  }
  
  steeringServo.write( s );
  throttleServo.write( t );
}
