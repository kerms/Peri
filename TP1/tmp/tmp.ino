int LED = 13;
int PhotoRes = A0;
void setup() {
  // put your setup code here, to run once:
 pinMode(LED, OUTPUT);
 Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int current = 0, previous = 0; 
  int sw = 0; 
   int valeur = analogRead(A0);
     Serial.println(valeur);
  
if (valeur >= 250) {
  current = 1; 
}
if (current != previous && current == 1){
  sw = (sw + 1 ) % 2;
  previous = current ;
}
if (current != previous && current == 0){
  previous = current ;
}
if (sw == 1){
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(250);   
}
else{  
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
}
