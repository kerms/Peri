#define MAX_WAIT_FOR_TIMER 3
#define photo_id A0

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

struct GPIO_t photo = {.timer = 0, .period = 500000, .pin=photo_id};
struct GPIO_t led = {.timer = 1, .period = 50000, .pin=13};

struct mailbox {
  STATE state;
  int val;
};
struct mailbox mb0 = {.state=EMPTY};

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
  Serial.println(mb->val);
  led.period = 1000000/(1000*5/mb->val);
  mb->state = EMPTY;
}

void loop_photo(struct GPIO_t *photo) {
  loop_T2(&mb0);
  if (!waitFor(photo->timer, photo->period)) return;
  loop_T1(&mb0);
}

void loop_led(struct GPIO_t *led) {
  if (!waitFor(led->timer, led->period)) return;
  digitalWrite(led->pin, led->etat);
  led->etat = (led->etat+1)%2;
}


/* ----------------------------------------------- */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  loop_photo(&photo);
  loop_led(&led);
}
