void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

int counter = 0;

void loop() {
  // put your main code here, to run repeatedly: 
  Serial.print("Count: ");
  Serial.print(counter++);
  Serial.println();
  delay(1000);
}
