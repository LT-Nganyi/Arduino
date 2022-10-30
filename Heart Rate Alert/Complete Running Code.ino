#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <TimeLib.h>
#include <DS1307RTC.h>
//Create software serial object to communicate with SIM800L
TinyGPSPlus gps;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

SoftwareSerial simSerial(2, 3); //SIM800L Tx & Rx is connected to Arduino #2 & #3
SoftwareSerial gpsSerial(5, 6);// gps tx and rx 
#define button1 7
#define greenLed 11
#define redLed 12
#define Speaker 4
bool emergencyState;
int sensorPin = A0;    // select the input pin for the potentiometer
//int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
float sensorValueDeg = 0;

double latitude= 0.0;
double longitude = 0.0 ;

long instance1=0, timer;
double hrv =0, hr = 72, interval = 0;
int value = 0, count = 0;  
bool flag = 0;
#define shutdown_pin 10 
#define threshold 100 // to identify R peak
#define timer_value 10000 // 10 seconds timer to calculate hr
float celcius = 0.0;
long showTime = 0;
long beep = 0;
void setup()
{
  pinMode(button1, INPUT_PULLUP);
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  gpsSerial.begin(9600);
  //Begin serial communication with Arduino and SIM800L
 // mySerial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  Serial.println("Initializing..."); 
  lcd.print("Initialising");
  delay(3000);  
  showTime = millis();
  beep = millis();
}

void loop() { 
  digitalWrite(greenLed,HIGH);
  if (millis()-beep >= 1000){
    beep = millis();
    tone(Speaker,1000,150);
  }
   
  
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
    Serial.println("leads off!");
    digitalWrite(shutdown_pin, LOW); //standby mode
    instance1 = micros();
    timer = millis();
  }
  else {
    digitalWrite(shutdown_pin, HIGH); //normal mode
    value = analogRead(A2);
    value = map(value, 250, 400, 0, 100); //to flatten the ecg values a bit
    if((value > threshold) && (!flag)) {
      count++;  
      Serial.println("in");
      flag = 1;
      interval = micros() - instance1; //RR interval
      instance1 = micros(); 
    }
    else if((value < threshold)) {
      flag = 0;
    }
    if ((millis() - timer) > 10000) {
      hr = count*6;
      timer = millis();
      count = 0; 
    }
    hrv = hr/60 - interval/1000000;
    Serial.print(hr);
    Serial.print(",");
    Serial.print(hrv);
    Serial.print(",");
    Serial.println(value);
    delay(1);
  }

   //lcd.clear();
   lcd.setCursor(0,0);
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
      lcd.print("Disconnected!");
  }else{
    lcd.print("HR: ");
    lcd.print(hr);
    lcd.print(" BPM ");
  } 
  
 
  tmElements_t tm;
  if (RTC.read(tm))
  {
    //showTime = millis();
    //lcd.setCursor(0, 0); // Set the cursor on the third column and first row.
    //lcd.print("Time: ");
    //lcd.print(tm.Hour); // Print the string "Hello World!"
    //lcd.print(":");
    //lcd.print(tm.Minute); // Print the string "Hello World!"
    //lcd.print(":");
    //lcd.print(tm.Second);
    
    /*
    lcd.setCursor(0, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
    lcd.print("Date: ");
    lcd.print(tm.Day); // Print the string "Hello World!"
    lcd.print("/");
    lcd.print(tm.Month); // Print the string "Hello World!"
    lcd.print("/");
    lcd.print(tmYearToCalendar(tm.Year)); 
    delay(1000);
    */
  }
  //delay(1000);
  
    while(gpsSerial.available() > 0)
      gps.encode(gpsSerial.read()); 
  latitude = gps.location.lat();   
  longitude = gps.location.lng(); 
  Serial.println(latitude);
  Serial.println(longitude);
  Serial.println(gps.satellites.value());
  emergencyState = digitalRead(button1);
  sensorValue = analogRead(sensorPin);
  celcius = (((sensorValue/1024.0)*5000)/10)-300;
  Serial.println("Temperature:");
  Serial.println(sensorValue);
  
  if ((millis()-showTime) > 3000)
  {
  showTime = millis();
  lcd.setCursor(0,1);
  lcd.print("Temp:");
  lcd.print(int(celcius));
  lcd.print((char)223);
  lcd.print("C"); 
  }

  if ( emergencyState == LOW)
  {
    digitalWrite(greenLed,LOW);
    digitalWrite(redLed,HIGH);
    delay(200); 
    lcd.clear();
    
    String location = "https://www.google.com/maps/?q="+String(latitude,6)+ "," + String(longitude,6);
    String occurenceTime = String(tm.Hour)+ ":" + String(tm.Minute)+ ":" + String(tm.Second);
    String occurenceDate = String(tm.Day) + "/" + String(tm.Month) + "/" + String(tmYearToCalendar(tm.Year));
    String vitals =  "Heartrate: " +String(hr) + " , " + "Temperature: " + String(celcius)+ "°C.";
    
    Serial.println("emergency situation activated");
    lcd.setCursor(0,1);
    lcd.print("Sending EMS txt");
    //delay(3000);
    for ( int x =0; x<12; x++)
    {
      tone(Speaker,1000,200);
      delay(250);
       
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Lat:");
    lcd.print(latitude);
    lcd.setCursor(0,1);
    lcd.print("Lon:");
    lcd.print(longitude);
    delay(3000);
    simSerial.begin(9600);
    delay(1000);
    simSerial.println("AT");
    simSerial.println("AT+CMGF=1\r");
    delay(100);
    simSerial.println("AT+CMGS=\"+254712345678\"\r");  //Phone number to send alert to, example +212123456789"
    delay(500);
    simSerial.println("Possible Cardiac Arrest Emergency:");
    simSerial.println(vitals);
    simSerial.println(occurenceTime);
    simSerial.println(occurenceDate);
    simSerial.println(location);       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
    delay(500);
    simSerial.println((char)26);// (required according to the datasheet)
    delay(500);
    simSerial.println();
    Serial.println("Text Sent.");
    //updateSerial(); 
    delay(500);
    lcd.print("Text has been sent");
    delay(5000);
    // Begin Phone call 
    simSerial.println("AT");
    simSerial.println("ATD+ +254712345678;");
    lcd.print("Calling Number..");
    delay(20000); // wait for 20 seconds...
    simSerial.println("ATH"); //hang up
    
    simSerial.end();

    delay(2000);
    digitalWrite(redLed,LOW);
  }
  //delay(2000);
  
}
