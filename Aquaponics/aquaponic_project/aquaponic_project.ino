//soil sensor code from https://how2electronics.com/interface-capacitive-soil-moisture-sensor-arduino/ by ALEX NEWTON

const int AirValue = 600;   //you need to replace this value with Value_1
const int WaterValue = 350;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

//buzzer
const int PWM = 3 ; //set the button pin to 3 also contains the pulse width modulation for this type of button

//water sensor
int water_level /*adc_id*/ = 0; //the level first starts at 0 water sensor goes to analog pin 0
int HistoryValue = 0;
char printBuffer[128];

//temperature sensor libraries
#include "SimpleDHT.h"
#include "DHT.h"
#include "DHT_U.h"

//this is from the lcd lesson contained on arduino practice examples 
/*
  The circuit:
   LCD RS pin to digital pin 7
   LCD Enable pin to digital pin 8
   LCD D4 pin to digital pin 9
   LCD D5 pin to digital pin 10
   LCD D6 pin to digital pin 11
   LCD D7 pin to digital pin 12
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 7 Nov 2016
  by Arturo Guadalupi
  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12; //these are the different pins used on the board that match with lcd screen ports
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //calling the pins
#define DHTTYPE DHT11   // DHT 11
DHT dht(2, DHTTYPE); //put the pin number that senssor is connected to

//make sure RW is connected to ground
void setup() {
  Serial.begin(9600); //for water sensor

  //Green LED
  pinMode(31, OUTPUT); //pin 31
  //red LED
  pinMode(29, OUTPUT); //pin 29

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("");
  dht.begin(); //this makes sure the senor begins the measurments

}

void loop() {
  lcd.clear();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  float h = dht.readHumidity();  //this displays the humidity on LCD towards middle

  // Read temperature as Celsius (the default)
  lcd.setCursor (9, 0),
                lcd.print(h);
  lcd.setCursor (15, 0),
                lcd.print("H");
  lcd.setCursor(9, 0 ); //(column then number)
  //Serial.print(h); //prints humidity in serial monitor
  float t = dht.readTemperature(); // this displays the temperature in celcius
  lcd.setCursor (0, 0),
                lcd.print(t);
  lcd.setCursor (6, 0),
                lcd.print("C");
  lcd.setCursor(0, 1);
  //Serial.print(t); //prints celcius in serial monitor
  float f = dht.readTemperature(true); //this displays the temperature in fahrenheit
  lcd.setCursor (9, 1),
                lcd.print(f);
  lcd.setCursor (15, 1),
                lcd.print("F");
  lcd.setCursor(0, 1);
  //Serial.print(f); //prints fahrenheit in serial monitor

  //displays TEMPDIS
  lcd.setCursor(0, 1);
  lcd.print(printBuffer);

  //water sensor
  int value = analogRead(water_level); // get water_level value

  if (((HistoryValue >= value) && ((HistoryValue - value) > 10)) || ((HistoryValue < value) && ((value - HistoryValue) > 10)))
  {
    sprintf(printBuffer, "WL%d %d", water_level, value); //prints the value of the water level. /n creates the new line(for serial monitor)
    Serial.print(printBuffer); //prints the message above
    HistoryValue = value;
  }

  delay(100); // Check for new value every 1 sec
  //green LED
  if (value > 197) {
    digitalWrite(31, HIGH); //digital port 31 Green
    digitalWrite(29, LOW); //digital port 31
  }

  //buzzer goes off if system is stable or not also the lcd prints to add water.
  if (value < 196) {
    lcd.clear();
    lcd.print("Add Water");
    //lcd.clear();
    delay(500);
    tone (PWM, 440, 500);
    delay(500);
    digitalWrite(29, HIGH); //digital port 29 turns on red for unstable
    digitalWrite(31, LOW); //digital port 31 shuts off green
    lcd.clear();
  }//buzz
  delay(250);

  if (value > 196) { //if the water level sensor is greater than 196 then all is green 
    
    //soil sensor print in serial
    soilMoistureValue = analogRead(A1);  //put Sensor insert into soil
    Serial.println(soilMoistureValue); //prints the value of the soilmosture level on serial line
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100); //map changes values and makes it into ranges either positive or negative
    if (soilmoisturepercent >= 100)
    {
      Serial.println("100 %"); //if the level is more than 100 then the level printed is 100
      delay(5000);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(soilmoisturepercent);
      lcd.print("%");
    }
    else if (soilmoisturepercent <= 0)//if there is no moisture in soil then there is an error 
    {
    lcd.clear();
    lcd.print("Check Soil");
    //lcd.clear();
    delay(500);
    tone (PWM, 440, 500);
    delay(500);
    digitalWrite(29, HIGH); //digital port 29 turns on red for unstable
    digitalWrite(31, LOW); //digital port 31 shuts off green
    lcd.clear();
    delay(250);
    
    Serial.println("0 %"); //if the level is less than 100 then the level printed is 0
//    delay(5000);
//    lcd.clear();
//    lcd.setCursor(0, 1);
//    lcd.print(soilmoisturepercent);
//    lcd.print("%");
    }
    else if (soilmoisturepercent > 0 && soilmoisturepercent < 100)
    {
      Serial.print(soilmoisturepercent); //prints the value of the soil from the sensor
      Serial.println("%");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Soil Moisture:");
      lcd.setCursor(0, 1);
      lcd.print(soilmoisturepercent);
      lcd.print("%");
      delay(3000);

    }
  }
if (value < 196 && soilmoisturepercent <= 100){
  lcd.clear();
    lcd.print("Check Water and");
    lcd.setCursor(0,1);
    lcd.print("Soil");
    //lcd.clear();
    delay(500);
    tone (PWM, 440, 500);
    delay(500);
    digitalWrite(29, HIGH); //digital port 29 turns on red for unstable
    digitalWrite(31, LOW); //digital port 31 shuts off green
    lcd.clear();
    delay(250);
}
}//loop
