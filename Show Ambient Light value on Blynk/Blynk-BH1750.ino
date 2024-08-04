#define BLYNK_TEMPLATE_ID           "TMPL6Y4j1W25k"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "eOdE4WymZkXHaibgj6j5u2reNvZGbxJ_"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "edeartd";
char pass[] = "edeartd1964";

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Wire.begin();  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  Blynk.run();  // Process Blynk communication

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  Blynk.virtualWrite(V0, lux);  // Send the light level value to a Virtual Pin in the Blynk app
  delay(1000);
}
