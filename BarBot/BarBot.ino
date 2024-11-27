#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""
#define LINE_TOKEN "" 
//blynk libraly
#include <BlynkSimpleEsp32.h>

#include <WiFi.h>

#include <WiFiClientSecure.h>
//libraly line notify
#include <TridentTD_LineNotify.h>


//เชื่อมรหัส
const char* ssid = "Coworking-P6";         
const char* password = "lumpini1234";      
//ลงline token


//กำหนดขา ultrasonic
#define trig_pin 23  
#define echo_pin 22 
//กำหนดขา relay
#define RELAY_PIN4 21
#define RELAY_PIN3 20
//กำหนด virtualPin Relay
#define VIRTUAL_RELAY1 V1
//กำหนดระดับน้ำแบบหวานปกติ
int time=1000 ; 
//หวานมาก
int time = 1500;

bool isRelayRunning = false; //ralay

//relay VirtualPin
BLYNK_WRITE(VIRTUAL_RELAY1) {
  int relayState = param.asInt(); 
  if (relayState == 1 && !isRelayRunning) {
    runRelaySequence();
  }
}
//ส่งค่า ระดับน้ำ ไปที่ blynk
BLYNK_READ(V2)
{
  
  Blynk.virtualWrite(V2, water_level_percent); // ส่งค่าไปยัง Virtual Pin V2
}

// menu แดงโซดา (relay กำหนดระยะเวลาการเปิดpump เมื่อ pulse เข้ามา1 ครั้ง )
void runRelaySequence() {
  isRelayRunning = true; 
  //sweet water
  digitalWrite(RELAY_PIN4, LOW);
  delay(5000); 
  digitalWrite(RELAY_PIN4, HIGH);
//หน่วงเวลา
  delay(4000);
 //Soda
  digitalWrite(RELAY_PIN3, LOW);
  delay(13000+time);
  digitalWrite(RELAY_PIN3, HIGH);

  isRelayRunning = false;
}





//ระยะความสูงของแทงค์น้ำ
const int tank_height = 22;



void setup() {
  Serial.begin(115200);
//ultrasonic PinMode
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
//relay pinMode
  pinMode(RELAY_PIN4, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
//กำหนดให้relay ปิดตอนเริ่มต้น
  digitalWrite(RELAY_PIN4, HIGH);
  digitalWrite(RELAY_PIN3, HIGH);

//blynk begin
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
//wifi begin
  WiFi.begin(ssid, password);
  //หาไวไฟ ไม่่ต้องมีก็ได้
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
//ให้line token เริ่มต้น
  LINE.setToken(LINE_TOKEN);
}

void loop() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(5);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH);
  //สูตรแปลงเป็น cm
  int distance = duration / 58;

  //สูตรแปลง cm เป็น % 
  int water_level_percent = max(0, min(100, (tank_height - distance) * 100 / tank_height));
//แสดง % และ cm 
  Serial.println("ระยะ: " + String(distance) + " cm");
  Serial.println("ระดับน้ำ: " + String(water_level_percent) + " %");
//ถ้าระดับน้ำอยู่ในช่วง 40 ถึง ถึง 50 ส่งแจ้งเตือนไปที่Line Notify
  if (water_level_percent <= 50 && water_level_percent>=40) {
    LINE.notify("ระดับน้ำ = " + String(water_level_percent) + " %");

  }

  delay(3000);
  //เริ่มทำงาน
   Blynk.run();
}
