#include <Wire.h>


const int led_id = 13;
const int photo_id = A0;
int val = 0;           // variable to store the value read
int switch_on = 0;
int prev = 0;


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  while (!Serial);
  pinMode(led_id, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    val = analogRead(photo_id);     // read the input pin
    if (val > 700) {
        switch_on = 1;
        if ( prev == 0 ) {
            Serial.println("+++++HIGH+++++");
            prev = 1;
        }
    }
    else {
      switch_on = 0;
      prev = 0;  
    }
    Serial.print("valeur de photoresistance : ");
    Serial.println(val);             // debug value
    digitalWrite(led_id, LOW);
    // fait une pause de 1 seconde
    delay(250/(switch_on*2+1));
    // éteint la LED
    digitalWrite(led_id, HIGH);
    // fait une pause de 1 seconde
    delay(250/(switch_on*2+1));
}
