#include <Arduino.h>
#include <Wire.h>
#include "Accel_tilt.h"

#define INT 5
#define SLAVE_ADR 0x15
#define shakeInt 0x00
#define dataInt 0x01 //bit 7 - Tilt, bit 6 - z orientation+-, bit 5 - xy orientation bit 1 --> bit 4 xy orientation bit 0, bit 0, data ready
#define orient 0x02 
#define Xout 0x03 //goes to 0x04 upper at 0x03
#define Yout 0x05 // 2 bytes, upper at 0x05
#define Zout 0x07 // 2 bytes, upper at 0x07
#define Tout 0x09 //1 byte only
#define shakeIntSet 0x0A
#define dataAvailSet 0x0B
#define SDA 35
#define SCL 36

void getData(void);

#define FSR8 0b01000000 //8g operation
#define FSR2 0b00000000 //2g operation
#define FSR8_SCALE 256.0 //8g operation scaling factor
#define FSR2_SCALE 1024.0 //2g operation scaling factor

//storage for accelerometer
double ZVal = 0;
double avgZ = 0;
double prevZ = 0;
double Zoffset = 0;
int moveCount = 0;
int pickup = 0;

void setup()
{
    // Start I2C transmission and serial comms
    delay(400);

    Wire.begin(SDA,SCL);
    Serial.begin(115200);

    // Initialise the accelerometer interrupts to 0
    Wire.beginTransmission(SLAVE_ADR);
    Wire.write(shakeInt);
    Wire.write(0);
    Wire.endTransmission();

    // Initialise the accelerometer data collects to 0
    Wire.beginTransmission(SLAVE_ADR);
    Wire.write(dataInt);
    Wire.write(0);
    Wire.endTransmission();

    //configure range of FSRs
    Wire.beginTransmission(SLAVE_ADR); 
    Wire.write(0x0D);
    Wire.write(FSR8);
    Wire.endTransmission();

    // disable temperature compensation 0x80 else 0x00
    Wire.beginTransmission(SLAVE_ADR); 
    Wire.write(0x0B);
    Wire.write(0x00);
    Wire.endTransmission();

    //configure shake interrupts -- not used
    Wire.beginTransmission(SLAVE_ADR);
    Wire.write(0x0C);
    Wire.write(0b10001000);
    Wire.endTransmission();

    //calculate the starting orientation of the accelerometer
    for (int counter = 0;counter<100;counter++){
        getData();
        delay(1);
        avgZ += ZVal*0.01; //equal avegrage of 100 values
    }

    //add offset based on assumption that +z axis is up and avgZ should be 1g due to gravity
    Zoffset = 1-(avgZ/FSR8_SCALE);
}

void loop()
{
    
    // fetch data from accelerometer via I2C
    for (int i = 0; i<20;i++){
        prevZ = ZVal;
        getData();
        if(abs(prevZ-ZVal)>=1)
        {
            moveCount += 1;
        }
        delay(5);
    }

    if (moveCount>2)
    {
        Serial.println("Pickup detected");
        moveCount = 0;
    }
}

void getData(){ // Retreives data from the accelerometer via I2C
  Wire.beginTransmission(SLAVE_ADR);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADR, 10);
  Wire.read();
  int dataByte = Wire.read();

  
  if (bitRead(dataByte, 0)){ // Check whether new data is available from the accelerometer
    int orientByte = Wire.read();

    int Byte1 = Wire.read();
    int Byte2 = Wire.read();
    int XVal = ((Byte1<<24)|(Byte2<<16))>>20;

    Byte1 = Wire.read();
    Byte2 = Wire.read();
    int YVal = ((Byte1<<24)|(Byte2<<16))>>20;

    Byte1 = Wire.read();
    Byte2 = Wire.read();
    ZVal = ((Byte1<<24)|(Byte2<<16))>>20;

    ZVal = ZVal/FSR8_SCALE + Zoffset;

    Byte1 = Wire.read();
    int temp = (Byte1<<24)>>24;
    int tempf = 25 + temp*0.586;
  }
  else{ // else just read data into nothing to clear transmission out
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
  }
  
}