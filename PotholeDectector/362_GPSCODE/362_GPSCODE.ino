

/* Names: Simon Acosta(sacost6), Danny Belmonte(dbelmo2)
 * Team number: 70
 * Project Name: Pothole Detector 
 * 
 * Abstract: The Pothole Alarm and Register will detect potholes and record it to a MySQL database using a python script. 
 * It will warn users if it is within 50 feet of a pothole by constantly checking the user’s location and comparing it to previously-stored coordinates in the database. 
 * The project will also warn users about how much damage the pothole may have done to the vehicle’s suspension using a rating system that will rate the severity of the pothole from 1-5. 
 * The project was inspired by the rocky Chicagoland streets and Chicago’s (and surrounding suburbs’)  unwillingness to do anything about them.  
 *  
 * CANNOT DO A FULL LIVE DEMO BECAUSE GPS DOES NOT CONNECT INSIDE OF SEL
 * 
 */
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 3
// Connect the GPS RX (receive) pin to Digital 2

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
int readB;
const int buzzer = 9;

#define GPSECHO  true

void serialFlush() {
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

void setup()
{

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);

  // Set-up buzzer 
  pinMode(buzzer, OUTPUT);
}

uint32_t timer = millis();
void loop()                    
{
  boolean ree = false; 
  float latitude, longitude; 
  char arr[10];
  
  char c = GPS.read();


  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;  
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    // Check the serial comm for information, if none print regular coordinates
    // to be checked in database by python script
    if(Serial.available() == 0) {
      if(GPS.fix) {
        Serial.print(GPS.latitude, 4);
          Serial.print(",");
          Serial.println(GPS.longitude, 4);
      }
    }
    // if there is information availabe to be read,
    // parse it to check if it is a bump approaching 
    // or if it is a bump to be recorded to the database by the python script
    while(Serial.available() > 0) {
     Serial.readBytes(arr, 4);
     if(arr[0] == 'B' && arr[1] == 'u') {
        tone(buzzer, 500); // Send 1KHz sound signal...
        delay(2000);        // ...for 1 sec
        noTone(buzzer);     // Stop sound...
     }
     else {
       serialFlush();
       Serial.println("Accelerometer detected bump!");
       if (GPS.fix) {
          Serial.print(GPS.latitude, 4);
          Serial.print("bump");
          Serial.println(GPS.longitude, 4);
       }
     }
    }
    
  }

}
