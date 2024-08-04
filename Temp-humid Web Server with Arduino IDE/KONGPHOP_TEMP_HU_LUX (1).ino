#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#include <Wire.h>
#include <BH1750.h>
#include <ESP8266HTTPClient.h>

//----------------------------------------
//#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321

const int DHTPin = D4; // เปลี่ยนนี้เป็นขา sda ที่ต้องการ

BH1750 lightMeter; // สร้างอ็อบเจ็กต์เซ็นเซอร์แสง

String t;

#define ON_Board_LED 2 //--> Defining an On Board LED, used for indicators when the

//----------------------------------------
// SSID dan Password wifi mu gan.
const char* lineNotifyToken = "g1ZBZYhTZKWl2YaqUpRhb2cd8f2dnMAIYmshN2cb7j0";
float temperature = 0.0; // ตัวแปรอุณหภูมิ
float lux = 0.0; // ตัวแปรค่าแสง
const char* ssid = "edeartd"; //--> Nama Wifi / SSID.
const char* password = "edeartd1964"; //--> Password wifi .

//----------------------------------------
//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;

//----------------------------------------
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
WiFiClientSecure client; //--> Create a WiFiClientSecure object.

// Timers auxiliary variables
long now = millis();
long lastMeasure = 0;
String GAS_ID = "AKfycbwRnlrv80Htu7vZrzf5S8vJ0apvKjewO6gwLmsyjTALj_JcoPXGbD4GrXm4_ED88zB8"; //--> spreadsheet script ID

// Function declarations
void sendData(float tempValue, float humiValue, float lightValue);
void sendLineNotify();

//============================================ void setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  delay(500);
  dht.begin();
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
  pinMode(ON_Board_LED, OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //-->

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to
  //----------------------------------------If successfully connected to the wifi router, the IP Address
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------
  client.setInsecure();
  // รับข้อมูลจากเซ็นเซอร์แสง BH1750
  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  now = millis();
  // Publishes new temperature and humidity every 3 seconds
  if (now - lastMeasure > 3000) {
    lastMeasure = now;
    // อ่านค่าแสงจากเซ็นเซอร์แสง BH1750
    float lux = lightMeter.readLightLevel();
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Computes temperature values in Celsius
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    // Uncomment to compute temperature values in Fahrenheit
    // float hif = dht.computeHeatIndex(f, h);
    // static char temperatureTemp[7];
    // dtostrf(hif, 6, 2, temperatureTemp);
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    Serial.print("Light: ");
    Serial.print(lux);
    Serial.print(" lx");
    Serial.print("  Humidity: ");
    Serial.print(h);
    Serial.print("  Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F  ");
    // Serial.print(hif);
    // Serial.println(" *F");
    sendData(t, h, lux);
    static unsigned long lastLineNotifyTime = 0;
    const unsigned long lineNotifyInterval = 3600000; // 1 ชั่วโมง
    if (millis() - lastLineNotifyTime >= lineNotifyInterval) {
      lastLineNotifyTime = millis();
      sendLineNotify();
    }
  }
}

//*****

//======================================================================
void sendLineNotify() {
  HTTPClient http;

  // URL ของ Line Notify
  String lineNotifyURL = "https://notify-api.line.me/api/notify";
  float lux = lightMeter.readLightLevel();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // ข้อความที่คุณต้องการส่ง
  String message = "\nTemperature: " + String(temperature) + " *C\n";
  message += "Humidity: " + String(humidity) + "\n";
  message += "Light: " + String(lux) + " lx";

  // เตรียม Header สำหรับ Line Notify
  http.begin(client, lineNotifyURL); // แก้ไขนี้
  http.addHeader("Authorization", "Bearer " + String(lineNotifyToken));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // เตรียมข้อมูลที่จะส่งไปยัง Line Notify
  String postData = "message=" + message;
  // Send the POST request
  int httpResponseCode = http.POST(postData);

  // Capture and log the server response
  String response = http.getString();
  Serial.print("Server Response: ");
  Serial.println(response);

  // Check the response
  if (httpResponseCode == 200) {
    Serial.println("Line Notify sent successfully!");
  } else {
    Serial.print("Line Notify failed with error code: ");
    Serial.println(httpResponseCode);
  }

  // ปิดการเชื่อมต่อ
  http.end();
  delay(1000); // รอ 1 วินาทีก่อนส่งข้อมูลใหม่
}

void sendData(float tempValue, float humiValue, float lightValue) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------
  //----------------------------------------Proses dan kirim data
  float string_temp = tempValue;
  float string_humi = humiValue;
  float string_lightValue = lightValue;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi + "&Lux=" + string_lightValue;
  // 2 variables
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------
  //---------------------------------------
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successful!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  delay(60000);
}
