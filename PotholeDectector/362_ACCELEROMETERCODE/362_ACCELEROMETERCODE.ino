  #include <Adafruit_MMA8451.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
Adafruit_MMA8451 mma = Adafruit_MMA8451();
#include <LiquidCrystal.h>


int oldAcc = 0;
int BUTTON = 5;
int isPressed = 0;
float threshold = 1;
int counter = 0;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd (rs, en, d4, d5, d6, d7);

void getDamageLevel(int data) {
  if(data <= 5) {
    lcd.setCursor(0,0);
    lcd.print("Acc change:");
    lcd.print(data);
    lcd.setCursor(0,1);
    lcd.print("damage lvl:mod");
  }
  else if( data > 5) {
    lcd.setCursor(0,0);
    lcd.print("Acc change:");
    lcd.print(data);
    lcd.setCursor(0,1);
    lcd.print("damage lvl:high");
  }
  
}


void setup(void) {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  lcd.begin(16,2);
  
  mma.setRange(MMA8451_RANGE_2_G);
   
} 

void loop() { 
  // Read the 'raw' data in 14-bit counts
  mma.read();
  sensors_event_t event; 
  mma.getEvent(&event);

      
  if((oldAcc - event.acceleration.z ) >= threshold) {
    counter++;
    Serial.println("Bump");
    getDamageLevel(oldAcc - event.acceleration.z);
    Serial.write(1); //Write the serial data
    delay(2000);
  } 
 lcd.clear();
  oldAcc = event.acceleration.z;
  delay(1000);
  
}
