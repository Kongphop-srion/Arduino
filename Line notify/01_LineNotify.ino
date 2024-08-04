/* 
 * ไลบรารี่ TridentTD_LineNotify version 2.1
 * ใช้สำหรับ ส่ง แจ้งเตือนไปยัง LINE สำหรับ ESP8266 และ ESP32
 * สามารถส่งได้ทั้ง ข้อความ , สติกเกอร์ และรูปภาพ(ด้วย url)
 * -----------------------------------------------------
 * ให้ save เป็น file ต่างหากก่อนถึงจะส่ง Line Notify ภาษาไทยได้
 */

#include <TridentTD_LineNotify.h>

#define SSID        "edeartd"
#define PASSWORD    "edeartd1964"
#define LINE_TOKEN  "jd38iHTzoybvgwM75LQZn9gQi8n2nUY3i2whJBk8vvH"

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);

  // ตัวอย่างส่งข้อความ
  LINE.notify("TEST- HELLO LINE NOTIFY");

  // ตัวอย่างส่งข้อมูล ตัวเลข
  LINE.notify(1234);          // จำนวนเต็ม
  LINE.notify(212.43434,5);   // จำนวนจริง แสดง 5 หลัก

  // เลือก Line Sticker ได้จาก https://devdocs.line.me/files/sticker_list.pdf
  LINE.notifySticker(3,240);        // ส่ง Line Sticker ด้วย PackageID 3 , StickerID 240
  LINE.notifySticker("Hello",1,2);  // ส่ง Line Sticker ด้วย PackageID 1 , StickerID 2  พร้อมข้อความ

  // ตัวอย่างส่ง รูปภาพ ด้วย url
  LINE.notifyPicture("https://www.fitm.kmutnb.ac.th/news/FITM_logo.jpg");
  LINE.notifyPicture("FITM","https://www.fitm.kmutnb.ac.th/news/FITM_logo.jpg");
}

void loop() {
  delay(1000);
}
