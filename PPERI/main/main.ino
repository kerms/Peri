/*
 Basic HELTEC ESP32 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the HELTEC ESP32 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
*/

#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "OLEDDisplayUi.h"

#define MAX_WAIT_FOR_TIMER 10

/* Constantes pour les broches */
const byte TRIGGER_PIN = 5; // Broche TRIGGER
const byte ECHO_PIN = 23;    // Broche ECHO

// Update these with values suitable for your network.

const char* ssid = "dd-wrt";
const char* password = "peri2019";

//const char* ssid = "NETGEAR23";
//const char* password = "dynamicapple788";
const char* IN_TOPIC="inTopic6";
const char* OUT_TOPIC="inTopic6";
//const char* mqtt_server = "192.168.1.99";
const char* mqtt_server = "192.168.1.46";

static unsigned long ULTRASON_TIMER;
static unsigned long OLED_TIMER;
static unsigned long ULTRASON_VAL_TIMER;
static unsigned long TOUCH_TIMER;

/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s

/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;

long measure;
SSD1306  display(0x3c, 4, 15);
OLEDDisplayUi ui     ( &display );

int mm = 13; //blue
int cm = 12; //green
int m  = 2;  //red


uint64_t chipid; 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[75];
int value = 0;

void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "HeltecESP32LoraClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(OUT_TOPIC, "init");
      // ... and resubscribe
      //client.subscribe(IN_TOPIC);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

unsigned int waitFor(unsigned long *timer, unsigned long period){
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - *timer;              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta += 1 + (0xFFFFFFFF / period);    // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) *timer = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

/* affiche la valeur du capteur ultrason  sur l'OLED chaque 0.5 sec */
void loop_oled(float distance_mm) {
  if (!waitFor(&ULTRASON_TIMER, 1000000)) return;
  display.drawString(0, 0,  "Distance: ");
  display.drawString(0, 10, String(distance_mm) +  "mm");
  display.drawString(0, 20, String(distance_mm / 10.0, 2) + "cm");
  display.drawString(0, 30, String(distance_mm / 1000.0, 2) + "m");  
  //Serial.println("OLED !");
  
}
/* envoyer la valeur de la distance en metre toutes les 3 sec */ 
void loop_ultrason_send(float distance_mm) {
  if (!waitFor(&OLED_TIMER, 3000000)) return;
  (*(int *)&msg) = 1;
  (*(float *)&msg[4]) = distance_mm;
  
  Serial.println("Value distance : " + String(distance_mm));
  client.beginPublish (OUT_TOPIC, 8, 0);
  client.write((uint8_t*)msg, 4);
  client.write((uint8_t*)&msg[4],4);
  client.endPublish();
}

/* recuperer la valeur du capteur ultrason toutes les 10 usec */
void loop_ultrason_val(){
  if (!waitFor(&ULTRASON_TIMER, 10000)) return;
  // clear the display
  display.clear();
  /*  Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  /* Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
  measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
}


void loop_touch_pin(float distance_mm){
  if (!waitFor(&TOUCH_TIMER, 10000)) return;
  long now = millis();
  
  if (now - lastMsg > 1000) 
  {
    
    //Serial.println("Value touch ok : " + String(touchRead(mm)));
    int touch_val = touchRead(mm);
    if (touch_val < 20 && touch_val>2) 
    {
      snprintf (msg, 75, "distance : %f mm", distance_mm);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(OUT_TOPIC, msg);
      lastMsg = now;
      ++value;
    }
    
    
    //Serial.println("Value touch not ok : " + String(touchRead(touch_nok)));
    touch_val = touchRead(cm);
    if(touch_val < 15 && touch_val>2) {
      snprintf (msg, 75, "distance : %f cm", distance_mm / 10.0, 2);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(OUT_TOPIC, msg);
      lastMsg = now;
      ++value;
    }

    //Serial.println("Value touch not ok : " + String(touchRead(touch_nok)));
    touch_val = touchRead(m);
    if(touch_val < 15 && touch_val>2) {
      snprintf (msg, 75, "distance : %f m", distance_mm / 1000.0, 2);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(OUT_TOPIC, msg);
      lastMsg = now;
      ++value;
    }
  }

}


void setup() 
{
    /* Init ultra son */
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);



  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  ULTRASON_TIMER = 0;
  OLED_TIMER = 0;
  delay(3000);
  
  display.drawString(0, 0, "Hello world");
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}



void loop() 
{

  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  /* Calcul la distance à partir du temps mesuré */
  float distance_mm = measure / 2.0 * SOUND_SPEED;
  
  loop_ultrason_val();
  loop_ultrason_send(distance_mm);
  loop_oled(distance_mm);
  //loop_touch_pin(distance_mm);
}
