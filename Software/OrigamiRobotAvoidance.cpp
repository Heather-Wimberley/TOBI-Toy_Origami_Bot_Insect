#include <Arduino.h>

// initialise pins
const int IR1 = 4; //Right IR sensor
const int IR2 = 5; //Left IR sensor
const int motorAp = 42; //IN1; Motor A is on right
const int motorAm = 41; //IN2; Motor A is on right
const int motorBp = 40; //IN3; Motor B is on left
const int motorBm = 39; //IN4; Motor B is on left

//current speed and angular velocity
int v = 0; //1 is forwards; 0 is stopped; -1 is backwards
int w = 0; //1 is clockwise; 0 is stopped; -1 is anticlockwise

int spinFlag = 0;

//create global IR array
int object [2];

//declare all functions
void getIR(void);
void calculateSpeed(void);
void setSpeed(void);

void setup() {
    // set IR pins as input
    pinMode(IR1,INPUT);
    pinMode(IR2,INPUT);

    // set all motor pins as output
    pinMode(motorAp,OUTPUT);
    pinMode(motorAm,OUTPUT);
    pinMode(motorBp,OUTPUT);
    pinMode(motorBm,OUTPUT);
}

void loop() {
    getIR();
    calculateSpeed();
    setSpeed();
}

void getIR(){

    int sensorVal [2] = {0,0};
    // freq = 200 Hz
    for (int i = 0;i<20;i++){
        sensorVal[0] += (1-digitalRead(IR1));
        sensorVal[1] += (1-digitalRead(IR2));
        delay(5);
    }

    if (sensorVal[0]>2){
        object[0] = 1;}
    else {
        object[0] = 0;}
    
    if (sensorVal[1]>2) {
        object[1] = 1; }
    else {
        object[1] = 0; }
}

void calculateSpeed(){
    if (spinFlag == 0){
        if (object[0]==0 && object[1] == 0) {
            spinFlag = 0;
        }
        else {
            spinFlag = 1;
        }
    }
    else {
        if (spinFlag<24){
            spinFlag +=1;
        }
        else{
            spinFlag = 100;
        }
    }
    
}

void setSpeed() {
    if (spinFlag == 0)
    {
        digitalWrite(motorAp,HIGH);
        digitalWrite(motorAm,LOW);
        digitalWrite(motorBp,HIGH);
        digitalWrite(motorBm,LOW);
    }

    if (spinFlag > 0 && spinFlag < 10)
    {
        digitalWrite(motorAp,LOW);
        digitalWrite(motorAm,HIGH);
        digitalWrite(motorBp,LOW);
        digitalWrite(motorBm,HIGH);
        if (spinFlag == 100) {
            spinFlag = 0;
        }
    }

    if (spinFlag >= 10)
    {
        digitalWrite(motorAp,LOW);
        digitalWrite(motorAm,HIGH);
        digitalWrite(motorBp,HIGH);
        digitalWrite(motorBm,LOW);
        if (spinFlag == 100) {
            spinFlag = 0;
        }
    }
}