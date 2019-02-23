#include <Keyboard.h>

//#include <Wire.h>

#define MAX_WAIT_FOR_TIMER 3
#define photo_id A0
#define LED_ID 13

unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta += 1 + (0xFFFFFFFF / period);    // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

typedef enum {EMPTY, FULL} STATE;

struct GPIO_t {
  int timer;
  unsigned long period;
  int pin;
  int etat;
};
struct GPIO_t photo;

struct led_t {
  GPIO_t gpio;      // gpio information
  int cpt;          // remaining time to blink
  unsigned mult_val;// total time need to blink
};
struct led_t led;

struct mailbox {
  STATE state;
  int val;
};
struct mailbox mb0 = {.state=EMPTY};

int key_s = 0;

//write
void loop_T1(struct mailbox *mb) {
  if (mb->state != EMPTY) return; // attend que la mailbox soit vide
  mb->val = analogRead(photo_id);
  mb->state = FULL;
}

//read
void loop_T2(struct mailbox *mb) {
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  // usage de mb->val
  Serial.print("Photo value : ");
  Serial.println(mb->val);
  led.cpt = (led.cpt > mb->val) ? mb->val : led.cpt;
  led.mult_val = mb->val;
  mb->state = EMPTY;
}

void loop_photo(struct GPIO_t *photo) {
  loop_T2(&mb0);
  if (!waitFor(photo->timer, photo->period)) return;
  loop_T1(&mb0);
}

void loop_led(struct led_t *led) {
  if (!waitFor(led->gpio.timer, led->gpio.period)) return;
  //Serial.print("Cpt value : ");
  //Serial.println(led->cpt);
  
  if (0>(led->cpt)){
    //cpt == 0
    led->gpio.etat = (led->gpio.etat+(1 >> key_s))%2;
    digitalWrite(led->gpio.pin, led->gpio.etat);
    led->cpt = led->mult_val;
  } else {
    --led->cpt;
  }
}

void serialEvent() {
  int i, c;
  for (i = 0; i < Serial.available(); i++) {
    c = Serial.read();
    //s
    //Serial.print(Serial.read());
    if (c == 115) { //s
      key_s = (key_s+1)%2;
    }
  }
}

/* ----------------------------------------------- */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  photo.timer = 0;
  photo.period = 500000;
  photo.pin=photo_id;
  
  led.gpio.timer = 1;
  led.gpio.period = 200;
  led.gpio.pin = LED_ID;
}

void loop() {
  // put your main code here, to run repeatedly:
  loop_photo(&photo);
  loop_led(&led);
}
