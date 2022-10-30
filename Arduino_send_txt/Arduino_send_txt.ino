String x;
String reply;



void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
}

void loop() {
  while (!Serial.available());
  //x = Serial.readString().toInt();
  x = Serial.readString();
  reply = "String Received: " + x ;
  Serial.print(reply);
}
