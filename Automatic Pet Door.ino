/*
 Name:		Automatic_Dog's_door.ino
 Created:	11/8/2021 7:48:56 PM
 Author:	Sergio Oviedo Seas
*/

/*
* Arduino MEGA 2560
* 
* RC522 RFID module
*   GND - GND
*   VCC - 3.3V
*   RST - PIN 49
*   MISO - PIN 50
*   MOSI - PIN 51
*   SCK - PIN 52
*   SDA (SS) - PIN 53
*   REF: https://lastminuteengineers.com/how-rfid-works-rc522-arduino-tutorial/
* 
* TowerPro SG-92R Micro Servo
*   GND (BROWN CABLE) - GND
*   VCC (RED CABLE) - 5V
*   SIGNAL (YELLOW CABLE) - PIN 44 (PWM)
*   REF: https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
* 
* Infrared Obstacle Avoidance Module 1
*   GND - GND
*   VCC - 5V
*   Signal - PIN 40
*   REF: https://arduinomodules.info/ky-032-infrared-obstacle-avoidance-sensor-module/
* 
* Infrared Obstacle Avoidance Module 1
*   GND - GND
*   VCC - 5V
*   Signal - PIN 41
*   REF: https://arduinomodules.info/ky-032-infrared-obstacle-avoidance-sensor-module/
* 
* 28BYJ-48 Stepper Motor with ULN2003 Driver
*   GND - GND
*   VCC - External 5V
*   IN1 - PIN 8
*   IN2 - PIN 9
*   IN3 - PIN 10
*   IN3 - PIN 11
*   REF: https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
* 
* Slotted Infrared Sensor Upper Limit
*   GND -GND
*   VCC - 3.3V
*   Signal - PIN 30
* 
* Slotted Infrared Sensor Upper Limit
*   GND -GND
*   VCC - 3.3V
*   Signal - PIN 31
*/


#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Stepper.h>

#define RST_PIN 49
#define SS_PIN 53
#define SERVO_CONTROL

byte readCard[4];
String MasterTag = "E781DAC6";	// REPLACE this Tag ID with your Tag ID!!!
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo lock; // Creates a servo object in order to control the door's lock

// Create variables
int pos = 0; // Variable for the servo position
boolean obstruction = false; // Detects any obstruction in the door
boolean doorOpen = false; // Limit switch for door opened position
boolean doorClose = true; // Limit switch for door closed position
int detectObstacleSensor1 = 40; // Detect obstacle sensor 1
int detectObstacleSensor2 = 41; // Detect obstacle sensor 2
int detectObstacleSensorVal1; // Save the value of avoidance sensor 1
int detectObstacleSensorVal2; // Save the value of avoidance sensor 2
boolean petDetected = true; // Trigger variable for pet detection, if true = pet detected under the door


void setup()
{
    // Initiating
    Serial.begin(9600);
    SPI.begin(); // SPI bus
    mfrc522.PCD_Init(); // MFRC522
    lock.attach(9);
    pinMode(detectObstacleSensor1, INPUT); //Define input pin for avoidance sensor 1
    pinMode(detectObstacleSensor2, INPUT); //Define input pin for avoidance sensor 2
}

void loop()
{
    //Wait until new tag is available
    while (getID())
    {
        if (tagID == MasterTag)
       {
            Serial.println("Acceso autorizado");
            Serial.println(tagID);
            //lock.write(90);
            Serial.println(pos);
            checkLockPosition();
            
        }
        else
        {
            Serial.println("Acceso denegado");
            Serial.println(tagID);
        }
        delay(1000);
    }
}

//Read new tag if available
boolean getID()
{
    // Getting ready for Reading PICCs
    if (!mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
        return false;
    }
    if (!mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
        return false;
    }
    tagID = "";
    for (uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
    //readCard[i] = mfrc522.uid.uidByte[i];
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    return true;
}

void checkLockPosition()
{
    if (pos <= 0)
    {
        servoOpenLock();
    }
    else if (pos > 0 && pos < 180)
    {
        Serial.println("Lock not in close position, opening the door...");
        lock.write(180); // Open is the failsafe position
        //Serial.println(pos);
    }
    else if (pos >= 180 && !obstruction && doorClose)
    {
        servoCloseLock();
    }
    else
    {
        Serial.println("System error!");
    }
}

void servoOpenLock()
{
    for (pos = 0; pos <= 180; pos += 1)
    {
        lock.write(pos);
        //delay(15);
        //Serial.println(pos);
        tagID = "";
    }
    /*
    delay(5000);
    while (obstacleAvoidance())
    {
        Serial.println("Pet under the door!!!");
    }
    servoCloseMove();
    */
}

void servoCloseLock()
{
    for (pos = 180; pos >= 0; pos -= 1)
    {
        if (!obstacleAvoidance())
        {
            lock.write(pos);
            //delay(15);
            //Serial.println(pos);
            tagID = "";
        }
        else
        {
            servoOpenLock();
        }
    }
}

boolean obstacleAvoidance()
{
    detectObstacleSensorVal1 = digitalRead(detectObstacleSensor1);
    detectObstacleSensorVal2 = digitalRead(detectObstacleSensor2);

    if (detectObstacleSensorVal1 == LOW || detectObstacleSensorVal2 == LOW)
    {
        petDetected = true;
    }
    else
    {
        petDetected = false;
    }
    return petDetected;
}