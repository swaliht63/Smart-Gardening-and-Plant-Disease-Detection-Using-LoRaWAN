#include <TheThingsNetwork.h>

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
const char *devAddr = "00f95747";
const char *nwkSKey = "b6c3a045457ec0819fa39dff9c64dcb1";
const char *appSKey = "EC1AF2B6DF884B1AEDC71CE695DDAB34";
byte payload[3];

// if the soil is dryer than this number, then start watering
#include <dht.h>


#define dht_apin 6 // Analog Pin sensor is connected to
 
dht DHT;
 
const int dry = 270;

const int pumpPin = 12;
const int soilSensor = A4;

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_IN865_867

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);
   pinMode(pumpPin, OUTPUT);
  pinMode(soilSensor, INPUT);
  digitalWrite(pumpPin, HIGH);
  delay(5000);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();
}

void loop()
{
  
  debugSerial.println("-- LOOP");
  sensor();

  // Prepare payload of 1 byte to indicate LED status
  

  // Send it off
  ttn.sendBytes(payload, sizeof(payload));

  delay(5000);
  
}

void sensor()
{
  int moisture = analogRead(soilSensor);
  moisture = map(moisture, 0, 1024, 0, 100);
  Serial.println(moisture);
  delay(5000);
  
  if (moisture <=30) {
    // the soil is too dry, water!
    Serial.println("Watering starts now..moisture is " + String(moisture));
    digitalWrite(pumpPin, HIGH);

    // keep watering for 5 sec
    delay(5000);

    // turn off water
    digitalWrite(pumpPin, LOW);
    Serial.println("Done watering.");
  } else {
    Serial.println("Moisture is adequate. No watering needed " + String(moisture));
  }
  DHT.read11(dht_apin);
    
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
    
    delay(5000);
    payload[0] = moisture;
    payload[1] = DHT.humidity;
    payload[2] = DHT.temperature;
}
