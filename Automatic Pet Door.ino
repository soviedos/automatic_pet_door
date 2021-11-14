/*
 Name:		Automatic_Dog's_door.ino
 Created:	11/8/2021 7:48:56 PM
 Author:	Sergio Oviedo Seas
*/


#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 5
#define SS_PIN 10
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
boolean doorOpen = false;
boolean doorClose = true;


void setup()
{
    // Initiating
    SPI.begin(); // SPI bus
    mfrc522.PCD_Init(); // MFRC522
    lock.attach(9);
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
        delay(5000);
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
        servoOpenMove();
    }
    else if (pos > 0 && pos < 180)
    {
        Serial.println("Lock not in close position, opening the door...");
        lock.write(180); // Open is the failsafe position
        Serial.println(pos);
    }
    else if (pos >= 180 && !obstruction && doorClose)
    {
        servoCloseMove();
    }
    else
    {
        Serial.println("System error!");
    }
}

void servoOpenMove()
{
    for (pos = 0; pos <= 180; pos += 1)
    {
        lock.write(pos);
        delay(15);
        Serial.println(pos);
        tagID = "";
    }
}

void servoCloseMove()
{
    for (pos = 180; pos >= 0; pos -= 1)
    {
        lock.write(pos);
        delay(15);
        Serial.println(pos);
        tagID = "";
    }
}