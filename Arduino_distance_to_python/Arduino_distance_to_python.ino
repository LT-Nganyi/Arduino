#define trigPin 8
#define echoPin 9
#define green 10

long duration, distance;

void setup() {
  
  Serial.begin (115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  
  //if (distance >= 200 || distance <= 0){
  if (distance >= 300 || distance <= 0){
    //Serial.println("Out of range");
    

// Error fix for the lack of return signal
// Echo pin is triggered and turned back to input mode.    
pinMode(echoPin, OUTPUT); // Then we set echo pin to output mode
digitalWrite(echoPin, LOW); // We send a LOW pulse to the echo pin
delayMicroseconds(200);
pinMode(echoPin, INPUT); // And finaly we come back to input mode

}else{
  Serial.print(distance);
}


  delay(5000);
}
