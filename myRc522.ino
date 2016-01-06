/*
  Arduino RFID Access Control

  Security !

  To keep it simple we are going to use Tag's Unique IDs
  as only method of Authenticity. It's simple and not hacker proof.
  If you need security, don't use it unless you modify the code

  Copyright (C) 2015 Omer Siar Baysal

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <SPI.h>
#include <MFRC522.h>
#include <RFID.h>


#define MAX_CARD_NUMS 100
#define COMMON_ANODE

#ifdef COMMON_ANODE
#define LED_ON LOW
#define LED_OFF HIGH
#else
#define LED_ON HIGH
#define LED_OFF LOW
#endif

#define redLed 7		// Set Led Pins
#define greenLed 6
#define blueLed 5

///////////////////////////////////////
//    GLOBAL VARIABLE
///////////////////////////////////////
boolean match = false;
boolean programMode = false;

int successRead;

byte masterCard[4];
byte readCard[4];
byte whiteList[MAX_CARD_NUMS][4];


/*
	We need to define MFRC522's pins and create instance
	Pin layout should be as follows (on Arduino Uno):
	MOSI: Pin 11 / ICSP-4
	MISO: Pin 12 / ICSP-1
	SCK : Pin 13 / ICSP-3
	SS : Pin 10 (Configurable)
	RST : Pin 9 (Configurable)
	look MFRC522 Library for
	other Arduinos' pin configuration 
 */

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

///////////////////////////////////////
// void setup()
///////////////////////////////////////
void setup()
{
    //Protocol Configuration
    Serial.begin(9600);	 // Initialize serial communications with PC
    SPI.begin();           // MFRC522 Hardware uses SPI protocol
    mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
    
    //If you set Antenna Gain to Max it will increase reading distance
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    
    
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
        successRead = getID();            // sets successRead to 1 when we get read from reader otherwise 0
        digitalWrite(blueLed, LED_ON);    // Visualize Master Card need to be defined
        delay(200);
        digitalWrite(blueLed, LED_OFF);
        delay(200);
    } while (!successRead); 
    
    for( int i = 0; i < 4; i++ ){
        masterCard[i] = readCard[i];
    }
    Serial.println(F("-------------------"));
    Serial.println(F("Master Card's UID"));
    for ( int i = 0; i < 4; i++ ) {
        Serial.print(masterCard[i], HEX);
    }
    Serial.println("");
    Serial.println(F("-------------------"));
    Serial.println(F("Everything Ready"));
    Serial.println(F("Waiting PICCs to be scanned"));
    cycleLeds();    // Everything ready lets give user some feedback by cycling leds
}


///////////////////////////////////////
// void loop()
///////////////////////////////////////
void loop()
{
    do{
        successRead = getID();
    } while(!successRead);
    
    if(programMode){
        if( isMaster(readCard) ){
            Serial.println(F("Master Card Scanned"));
            Serial.println(F("Exiting Program Mode"));
            Serial.println(F("-----------------------------"));
            programMode = false;
            return;
        }
        else{
            Serial.print("A2R:MOD:");
            for( int i=0; i<4; i++ ){
                Serial.print(readCard[i], HEX);
            }
            Serial.println(":");
            // WAIT FOR RASPI REPLY, or not :P
        }
    }
    else{
        if( isMaster(readCard) ){
            programMode = true;
            Serial.println(F("Hello Master - Entered Program Mode"));
            Serial.println(F("Scan a PICC to ADD or REMOVE"));
            Serial.println(F("-----------------------------"));
        }
        else{
            Serial.print("A2R:ENQ:");
            for( int i=0; i<4; i++ ){
                Serial.print(readCard[i], HEX);
            }
            Serial.println(":");
            // WAIT FOR RASPI REPLY
            while( !Serial.available() );
            char ret = (char)Serial.read();
             
            if(ret == '1'){
                Serial.println("Access Granted");
                granted(300);
            }
            else{ 
                Serial.println("Access Denied!");
                denied();
            }
        }
    }
}


///////////////////////////////////////
// void granted()
///////////////////////////////////////
void granted (int setDelay) {
    digitalWrite(blueLed, LED_OFF); 	// Turn off blue LED
    digitalWrite(redLed, LED_OFF); 	// Turn off red LED
    digitalWrite(greenLed, LED_ON); 	// Turn on green LED
    //digitalWrite(relay, LOW); 		// Unlock door!
    delay(setDelay); 					// Hold door lock open for given seconds
    //digitalWrite(relay, HIGH); 		// Relock door
    delay(1000); 						// Hold green LED on for a second
}


///////////////////////////////////////
// void denied()
///////////////////////////////////////
void denied() {
    digitalWrite(greenLed, LED_OFF); 	// Make sure green LED is off
    digitalWrite(blueLed, LED_OFF); 	// Make sure blue LED is off
    digitalWrite(redLed, LED_ON); 	// Turn on red LED
    delay(1000);
}


///////////////////////////////////////
// int getID()
///////////////////////////////////////
int getID()
{
    // Getting ready for Reading PICCs
    if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
        return 0;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
        return 0;
    }
    // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
    // I think we should assume every PICC as they have 4 byte UID
    // Until we support 7 byte PICCs
    Serial.print(F("Scanned PICC's UID:"));
    for (int i = 0; i < 4; i++) {  //
        readCard[i] = mfrc522.uid.uidByte[i];
        Serial.print(readCard[i], HEX);
    }
    Serial.println("");
    mfrc522.PICC_HaltA(); // Stop reading
    return 1;
}

///////////////////////////////////////
// void cycleLeds()
///////////////////////////////////////
void cycleLeds() {
    digitalWrite(redLed, LED_OFF); 	// Make sure red LED is off
    digitalWrite(greenLed, LED_ON); 	// Make sure green LED is on
    digitalWrite(blueLed, LED_OFF); 	// Make sure blue LED is off
    delay(200);
    digitalWrite(redLed, LED_OFF); 	// Make sure red LED is off
    digitalWrite(greenLed, LED_OFF); 	// Make sure green LED is off
    digitalWrite(blueLed, LED_ON); 	// Make sure blue LED is on
    delay(200);
    digitalWrite(redLed, LED_ON); 	// Make sure red LED is on
    digitalWrite(greenLed, LED_OFF); 	// Make sure green LED is off
    digitalWrite(blueLed, LED_OFF); 	// Make sure blue LED is off
    delay(200);
}


///////////////////////////////////////
// void normalModeOn()
///////////////////////////////////////
void normalModeOn () {
    digitalWrite(blueLed, LED_ON); 	// Blue LED ON and ready to read card
    digitalWrite(redLed, LED_OFF); 	// Make sure Red LED is off
    digitalWrite(greenLed, LED_OFF); 	// Make sure Green LED is off
    // Make sure Door is Locked
}


///////////////////////////////////////
// boolean checkTwo()
///////////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL ) 			// Make sure there is something in the array first
    match = true; 			// Assume they match at first
  for ( int k = 0; k < 4; k++ ) { 	// Loop 4 times
    if ( a[k] != b[k] ) 		// IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) { 			// Check to see if if match is still true
    return true; 			// Return true
  }
  else  {
    return false; 			// Return false
  }
}


///////////////////////////////////////
// boolean isMaster()
///////////////////////////////////////
boolean isMaster( byte test[] ) {
    if ( checkTwo( test, masterCard ) )
        return true;
    else
        return false;
}
